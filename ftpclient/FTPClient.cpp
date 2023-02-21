#include "BaseInc.h"
#include "FTPClient.h"
#include "tool.h"
namespace FTPSocket {
	void FTPClient::Login(const wchar_t* wAddr, const wchar_t* wUsr, const wchar_t* wPwd)
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

	bool FTPClient::Pasv()
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
		if (cell.recvMsgs.back().code == 500)
			return false;
		serverMode = PasvMode;
		string str(cell.GetContent());
		if (str.size() == 0) return false;
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
		cout << "target server:" << ip << "target port:" << port << endl;
		return true;
	}

	bool FTPClient::Type(int mode)
	{
		//设置服务器模式
		FSCommand cell;
		Enque("Type", &cell);
		memset(sendBuff, 0, SendSize);
		//sprintf(sendBuff, "TYPE BINARY\r\n");	//这一行报错
		if (mode == 0)
		{
			sprintf(sendBuff, "TYPE I\r\n");
		}
		else {
			sprintf(sendBuff, "TYPE A\r\n");
		}
		cmdClient.Send(sendBuff, SendSize);
		cell.WaitResult();
		//memset(recvBuff, 0, RecvSize);
		//cmdClient.Receive(recvBuff, RecvSize);
		//cout << "Stor Recv:" << recvBuff << endl;
		if (cell.recvMsgs.back().code == 200)
			return true;
		return	false;
	}

	const wchar_t* FTPClient::List()
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
			RemovePasvConnect(GetCmdId());
		}
		else {
			dataServer.ClientOffline(dataServer.clientList.back());
		}
		return String2WString(resStr).data();
	}

	void FTPClient::ListAllFileAndFolders(std::vector<FileInfo>& files)
	{
		Pasv();
		::Sleep(100);
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
			RemovePasvConnect(GetCmdId());
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
		if (cmdClient.GetIsRunning())
		{
			cmdClient.Shutdown();
			cmdClient.Close();
		}
		//if (dataClient.GetIsRunning()) dataClient.Close();
		if (dataServer.GetIsRunning()) dataServer.Close();

		for (int i = 0; i < pasvConn.size(); i++)
		{
			if (pasvConn[i].GetIsRunning())
				pasvConn[i].Close();
		}

	}

	const wchar_t* FTPClient::Pwd()
	{
		curFtpCmd = FTPSocket::Pwd;
		FSCommand cell;
		Enque("Pwd", &cell);

		memset(sendBuff, 0, SendSize);
		sprintf(sendBuff, "PWD\r\n");
		cmdClient.Send(sendBuff, strlen(sendBuff));
		cell.WaitResult();
		//memset(recvBuff, 0, RecvSize);
		//cmdClient.Receive(recvBuff, RecvSize);
		//std::string recvStr = cmdClient.Receive();
		//257 "/" is current directory.
		std::string resultDir = cell.contents.back();
		int nPos = resultDir.find('"');
		int nLastPos = resultDir.rfind('"');
		std::string dir = SubString(resultDir, '"', '"');
		static std::wstring tempResultForPwd;
		tempResultForPwd = String2WString(dir);

		return tempResultForPwd.data();
	}
	bool FTPClient::Cwd(const wchar_t* workDir)
	{
		std::string sWorkDir = WString2String(workDir);
		curFtpCmd = FTPSocket::Cwd;
		FSCommand cell;
		Enque("Cwd", &cell);

		memset(sendBuff, 0, SendSize);
		sprintf(sendBuff, "CWD %s\r\n", sWorkDir.data());
		cmdClient.Send(sendBuff, strlen(sendBuff));
		//memset(recvBuff, 0, RecvSize);
		//cmdClient.Receive(recvBuff, RecvSize);
		cell.WaitResult();
		if (cell.recvMsgs.size() > 0 && cell.recvMsgs.back().code == 250)
		{
			return true;
		}

		return true;

	}


	bool FTPClient::Cdup()
	{
		curFtpCmd = FTPSocket::Cdup;
		FSCommand cell;
		Enque("Cdup", &cell);
		memset(sendBuff, 0, SendSize);
		sprintf(sendBuff, "CDUP\r\n");
		cmdClient.Send(sendBuff, strlen(sendBuff));
		cell.WaitResult();
		if (cell.recvMsgs.back().code == 501)
		{
			return false;
		}

		return true;

	}

	void FTPClient::MakeDiectory(const wchar_t* wDir)
	{
		curFtpCmd = FTPSocket::MakeDir;
		std::string dir = WString2String(wDir);
		FSCommand cell;
		Enque("MakeDir", &cell);
		curFtpCmd = FTPSocket::MakeDir;
		memset(sendBuff, 0, SendSize);
		sprintf(sendBuff, "MKD %s\r\n", dir.data());
		cmdClient.Send(sendBuff, strlen(sendBuff));
		//memset(recvBuff, 0, RecvSize);
		cell.WaitResult();
		//cmdClient.Receive(recvBuff, RecvSize);
	}

	void FTPClient::Retr(const wchar_t* wServerFile, const wchar_t* wDstFile, int fileSize, IFileTransferObserver* observer)
	{
		Pasv();
		::Sleep(100);
		int taskID = GetCmdId();
		std::function<void(int, string, SOCKET, char*, int, IFileTransferObserver*)> dataReceive = [this](int taskID, string mode, SOCKET s, char* buff, int count, IFileTransferObserver* observ) {
			auto findIter = downloadFileTask.find(taskID);
			if (findIter != downloadFileTask.end())
			{
				TransFileInfo& tfi = findIter->second;
				if (mode == "disconnect")
				{
					if (tfi.ofs.is_open())
					{
						std::wstring wstr = String2WString(tfi.fileName);
						observ->DownloadFinishCallBack(taskID, wstr.data());
						tfi.ofs.close();
					}
					downloadFileTask.erase(findIter);
					return;
				}
				if (!tfi.ofs.is_open())
					cout << "ofs not open" << endl;
				if (count > 0)
				{
					tfi.ofs.write(buff, count);

					if (downloadFileTask.find(taskID) != downloadFileTask.end())
					{
						tfi.curTransFileSize += count;
						float percent = (float)tfi.curTransFileSize / (float)tfi.fileSize;
						if (observ != nullptr)
						{
							std::wstring wstrfileName = String2WString(tfi.fileName);
							observ->DownloadFileProgressCallBack(s, percent, wstrfileName.data());
						}
						else {
							cout << "obsever is null" << endl;
						}
					}
				}
				tfi.ofs.flush();
			}
		};
		if (serverMode == PasvMode)
		{
			NewPasvConnect(bind(dataReceive, taskID, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, observer));
			//a connection for this call, it is a tcp client
				//std::bind(&FTPClient::OnRecvDataChannelPasvMode, /*pasvConn.back(),*/this,
				//std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		}
		else {
			dataServer.RecvHandler = bind(dataReceive, taskID, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, observer);
		}

		string serverFile = WString2String(wServerFile);
		curFtpCmd = FTPSocket::Retr;

		downFileName = WString2String(wDstFile);

		//FSCommand retrCmd;
		//上传文件采用异步，所以这里就不等待了
		//Enque("Retr", &retrCmd);

		downloadFileTask[taskID] = TransFileInfo();
		downloadFileTask[taskID].ofs.open(downFileName.data(), std::ios::out | std::ios::binary);
		downloadFileTask[taskID].fileSize = fileSize;
		downloadFileTask[taskID].fileName = downFileName;
		//retrCmd.WaitResult();

		curCmd = "RETR";
		memset(sendBuff, 0, SendSize);
		sprintf(sendBuff, "RETR %s\r\n", serverFile.data());
		cmdClient.Send(sendBuff, strlen(sendBuff));
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

	void FTPClient::Stor(const wchar_t* wLocalFile, const wchar_t* wServerFileName, IFileTransferObserver* observer /*= nullptr*/)
	{
		Pasv();
		::Sleep(100);
		std::wstring wlf = wLocalFile;
		int taskID = GetCmdId();
		curFtpCmd = FTPSocket::Stor;


		std::function<void(int, string, SOCKET, char*, int, IFileTransferObserver*)> dataReceive = [this](int taskID, string mode, SOCKET s, char* buff, int count, IFileTransferObserver* observ) {
			//if (mode == "disconnect")
			//{
			////	observ->UploadFinishCallBack(taskID, String2WString(downloadFileTask[taskID].fileName).data());
			//	ofs.close();
			//	return;
			//}

		};
		//设置服务器模式
		//Type(1);
		while(!Pasv());
	
		if (serverMode == PasvMode)
		{
			NewPasvConnect(std::bind(dataReceive, taskID, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, observer));
			//a connection for this call, it is a tcp client
				//std::bind(&FTPClient::OnRecvDataChannelPasvMode, /*pasvConn.back(),*/this,
				//std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		}
		else {
			dataServer.RecvHandler = std::bind(dataReceive, taskID, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, observer);
		}
		std::string localFile = WString2String(wlf);
		std::string serverFileName = WString2String(wServerFileName);

		FSCommand cell;
		Enque("Stor", &cell);
		//需要自己向服务器上传文件


		//上传文件 需要服务器处于被动模式
		memset(sendBuff, 0, SendSize);
		sprintf(sendBuff, "STOR %s\r\n", localFile.data());
		//sprintf(sendBuff, "GET  %s\r\n", "1.txt");
		cmdClient.Send(sendBuff, strlen(sendBuff));
		cell.WaitResult();
		//memset(recvBuff, 0, RecvSize);
		//cmdClient.Receive(recvBuff, RecvSize);
		//cout << "Stor Recv:" << recvBuff << endl;

		uploadFileTask[taskID] = TransFileInfo();
		//uploadFileTask[taskID].fileSize = fileSize;
		uploadFileTask[taskID].fileName = serverFileName;
		uploadFileTask[taskID].curTransFileSize = 0;
		uploadFileTask[taskID].ifs.open(localFile.data(), std::ios::in | std::ios::binary);
		if (!uploadFileTask[taskID].ifs.is_open())
		{
			cout << "ifs not open" << endl;
			return;
		}


		auto UploadFileThread = [this](int taskId, std::wstring localFile, IFileTransferObserver* observ) {
			auto findIter = uploadFileTask.find(taskId);
			if (findIter != uploadFileTask.end())
			{
				TransFileInfo& tfi = findIter->second;
				if (!tfi.ifs.is_open()) {
					return;
				}
				tfi.ifs.seekg(0, std::ios::end);
				float fileSize = tfi.fileSize = tfi.ifs.tellg();
				if (fileSize > 0)
				{
					tfi.ifs.seekg(0, std::ios::beg);
					char buff[1024];
					tfi.curTransFileSize = 0;
					while (!tfi.ifs.eof())
					{
						memset(buff, 0, 1024);
						tfi.ifs.read(buff, 1024);
						float readSize = tfi.ifs.gcount() < 1024 ? tfi.ifs.gcount() : 1024;
						tfi.curTransFileSize += readSize;
						if (serverMode == PasvMode)
							pasvConn[taskId].Send(buff, readSize);
						else
							dataServer.Send(buff, readSize);
						observ->UploadFileProgressCallBack(taskId, tfi.curTransFileSize / fileSize, localFile.data());
					}
					tfi.ifs.close();
					observ->UploadFinishCallBack(taskId, localFile.data());
					uploadFileTask.erase(findIter);
				}
				else {
					tfi.ifs.close();
				}
				if (serverMode == PasvMode)
				{
					RemovePasvConnect(taskId);
					/*Cwd(L"/");
					Pwd();
					Type(1);*/
				}
				else {
					dataServer.ClientOffline(dataServer.clientList.back());
				}
			}
		};

		std::thread th(std::bind(UploadFileThread, taskID, wlf, observer));
		th.detach();

	}

	bool FTPClient::DeleteFile(const wchar_t* serverFile)
	{
		std::string sWorkDir = WString2String(serverFile);
		curFtpCmd = FTPSocket::Cwd;
		FSCommand cell;
		Enque("DELE", &cell);

		memset(sendBuff, 0, SendSize);
		sprintf(sendBuff, "DELE %s\r\n", sWorkDir.data());
		cmdClient.Send(sendBuff, strlen(sendBuff));
		//memset(recvBuff, 0, RecvSize);
		//cmdClient.Receive(recvBuff, RecvSize);
		cell.WaitResult();
		if (cell.recvMsgs.size() > 0 && cell.recvMsgs.back().code == 250)
		{
			return true;
		}
		return false;
	}

	bool FTPClient::RenameFile(const wchar_t* srcFile, const wchar_t* destFile)
	{
		std::string sSrcFile = WString2String(srcFile);
		std::string sDstFile = WString2String(destFile);
		curFtpCmd = FTPSocket::Cwd;
		FSCommand cell[2];
		Enque("Cwd", cell);

		memset(sendBuff, 0, SendSize);
		sprintf(sendBuff, "RNFR %s\r\n", sSrcFile.data());
		cmdClient.Send(sendBuff, strlen(sendBuff));
		//memset(recvBuff, 0, RecvSize);
		//cmdClient.Receive(recvBuff, RecvSize);
		cell[0].WaitResult();
		if (cell[0].recvMsgs.size() > 0 && cell[0].recvMsgs.back().code == 350)
		{
			Enque("Cwd", cell+1);
			memset(sendBuff, 0, SendSize);
			sprintf(sendBuff, "RNTO %s\r\n", sDstFile.data());
			cmdClient.Send(sendBuff, strlen(sendBuff));
			cell[1].WaitResult();
			if (cell[1].recvMsgs.size() > 0 && cell[1].recvMsgs.back().code == 250)
			{
				return true;
			}
			return false;
		}
		return false;
	}

	bool FTPClient::DeleteDirectory(const wchar_t* serverFile)
	{
		std::string sWorkDir = WString2String(serverFile);
		curFtpCmd = FTPSocket::Cwd;
		FSCommand cell;
		Enque("DELE", &cell);

		memset(sendBuff, 0, SendSize);
		sprintf(sendBuff, "RMD %s\r\n", sWorkDir.data());
		cmdClient.Send(sendBuff, strlen(sendBuff));
		//memset(recvBuff, 0, RecvSize);
		//cmdClient.Receive(recvBuff, RecvSize);
		cell.WaitResult();
		if (cell.recvMsgs.size() > 0 && cell.recvMsgs.back().code == 250)
		{
			return true;
		}
		return false;
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
		vector<FileInfo> vFileList;
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
			vFileList.push_back(fi);
		}
		vctFileList.assign(vFileList.begin(), vFileList.end());
	}


	void FTPClient::NewPasvConnect(std::function<void(string, SOCKET, char*, int)> recvFunc)
	{
		//the task id is for datachannel
		int taskId = NewCmdId();
		TcpClient client;
		if (pasvConn.find(taskId) == pasvConn.end())
		{
			pasvConn[taskId] = client;
			pasvConn[taskId].RecvHandler = recvFunc;
			pasvConn[taskId].Init(dataIp, dataPort);
			pasvConn[taskId].StartReceiveThread();
		}
	}

	void FTPClient::NewPortConnect(std::function<void(string, SOCKET, char*, int)> recvFunc)
	{

	}

	void FTPClient::RemovePasvConnect(int taskID)
	{
		if (pasvConn.size() > 0)
		{
			auto findIter = pasvConn.find(taskID);
			if (findIter != pasvConn.end())
			{
				if (pasvConn[taskID].GetIsRunning())
				{
					pasvConn[taskID].Shutdown();
					pasvConn[taskID].Close();
					//这里清除不能再另外一个线程执行
					//pasvConn.erase(findIter);
				}
			}
		}
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
		cout << buff << endl;
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