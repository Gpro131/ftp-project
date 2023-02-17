#include "BaseInc.h"
#include "FTPClient.h"
#include "tool.h"
void FTPClient::Login(string addr, string usr, string pwd)
{
	//����
	cmdClient.Init(addr, 21);
	cmdClient.StartReceiveThread();

	//memset(recvBuff, 0, RecvSize);
	//cmdClient.Receive(recvBuff, RecvSize);

	APICell cell[2];
	Enque("loginuser", cell);
	//��¼�û�
	memset(sendBuff, 0, SendSize);
	sprintf(sendBuff, "USER %s\r\n", usr.data());
	cmdClient.Send(sendBuff, strlen(sendBuff));
	//memset(recvBuff, 0, RecvSize);
	//cmdClient.Receive(recvBuff, RecvSize);
	cell[0].WaitResult();


	//����
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
	if (dataServer.GetIsRunning())
	{
		dataServer.Close();
	}
	APICell cell;
	Enque("Pasv", &cell);

	//��������ģʽ����Ҫ
	memset(sendBuff, 0, SendSize);
	sprintf(sendBuff, "PASV\r\n");
	cmdClient.Send(sendBuff, strlen(sendBuff));
	//memset(recvBuff, 0, RecvSize);
	//cmdClient.Receive(recvBuff, RecvSize);
	cell.WaitResult();

	string str(recvBuff);
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
	//���ӳɹ�
	dataClient.Init(ip, port);
	dataClient.StartReceiveThread();
}

std::string FTPClient::List()
{
	APICell cell;
	Enque("List", &cell);
	memset(sendBuff, 0, SendSize);
	sprintf(sendBuff, "LIST\r\n");
	cmdClient.Send(sendBuff, strlen(sendBuff));
	//memset(recvBuff, 0, RecvSize);
	//cmdClient.Receive(recvBuff, RecvSize);
	cell.WaitResult();
	cout <<"ListCellContent" << cell.content << endl;
	ParseListAck(dataServer.recvBuff, RecvSize);
	std::string resStr = dataServer.recvBuff;
	cout << resStr << endl;
	return resStr;
}

void FTPClient::Port()
{
	if (dataClient.GetIsRunning())
	{
		dataClient.Close();
	}
	APICell cell;
	Enque("Port", &cell);
	int port = 22020;
	dataServer.Init(22020);
	dataServer.StartReceiveThread(false);
	//dataServer.StartAcceptThread(false);
	memset(sendBuff, 0, SendSize);
	sprintf(sendBuff, "%s", GetPortCmd(port).data());
	cmdClient.Send(sendBuff, strlen(sendBuff));
	cell.WaitResult();
	/*memset(recvBuff, 0, RecvSize);
	cmdClient.Receive(recvBuff, RecvSize);*/
}

void FTPClient::Exit()
{
	memset(sendBuff, 0, SendSize);
	sprintf(sendBuff, "QUIT\r\n");
	cmdClient.Send(sendBuff, strlen(sendBuff));
	//memset(recvBuff, 0, RecvSize);
	//cmdClient.Receive(recvBuff, RecvSize);
	if (cmdClient.GetIsRunning()) cmdClient.Close();
	if (dataClient.GetIsRunning()) dataClient.Close();
	if (dataServer.GetIsRunning()) dataServer.Close();

}

std::string FTPClient::Pwd()
{
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
	return	dir;
}
void FTPClient::Cwd(std::string workDir)
{
	memset(sendBuff, 0, SendSize);
	sprintf(sendBuff, "CWD %s\r\n", workDir.data());
	cmdClient.Send(sendBuff, strlen(sendBuff));
	//memset(recvBuff, 0, RecvSize);
	//cmdClient.Receive(recvBuff, RecvSize);
}


void FTPClient::MakeDiectory()
{
	memset(sendBuff, 0, SendSize);
	sprintf(sendBuff, "MKD abc\r\n");
	cmdClient.Send(sendBuff, strlen(sendBuff));
	memset(recvBuff, 0, RecvSize);
	//cmdClient.Receive(recvBuff, RecvSize);
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
	//cmdClient.Receive(recvBuff, RecvSize);

	memset(sendBuff, 0, SendSize);
	//sprintf(sendBuff, "TYPE BINARY\r\n");
	sprintf(sendBuff, "TYPE I\r\n");
	//cmdClient.Send(sendBuff, SendSize); //�������շ��� datachannel�յ����ļ�����
	memset(recvBuff, 0, RecvSize);
	/*cmdClient.Receive(recvBuff, RecvSize);*/
	//memset(recvBuff, 0, RecvSize);
	//int nRetSize = recv(dataServer.s[0], recvBuff, RecvSize, 0);
	//dataServer.Receive(recvBuff, RecvSize);
	//cout << "data server:" << recvBuff << endl;
	//cout << "RETR Recv:" << recvBuff << endl;

}

void FTPClient::Stor(string fileName,string serverFileName)
{
	//��Ҫ���������ڱ���ģʽ
	APICell cell[2];
	Enque("Stor", cell);

	//�ϴ��ļ� ��Ҫ���������ڱ���ģʽ
	memset(sendBuff, 0, SendSize);
	sprintf(sendBuff, "STOR %s\r\n", serverFileName.data());
	//sprintf(sendBuff, "GET  %s\r\n", "1.txt");
	cmdClient.Send(sendBuff, strlen(sendBuff));
	cell[0].WaitResult();
	//memset(recvBuff, 0, RecvSize);
	//cmdClient.Receive(recvBuff, RecvSize);
	//cout << "Stor Recv:" << recvBuff << endl;
	Enque("Type", cell+1);
	//��Ҫ�Լ���������ϴ��ļ�
	memset(sendBuff, 0, SendSize);
	//sprintf(sendBuff, "TYPE BINARY\r\n");	//��һ�б���
	sprintf(sendBuff, "TYPE I\r\n");
	cmdClient.Send(sendBuff, SendSize);
	cell[1].WaitResult();
	//memset(recvBuff, 0, RecvSize);
	//cmdClient.Receive(recvBuff, RecvSize);
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
	return true;
}
void FTPClient::Enque(std::string type, APICell* cell)
{
	cell->callType = type;
	cell->getResult = false;
	cell->content = "";
	callDeque.push_back(cell);
}
void FTPClient::OnCmdClientRecv(SOCKET s, char* buff, int size)
{
	cout << "[recv]:" << buff << endl;
	stringstream ss(buff);
	char line[10480];
	string linestr;
	while (!ss.eof())
	{
		string recv;
		ss.getline(line, 10480);
		linestr = line;
		if (!linestr.empty() && ParseRecvInfo(linestr, recv))
		{
			if (callDeque.size() > 0)
			{
				callDeque.front()->content = recv;
				callDeque.front()->Notify();
				callDeque.pop_front();
			}
		}
	}

	//	cout << "[recv]----------------" << endl;
}