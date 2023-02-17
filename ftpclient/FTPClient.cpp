#include "BaseInc.h"
#include "FTPClient.h"
#include "tool.h"
void FTPClient::Login(string addr, string usr, string pwd)
{
	//连接
	cmdClient.Init(addr, 21);

	memset(recvBuff, 0, RecvSize);
	cmdClient.Receive(recvBuff, RecvSize);

	//登录用户
	memset(sendBuff, 0, SendSize);
	sprintf(sendBuff, "USER %s\r\n", usr.data());
	cmdClient.Send(sendBuff, strlen(sendBuff));
	memset(recvBuff, 0, RecvSize);
	cmdClient.Receive(recvBuff, RecvSize);
	//密码
	memset(sendBuff, 0, SendSize);
	sprintf(sendBuff, "PASS %s\r\n", pwd.data());
	cmdClient.Send(sendBuff, strlen(sendBuff));
	memset(recvBuff, 0, RecvSize);
	cmdClient.Receive(recvBuff, RecvSize);
}

void FTPClient::Pasv()
{
	if (dataServer.GetIsRunning())
	{
		dataClient.Close();
	}
	//开启被动模式后，需要
	memset(sendBuff, 0, SendSize);
	sprintf(sendBuff, "PASV\r\n");
	cmdClient.Send(sendBuff, strlen(sendBuff));
	memset(recvBuff, 0, RecvSize);
	cmdClient.Receive(recvBuff, RecvSize);


	string str(recvBuff);
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
	dataClient.Init(ip, port);
	dataClient.StartReceiveThread();
}

std::string FTPClient::List()
{
	memset(sendBuff, 0, SendSize);
	sprintf(sendBuff, "LIST\r\n");
	cmdClient.Send(sendBuff, strlen(sendBuff));
	memset(recvBuff, 0, RecvSize);
	cmdClient.Receive(recvBuff, RecvSize);
	::Sleep(500);
	ParseListAck(dataServer.recvBuff, RecvSize);
	std::string resStr = dataServer.recvBuff;
	return resStr;
}

void FTPClient::Port()
{
	if (dataClient.GetIsRunning())
	{
		dataClient.Close();
	}
	int port = 22020;
	dataServer.Init(22020);
	dataServer.StartReceiveThread(false);
	//dataServer.StartAcceptThread(false);
	memset(sendBuff, 0, SendSize);
	sprintf(sendBuff, "%s\r\n", GetPortCmd(port).data());
	cmdClient.Send(sendBuff, strlen(sendBuff));
	memset(recvBuff, 0, RecvSize);
	cmdClient.Receive(recvBuff, RecvSize);
}

void FTPClient::Exit()
{
	memset(sendBuff, 0, SendSize);
	sprintf(sendBuff, "QUIT\r\n");
	cmdClient.Send(sendBuff, strlen(sendBuff));
	memset(recvBuff, 0, RecvSize);
	cmdClient.Receive(recvBuff, RecvSize);
	if (cmdClient.GetIsRunning()) cmdClient.Close();
	if (dataClient.GetIsRunning()) dataClient.Close();
	if (dataServer.GetIsRunning()) dataServer.Close();

}

std::string FTPClient::Pwd()
{
	memset(sendBuff, 0, SendSize);
	sprintf(sendBuff, "PWD\r\n");
	cmdClient.Send(sendBuff, strlen(sendBuff));
	memset(recvBuff, 0, RecvSize);
	cmdClient.Receive(recvBuff, RecvSize);
	//std::string recvStr = cmdClient.Receive();
	//257 "/" is current directory.
	std::string resultDir = recvBuff;
	int nPos = resultDir.find('"');
	int nLastPos = resultDir.rfind('"');
	std::string dir = SubString(resultDir,'"','"');
	return	dir;
}
void FTPClient::Cwd(std::string workDir)
{
	memset(sendBuff, 0, SendSize);
	sprintf(sendBuff, "CWD %s\r\n", workDir.data());
	cmdClient.Send(sendBuff, strlen(sendBuff));
	memset(recvBuff, 0, RecvSize);
	cmdClient.Receive(recvBuff, RecvSize);
}


void FTPClient::MakeDiectory()
{
	memset(sendBuff, 0, SendSize);
	sprintf(sendBuff, "MKD abc\r\n");
	cmdClient.Send(sendBuff, strlen(sendBuff));
	memset(recvBuff, 0, RecvSize);
	cmdClient.Receive(recvBuff, RecvSize);
}

void FTPClient::Retr(string serverFile, string dstFile)
{
	curCmd = "RETR";
	downFileName = dstFile;
	ofs.open(downFileName.data(), std::ios::out | std::ios::binary);
	memset(sendBuff, 0, SendSize);
	sprintf(sendBuff, "RETR %s\r\n", serverFile.data());
	cmdClient.Send(sendBuff, strlen(sendBuff));
	memset(recvBuff, 0, RecvSize);
	cmdClient.Receive(recvBuff, RecvSize);

	memset(sendBuff, 0, SendSize);
	//sprintf(sendBuff, "TYPE BINARY\r\n");
	sprintf(sendBuff, "TYPE I\r\n");
	cmdClient.Send(sendBuff, SendSize); //这个命令刚发出 datachannel收到了文件内容
	memset(recvBuff, 0, RecvSize);
	cmdClient.Receive(recvBuff, RecvSize);
	//memset(recvBuff, 0, RecvSize);
	//int nRetSize = recv(dataServer.s[0], recvBuff, RecvSize, 0);
	//dataServer.Receive(recvBuff, RecvSize);
	//cout << "data server:" << recvBuff << endl;
	//cout << "RETR Recv:" << recvBuff << endl;

}

void FTPClient::Stor(string fileName)
{
	//需要服务器处于被动模式


	//上传文件 需要服务器处于被动模式
	memset(sendBuff, 0, SendSize);
	sprintf(sendBuff, "STOR %s\r\n", "1.txt");
	//sprintf(sendBuff, "GET  %s\r\n", "1.txt");
	cmdClient.Send(sendBuff, strlen(sendBuff));

	memset(recvBuff, 0, RecvSize);
	cmdClient.Receive(recvBuff, RecvSize);
	//cout << "Stor Recv:" << recvBuff << endl;

	//需要自己向服务器上传文件
	memset(sendBuff, 0, SendSize);
	//sprintf(sendBuff, "TYPE BINARY\r\n");	//这一行报错
	sprintf(sendBuff, "TYPE I\r\n");
	cmdClient.Send(sendBuff, SendSize);
	memset(recvBuff, 0, RecvSize);
	cmdClient.Receive(recvBuff, RecvSize);
	//cout << "Stor Recv:" << recvBuff << endl;
	ifstream ifs;
	ifs.open(fileName.data(), std::ios::in | std::ios::binary);
	/*if (!ifs.is_open())
		cout << "ifs not open" << endl;*/
	char buff[1024];
	while (!ifs.eof())
	{
		int readSize = ifs.gcount() < 1024 ? ifs.gcount() : 1024;
		memset(buff, 0, 1024);
		ifs.read(buff, 1024);
		dataServer.Send(buff, readSize);
	}
	ifs.close();
}

void FTPClient::OnRecvDataChannel(SOCKET s, char* buff, int count)
{
	if (curCmd == "RETR")
	{
		cout << buff << endl;
		if (!ofs.is_open())
			cout << "ifs not open" << endl;
		if (count > 0)
			ofs.write(buff, count);
		ofs.flush();
	}
	if (s == INVALID_SOCKET)
	{
		ofs.close();
	}
}

void FTPClient::SendData(SOCKET s)
{
	if (send(s, sendBuff, SendSize, 0) < 0)
	{
		std::cout << "Failed to send data!" << std::endl;
	}
}

void FTPClient::ParseListAck(char* buff, int size)
{
	vector<map<int, string>> fileList;
	stringstream  ss(dataServer.recvBuff);
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
		cout << line << endl;
	}
}
bool FTPClient::ParseRecvInfo(std::string content, std::string& recvContent)
//std::string FTPClient::ParseRecvInfo(std::string content)
{
	size_t firstspacepos =content.find_first_of(' ');
	int msgcode = atoi(content.substr(0, firstspacepos).data());
	if (msgcode == 200 || msgcode == 212 ||msgcode ==213)
	{
		recvContent = content.substr(firstspacepos, content.size());
		return true;
	}
	return false;
}