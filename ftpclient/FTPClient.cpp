#include "BaseInc.h"
#include "FTPClient.h"
#include "tool.h"
namespace FTPSocket {
	void FTPClient::Login(std::wstring  wAddr, std::wstring  wUsr, std::wstring  wPwd)
	{
		std::string addr = WString2String(wAddr);
		std::string usr = WString2String(wUsr);
		std::string pwd = WString2String(wPwd);

		//连接
		cmdClient.Init(WString2String(wAddr), 21);
		cmdClient.StartReceiveThread();
		cmdClient.name = "cmdClient";
		//memset(recvBuff, 0, RecvSize);
		//cmdClient.Receive(recvBuff, RecvSize);

		FSCommand cell[2];
		Enque("loginuser", cell);
		//登录用户
		memset(sendBuff, 0, SendSize);
		sprintf(sendBuff, "USER %s\r\n", usr.data());
		cmdClient.Send(sendBuff, strlen(sendBuff));
		//memset(recvBuff, 0, RecvSize);
		//cmdClient.Receive(recvBuff, RecvSize);
		cell[0].WaitResult();


		//密码
		Enque("loginpass", cell + 1);
		memset(sendBuff, 0, SendSize);
		sprintf(sendBuff, "PASS %s\r\n", pwd.data());
		cmdClient.Send(sendBuff, strlen(sendBuff));
		memset(recvBuff, 0, RecvSize);
		//cmdClient.Receive(recvBuff, RecvSize);
		cell[1].WaitResult();

	}

	void FTPClient::Pasv()
	{
		curFtpCmd = FTPSocket::Pasv;
		if (dataServer.GetIsRunning())
		{
			dataServer.Close();
		}
		FSCommand cell;
		Enque("Pasv", &cell);

		//开启被动模式后，需要
		memset(sendBuff, 0, SendSize);
		sprintf(sendBuff, "PASV\r\n");
		cmdClient.Send(sendBuff, strlen(sendBuff));
		//memset(recvBuff, 0, RecvSize);
		//cmdClient.Receive(recvBuff, RecvSize);
		cell.WaitResult();
		serverMode = PasvMode;
		string str(cell.GetContent());
		if (str.size() == 0) return;
		size_t posS = str.find_first_of("(");
		size_t posE = str.find_first_of(")");
		str = str.substr(posS + 1, posE - posS - 1);
		stringstream ss;
		ss << str;
		char ip[255];
		int port;
		string tmp[4];
		string p1, p2;
		std::getline(ss, tmp[0], ',');
		std::getline(ss, tmp[1], ',');
		std::getline(ss, tmp[2], ',');
		std::getline(ss, tmp[3], ',');
		std::getline(ss, p1, ',');
		std::getline(ss, p2, ',');
		//ss >> tmp[0] >> tmp[1] >> tmp[2] >> tmp[3] >> p1 >> p2;
		sprintf(ip, "%s.%s.%s.%s", tmp[0].data(), tmp[1].data(), tmp[2].data(), tmp[3].data());
		port = stoi(p1) * 256 + stoi(p2);
		//连接成功
		this->dataIp = ip;
		this->dataPort = port;
		//dataClient.Init(ip, port);
		//dataClient.StartReceiveThread();
		//dataClient.name = "dataClient";
	}

	std::wstring FTPClient::List()
	{
		curFtpCmd = FTPSocket::List;
		FSCommand cell;
		cell.nMaxTimes = 2;
		Enque("List", &cell);
		std::function<void(string, SOCKET, char*, int)> dataReceive = [&](string mode, SOCKET s, char* buff, int size) {
			if (mode == "msg")
				listCmd.contents.push_back(buff);
		};

		if (serverMode == PasvMode)
		{
			NewPasvConnect(dataReceive);
			//a connection for this call, it is a tcp client
			//std::bind(&FTPClient::OnRecvDataChannelPasvMode, /*pasvConn.back(),*/this,
			//std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		}
		else {
			dataServer.RecvHandler = dataReceive;
		}

		memset(sendBuff, 0, SendSize);
		sprintf(sendBuff, "LIST\r\n");
		cmdClient.Send(sendBuff, strlen(sendBuff));
		cell.WaitResult();

		::Sleep(200);
		std::string resStr;
		for (int i = 0; i < listCmd.contents.size(); i++)
		{
			resStr += listCmd.contents[i];
		}
		//ParseListAck(resStr);
		if (!resStr.empty())
		{
			vector<FileInfo> vctFiles;
			GetFileListFromContent(resStr, vctFiles);
		}
		listCmd.Reset();
		//if(dataServer.clientList.size() > 0)
		//dataServer.ClientOffline(dataServer.clientList.back());
		if (serverMode == PasvMode)
		{
			RemovePasvConnect();
		}
		else {
			dataServer.ClientOffline(dataServer.clientList.back());
		}
		return String2WString(resStr);
	}

	void FTPClient::ListAllFileAndFolders(std::vector<FileInfo>& files)
	{
		curFtpCmd = FTPSocket::List;
		FSCommand cell;
		cell.nMaxTimes = 2;
		Enque("List", &cell);
		std::function<void(string, SOCKET, char*, int)> dataReceive = [&](string mode, SOCKET s, char* buff, int size) {
			if (mode == "msg")
				listCmd.contents.push_back(buff);
		};

		if (serverMode == PasvMode)
		{
			NewPasvConnect(dataReceive);
			//a connection for this call, it is a tcp client
				//std::bind(&FTPClient::OnRecvDataChannelPasvMode, /*pasvConn.back(),*/this,
				//std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		}
		else {
			dataServer.RecvHandler = dataReceive;
		}

		memset(sendBuff, 0, SendSize);
		sprintf(sendBuff, "LIST\r\n");
		cmdClient.Send(sendBuff, strlen(sendBuff));
		cell.WaitResult();
		::Sleep(200);
		std::string resStr;
		for (int i = 0; i < listCmd.contents.size(); i++)
		{
			resStr += listCmd.contents[i];
		}
		//ParseListAck(resStr);
		if (!resStr.empty())
		{
			GetFileListFromContent(resStr, files);
		}


		listCmd.Reset();

		if (serverMode == PasvMode)
		{
			RemovePasvConnect();
		}
		else {
			dataServer.ClientOffline(dataServer.clientList.back());
		}
	}

	void FTPClient::Port()
	{
		serverMode = PortMode;
		curFtpCmd = FTPSocket::Port;
		/*if (dataClient.GetIsRunning())
		{
			dataClient.Close();
		}*/
		FSCommand cell;
		Enque("Port", &cell);
		int port = 22020;
		dataServer.Init(22020);

		//dataServer.name = "dataServer";
		dataServer.StartRecvThread();	//这个模式后通过，下一个命令会连接上
		memset(sendBuff, 0, SendSize);
		sprintf(sendBuff, "%s", GetPortCmd(port).data());
		cmdClient.Send(sendBuff, strlen(sendBuff));
		cell.WaitResult();

		//dataServer.AcceptThread();
		//dataServer.StartReceiveThread();

		/*memset(recvBuff, 0, RecvSize);
		cmdClient.Receive(recvBuff, RecvSize);*/
	}

	void FTPClient::Exit()
	{
		curFtpCmd = FTPSocket::Exit;
		memset(sendBuff, 0, SendSize);
		sprintf(sendBuff, "QUIT\r\n");
		cmdClient.Send(sendBuff, strlen(sendBuff));
		//memset(recvBuff, 0, RecvSize);
		//cmdClient.Receive(recvBuff, RecvSize);
		if (cmdClient.GetIsRunning()) cmdClient.Close();
		//if (dataClient.GetIsRunning()) dataClient.Close();
		if (dataServer.GetIsRunning()) dataServer.Close();

		for (int i = 0; i < pasvConn.size(); i++)
		{
			if (pasvConn[i].GetIsRunning())
				pasvConn[i].Close();
		}

	}

	std::wstring FTPClient::Pwd()
	{
		curFtpCmd = FTPSocket::Pwd;
		memset(sendBuff, 0, SendSize);
		sprintf(sendBuff, "PWD\r\n");
		cmdClient.Send(sendBuff, strlen(sendBuff));
		//memset(recvBuff, 0, RecvSize);
		//cmdClient.Receive(recvBuff, RecvSize);
		//std::string recvStr = cmdClient.Receive();
		//257 "/" is current directory.
		std::string resultDir = recvBuff;
		int nPos = resultDir.find('"');
		int nLastPos = resultDir.rfind('"');
		std::string dir = SubString(resultDir, '"', '"');
		return String2WString(dir);
	}
	void FTPClient::Cwd(std::string workDir)
	{
		curFtpCmd = FTPSocket::Cwd;
		memset(sendBuff, 0, SendSize);
		sprintf(sendBuff, "CWD %s\r\n", workDir.data());
		cmdClient.Send(sendBuff, strlen(sendBuff));
		//memset(recvBuff, 0, RecvSize);
		//cmdClient.Receive(recvBuff, RecvSize);
	}


	void FTPClient::MakeDiectory(std::wstring wDir)
	{
		std::string dir = WString2String(wDir);

		curFtpCmd = FTPSocket::MakeDir;
		memset(sendBuff, 0, SendSize);
		sprintf(sendBuff, "MKD %s\r\n", dir);
		cmdClient.Send(sendBuff, strlen(sendBuff));
		memset(recvBuff, 0, RecvSize);
		//cmdClient.Receive(recvBuff, RecvSize);
	}

	void FTPClient::Retr(wstring wServerFile, wstring wDstFile, int fileSize, IFileTransferObserver* observer)
	{
		int taskID = -1;
		if (serverMode == PasvMode)
		{
			taskID = pasvConn.size();
			//a connection for this call, it is a tcp client
				//std::bind(&FTPClient::OnRecvDataChannelPasvMode, /*pasvConn.back(),*/this,
				//std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		}
		else {
			taskID = dataServer.clientList.size();
		}


		std::function<void(string, SOCKET, char*, int)> dataReceive = [&](string mode, SOCKET s, char* buff, int count) {
			if (mode == "disconnect")
			{
				observer->DownloadFinishCallBack(taskID, String2WString(downloadFileTask[taskID].fileName).data());
				ofs.close();
				return;
			}
			if (!ofs.is_open())
				cout << "ifs not open" << endl;
			if (count > 0)
			{
				ofs.write(buff, count);

				if (downloadFileTask.find(taskID) != downloadFileTask.end())
				{
					downloadFileTask[taskID].curTransFileSize += count;
					float percent = (float)downloadFileTask[taskID].curTransFileSize / (float)downloadFileTask[taskID].fileSize;
					if (observer != nullptr)
					{
						observer->DownloadFileProgressCallBack(taskID, percent, String2WString(downloadFileTask[taskID].fileName).data());
					}
				}
			}
			ofs.flush();
		};
		if (serverMode == PasvMode)
		{
			NewPasvConnect(dataReceive);
			//a connection for this call, it is a tcp client
				//std::bind(&FTPClient::OnRecvDataChannelPasvMode, /*pasvConn.back(),*/this,
				//std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		}
		else {
			dataServer.RecvHandler = dataReceive;
		}



		string serverFile = WString2String(wServerFile);
		curFtpCmd = FTPSocket::Retr;
		curCmd = "RETR";
		downFileName = WString2String(wDstFile);
		ofs.open(downFileName.data(), std::ios::out | std::ios::binary);
		memset(sendBuff, 0, SendSize);
		sprintf(sendBuff, "RETR %s\r\n", serverFile.data());

		//FSCommand retrCmd;
		//上传文件采用异步，所以这里就不等待了
		//Enque("Retr", &retrCmd);

		cmdClient.Send(sendBuff, strlen(sendBuff));
		downloadFileTask[taskID] = TransFileInfo();
		downloadFileTask[taskID].fileSize = fileSize;
		downloadFileTask[taskID].fileName = downFileName;
		//retrCmd.WaitResult();

		//memset(recvBuff, 0, RecvSize);
		////cmdClient.Receive(recvBuff, RecvSize);

		//memset(sendBuff, 0, SendSize);
		////sprintf(sendBuff, "TYPE BINARY\r\n");
		//sprintf(sendBuff, "TYPE I\r\n");
		////cmdClient.Send(sendBuff, SendSize); //这个命令刚发出 datachannel收到了文件内容
		//memset(recvBuff, 0, RecvSize);
		/*cmdClient.Receive(recvBuff, RecvSize);*/
		//memset(recvBuff, 0, RecvSize);
		//int nRetSize = recv(dataServer.s[0], recvBuff, RecvSize, 0);
		//dataServer.Receive(recvBuff, RecvSize);
		//cout << "data server:" << recvBuff << endl;
		//cout << "RETR Recv:" << recvBuff << endl;

	}

	void FTPClient::Stor(std::wstring wLocalFile, std::wstring wServerFileName, int fileSize, IFileTransferObserver* observer /*= nullptr*/)
	{
		int taskID = -1;
		if (serverMode == PasvMode)
		{
			taskID = pasvConn.size();
			//a connection for this call, it is a tcp client
				//std::bind(&FTPClient::OnRecvDataChannelPasvMode, /*pasvConn.back(),*/this,
				//std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		}
		else {
			taskID = dataServer.clientList.size();
		}


		std::function<void(string, SOCKET, char*, int)> dataReceive = [&](string mode, SOCKET s, char* buff, int count) {
			if (mode == "disconnect")
			{
				observer->UploadFinishCallBack(taskID, String2WString(downloadFileTask[taskID].fileName).data());
				ofs.close();
				return;
			}
			if (!ofs.is_open())
				cout << "ifs not open" << endl;
			if (count > 0)
			{
				ofs.write(buff, count);

				if (downloadFileTask.find(taskID) != downloadFileTask.end())
				{
					downloadFileTask[taskID].curTransFileSize += count;
					float percent = (float)downloadFileTask[taskID].curTransFileSize / (float)downloadFileTask[taskID].fileSize;
					if (observer != nullptr)
					{
						observer->UploadFileProgressCallBack(taskID, percent, String2WString(downloadFileTask[taskID].fileName).data());
					}
				}
			}
			ofs.flush();
		};
		if (serverMode == PasvMode)
		{
			NewPasvConnect(dataReceive);
			//a connection for this call, it is a tcp client
				//std::bind(&FTPClient::OnRecvDataChannelPasvMode, /*pasvConn.back(),*/this,
				//std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		}
		else {
			dataServer.RecvHandler = dataReceive;
		}
		std::string localFile = WString2String(wLocalFile);
		std::string serverFileName = WString2String(wServerFileName);


		curFtpCmd = FTPSocket::Stor;
		//需要服务器处于被动模式
		FSCommand cell[2];
		Enque("Stor", cell);

		//上传文件 需要服务器处于被动模式
		memset(sendBuff, 0, SendSize);
		sprintf(sendBuff, "STOR %s\r\n", localFile.data());
		//sprintf(sendBuff, "GET  %s\r\n", "1.txt");
		cmdClient.Send(sendBuff, strlen(sendBuff));
		cell[0].WaitResult();
		//memset(recvBuff, 0, RecvSize);
		//cmdClient.Receive(recvBuff, RecvSize);
		//cout << "Stor Recv:" << recvBuff << endl;
		Enque("Type", cell + 1);
		//需要自己向服务器上传文件
		memset(sendBuff, 0, SendSize);
		//sprintf(sendBuff, "TYPE BINARY\r\n");	//这一行报错
		sprintf(sendBuff, "TYPE I\r\n");
		cmdClient.Send(sendBuff, SendSize);
		cell[1].WaitResult();
		//memset(recvBuff, 0, RecvSize);
		//cmdClient.Receive(recvBuff, RecvSize);
		//cout << "Stor Recv:" << recvBuff << endl;
		ifstream ifs;
		ifs.open(localFile.data(), std::ios::in | std::ios::binary);
		if (!ifs.is_open())
		{
			cout << "ifs not open" << endl;
			return;
		}
		std::thread th([&]() {
			if (ifs.gcount() > 0)
			{
				char buff[1024];
				float fileSize = (float)ifs.gcount();
				float uploadFileSize = 0.f;
				while (!ifs.eof())
				{
					float readSize = ifs.gcount() < 1024 ? ifs.gcount() : 1024;
					uploadFileSize += readSize;
					memset(buff, 0, 1024);
					ifs.read(buff, 1024);
					dataServer.Send(buff, readSize);
					observer->UploadFileProgressCallBack(taskID, uploadFileSize / fileSize, wLocalFile.data());
				}
				ifs.close();
				observer->UploadFinishCallBack(taskID, wLocalFile.data());
			}
			else {
				ifs.close();
			}
			});
		th.detach();

	}

	void FTPClient::OnRecvDataChannelPortMode(string mode, SOCKET s, char* buff, int count)
	{
		if (mode != "msg") return;
		if (curFtpCmd == FTPSocket::FtpCmd::List)
		{
			if (serverMode == PortMode)
			{
				//预计有下一次接收数据
				listCmd.contents.push_back(buff);
				//listCmd.Notify();
				listCmd.recvTimes++;
				printf("recv List Cmd[%d]:%s\r\n", count, "");
			}
		}
		//cout << fileLists;

		if (curFtpCmd == FTPSocket::Retr)
		{
			if (serverMode == PortMode)
			{
				cout << buff << endl;
				if (!ofs.is_open())
					cout << "ifs not open" << endl;
				if (count > 0)
					ofs.write(buff, count);
				ofs.flush();
			}
		}
		if (s == INVALID_SOCKET)
		{
			if (serverMode == PortMode)
			{
				ofs.close();
			}
		}
	}

	void FTPClient::OnRecvDataChannelPasvMode(string mode, SOCKET s, char* buff, int count)
	{
		if (mode != "msg") return;
		if (curFtpCmd == FTPSocket::FtpCmd::List)
		{
			if (serverMode == PasvMode)
			{

				//预计有下一次接收数据
				/*if (count < 1460)
				{

				}
				else {
					listCmd.content += buff;
				}*/
				listCmd.contents.push_back(buff);
				//listCmd.Notify();
				listCmd.recvTimes++;
				printf("recv List Cmd[%d]:%s\r\n", count, "");
			}
		}
		if (curFtpCmd == FTPSocket::Retr)
		{
			if (serverMode == PasvMode)
			{
				//cout << buff << endl;
				if (!ofs.is_open())
					cout << "ifs not open" << endl;
				if (count > 0)
					ofs.write(buff, count);
				ofs.flush();
			}
		}
		if (s == INVALID_SOCKET)
		{
			if (serverMode == PasvMode)
			{
				ofs.close();
			}
		}
		//cout << fileList
	}

	void FTPClient::SendData(SOCKET s)
	{
		if (send(s, sendBuff, SendSize, 0) < 0)
		{
			std::cout << "Failed to send data!" << std::endl;
		}
	}

	void FTPClient::ParseListAck(string msg)
	{
		vector<map<int, string>> fileList;
		stringstream  ss(msg);
		char h;
		ss.read(&h, 1);

		string line;
		string word;
		while (!ss.eof())
		{
			getline(ss, line);

			map<int, string> f;
			stringstream  ss(line);
			int off = 0;
			while (!ss.eof())
			{
				word.clear();
				ss >> word;
				if (!word.empty())
					f[off++] = word;
			}
			if (line[0] == 'd')
				f[LC_FileType] = "Directory";
			else
				f[LC_FileType] = "File";
			ss >> line;
			//cout << line << endl;
		}
	}

	void FTPClient::GetFileListFromContent(const string& content, vector<FileInfo>& vctFileList)
	{
		cout << content << endl;
		vector<map<int, string>> fileList;
		stringstream  ss(content);

		string line;
		string word;
		while (!ss.eof())
		{
			getline(ss, line);
			if (line.empty())continue;
			ListFileInfo fileInfo(line);
			FileInfo fi;
			chars2wchars(fileInfo.fileName, fi.fileName, 255);
			fi.fileSize = fileInfo.filesize;
			fi.isFile = fileInfo.isFile;
			chars2wchars(fileInfo.author, fi.author, 255);
			chars2wchars(fileInfo.authority, fi.authority, 10);
			vctFileList.push_back(fi);
		}
	}


	void FTPClient::NewPasvConnect(std::function<void(string, SOCKET, char*, int)> recvFunc)
	{
		TcpClient client;
		pasvConn.push_back(client);
		pasvConn.back().RecvHandler = recvFunc;
		pasvConn.back().Init(dataIp, dataPort);
		pasvConn.back().StartReceiveThread();
	}

	void FTPClient::NewPortConnect(std::function<void(string, SOCKET, char*, int)> recvFunc)
	{

	}

	void FTPClient::RemovePasvConnect()
	{
		if (pasvConn.back().GetIsRunning())
			pasvConn.back().Close();
		pasvConn.pop_back();
	}

	bool FTPClient::ParseRecvInfo(std::string content, std::string& recvContent)
		//std::string FTPClient::ParseRecvInfo(std::string content)
	{
		size_t firstspacepos = content.find_first_of(' ');
		std::string strMsgCode = content.substr(0, firstspacepos);
		if (strMsgCode.empty()) return false;
		int msgcode = atoi(strMsgCode.data());
		cout << "[msgcode]" << msgcode << endl;
		recvContent = content.substr(firstspacepos, content.size());
		/*if (msgcode == 200 || msgcode == 220 || msgcode == 230 || msgcode == 212 || msgcode == 213)
		{

			return true;
		}*/
		if (msgcode == 150)
			return false;
		return true;
	}
	void FTPClient::Enque(std::string type, FSCommand* cell)
	{
		cell->callType = type;
		cell->getResult = false;
		//cell->content = "";
		callDeque.push_back(cell);
	}
	void FTPClient::OnCmdClientRecv(string mode, SOCKET s, char* buff, int size)
	{
		if (mode != "msg") return;
		stringstream ss(buff);
		char line[10480];
		while (!ss.eof())
		{
			string linestr;
			ss.getline(line, 10480);
			linestr = line;
			if (!linestr.empty())
			{
				if (callDeque.size() > 0)
				{
					/*callDeque.front()->content = recv;
					callDeque.front()->recvMsg = recv;*/
					//需要接收两次
					callDeque.front()->RecvResult(linestr);
					if (callDeque.front()->IsFinished())
					{
						callDeque.pop_front();
					}
				}
			}
		}

		//	cout << "[recv]----------------" << endl;
	}

}