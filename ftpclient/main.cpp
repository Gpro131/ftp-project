// FTPSocket.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "BaseInc.h"
#include "FTPState.h"
#include "TcpServer.h"
#include "tool.h"
void RecvThread(SOCKET s)
{
	char buff[1024];
	while (1)
	{
		memset(buff, 0, 1024);
		//接受目的主机 client
		int ret = recv(s, buff, 1024, 0);
		if (ret < 0) {
			std::cout << "Failed to receive data!" << std::endl;
			break;
		}
		else {
			cout << buff;
		}
	}

}

char RecvBuff[RecvSize];
char SendBuff[SendSize];
void Send(SOCKET s, char* buff, int size)
{
	if (send(s, buff, size, 0) < 0)
	{
		std::cout << "Failed to send data!" << std::endl;
	}
}
void Receive(SOCKET s)
{
	memset(RecvBuff, 0, RecvSize);
	int ret = recv(s, RecvBuff, RecvSize, 0);
	if (ret < 0) {
		std::cout << "Failed to receive data!" << std::endl;
	}
	cout << RecvBuff << endl;
	//stringstream ss;
	//ss << RecvBuff;
	//int state;
	//string data;
	//ss >> state;
	//ss >> data;
	//cout << "[" << state << "]" << GetDescript(state).data() << endl;
	//cout << data << endl;
}
char* SendUser(string usr)
{
	//sprintf(SendBuff, "", usr.data());
	return SendBuff;
}
/**
 *  FTP 命令
 *  USER %S \r\n   用户名
 *  PASS %S \r\n    密码
 *  QUIT\r\n     退出
 *  PASV\r\n   被动模式
 */
void LoginFtp(SOCKET s, string usr, string pwd)
{
	//登录用户
	memset(SendBuff, 0, SendSize);
	sprintf(SendBuff, "USER %s\r\n", usr.data());
	Send(s, SendBuff, strlen(SendBuff));
	memset(RecvBuff, 0, RecvSize);
	Receive(s);
	//密码
	memset(SendBuff, 0, SendSize);
	sprintf(SendBuff, "PASS %s\r\n", pwd.data());
	Send(s, SendBuff, strlen(SendBuff));
	memset(RecvBuff, 0, RecvSize);
	Receive(s);

}
void WriteCmd(SOCKET s, char* data, int size)
{
	Send(s, SendBuff, strlen(SendBuff));
	memset(RecvBuff, 0, RecvSize);
	Receive(s);
	printf(RecvBuff);
}
void QuitFtp(SOCKET s)
{
	//退出
	memset(SendBuff, 0, SendSize);
	sprintf(SendBuff, "QUIT \r\n");
	Send(s, SendBuff, strlen(SendBuff));
	memset(RecvBuff, 0, RecvSize);
	Receive(s);
	printf(RecvBuff);
}
string GetPortCmd()
{
	// 端口是 4 * 256 +2 = 1026
	return "PORT 10,1,70,31,4,2\r\n";
}
int mains()
{
	//	string ip = "119.45.236.24";
	//	int PORT = 21;
	//#define BUFFER_SIZE 1024
	//	// 初始化socket dll。
	//	WORD winsock_version = MAKEWORD(2, 2);
	//	WSADATA wsa_data;
	//	if (WSAStartup(winsock_version, &wsa_data) != 0) {
	//		std::cout << "Failed to init socket dll!" << std::endl;
	//		return 1;
	//	}
	//
	//	SOCKET clientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//	if (clientSock == INVALID_SOCKET) {
	//		std::cout << "Failed to create server socket!" << std::endl;
	//		return 2;
	//	}
	//
	//	// 绑定IP和端口  及需要连接的目的IP，和端口
	//	sockaddr_in server_addr;
	//	server_addr.sin_family = AF_INET;
	//	server_addr.sin_port = htons(PORT);
	//	server_addr.sin_addr.S_un.S_addr = inet_addr(ip.data());
	//
	//	// connect() 向服务器发起请求，服务器的IP地址和端口号保存在 sockaddr_in 结构体中。直到服务器传回数据后，connect() 才运行结束。
	//	if (connect(clientSock, (LPSOCKADDR)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
	//		std::cout << "Failed to connect server!" << std::endl;
	//		return 3;
	//	}
	//	LoginFtp(clientSock, "root", "1234");
	//	//ListFile(clientSock);
	//	string line;
	//	while (1)
	//	{
	//		getline(cin, line);
	//		line += "\r\n";
	//		memset(SendBuff, 0, SendSize);
	//		strcpy(SendBuff, line.data());
	//		//sprintf(SendBuff, "PWD\r\n");
	//		WriteCmd(clientSock, SendBuff, line.length());
	//	}
	//
	//	QuitFtp(clientSock);

		/*thread th(RecvThread, clientSock);
		th.detach();*/

		//char recv_data[BUFFER_SIZE] = {};

		//while (true) {
		//	std::string data;
		//	std::cout << "Input data: ";
		//	std::cin >> data;

		//	if (send(clientSock, data.c_str(), data.size(), 0) < 0) {
		//		std::cout << "Failed to send data!" << std::endl;
		//		break;
		//	}
		//	//接受目的主机 client
		//	/*int ret = recv(clientSock, recv_data, BUFFER_SIZE, 0);
		//	if (ret < 0) {
		//		std::cout << "Failed to receive data!" << std::endl;
		//		break;
		//	}
		//	std::cout << "Receive data from server: " << recv_data << std::endl;*/

		//	if (data == "Exit") {
		//		std::cout << "Exit!" << std::endl;
		//		break;
		//	}
		//}

		//closesocket(clientSock);
		//WSACleanup();

	return 0;

}


#include "TcpClient.h"
#include "FTPClient.h"
int mainda()
{
	WORD winsock_version = MAKEWORD(2, 2);
	WSADATA wsa_data;
	if (WSAStartup(winsock_version, &wsa_data) != 0) {
		std::cout << "Failed to init socket dll!" << std::endl;
		return 1;
	}
	/*TcpClient tcpClientCmd;
	tcpClientCmd.Init("119.45.236.24", 21);
	char* recv;
	int size;
	tcpClientCmd.Receive(recv, size);
	LoginFtp(tcpClientCmd.clientSock, "root", "1234");
	QuitFtp(tcpClientCmd.clientSock);*/
	//memset(tcpClientCmd.sendBuff, 0, tcpClientCmd.RecvSize);
	//tcpClientCmd.Send();
	//服务器被动 客户端主动
	TcpServer tcpServerData;  //自己开一个端口
	tcpServerData.Init(1026);
	char* recv;
	int size;
	tcpServerData.StartRecvThread();

	TcpClient tcpClientCmd;
	tcpClientCmd.Init("127.0.0.1", 21);
	//char* recv;
	//int size;
	//tcpClientCmd.Receive(recv, size);
	cout << "请输入以下命令" << endl;
	char buff0[255];
	char buff1[255];
	char buff2[255];
	std::string line;
	std::string cmd, arg0, arg1, arg2;

	stringstream ss;
	while (line != "exit")
	{
		getline(cin, line);
		ss << line;
		while (!ss.eof())
		{
			ss >> cmd;
			if (cmd == "login")
			{
				ss >> arg0;
				ss >> arg2;
				LoginFtp(tcpClientCmd.clientSock, "root", "1234");

			}


		}


	}

	//LoginFtp(tcpClientCmd.clientSock, "root", "1234");
	//Receive(tcpClientCmd.clientSock);
	////memset(tc, 0, RecvSize);
	//string portCmd = GetPortCmd();
	//WriteCmd(tcpClientCmd.clientSock, const_cast<char*>(portCmd.data()), portCmd.length());
	//string listCmd = "LIST\r\n";
	//WriteCmd(tcpClientCmd.clientSock, const_cast<char*>(listCmd.data()), listCmd.length());

	//QuitFtp(tcpClientCmd.clientSock);


	//tcpServerCmd.Receive(RecvBuff, size);
	//LoginFtp(tcpClientCmd.clientSock, "root", "1234");
	//QuitFtp(tcpClientCmd.clientSock);
	//memset(tcpClientCmd.sendBuff, 0, tcpClientCmd.RecvSize);
	//tcpClientCmd.Send();
	tcpServerData.Close();
	tcpClientCmd.Close();
	WSACleanup();
	return 0;
}
void GetIp()
{
	int ip[4];
	GetIpData(ip, 4);
}
int main()
{
	stringstream ss("1,2,3,4,5");
	char ch[1024];
	while (!ss.eof())
	{
		ss.getline(ch, 1024, ',');
		cout << ch;
	}
	cout << endl;
	std::string res = SubString("abc(1(2)3)dda", '(', ')');
	cout << "res:" << res << endl;
	/*std::string fileName = "C:/Users/Administrator/source/repos/FTPSocket/FTPSocket/main.cpp";
	ifstream ifs;
	ifs.open(fileName.data(), std::ios::in | std::ios::binary);
	if (!ifs.is_open())
		cout << "ifs not open" << endl;
	else {
		char buff[1024];
		while (!ifs.eof())
		{
			memset(buff, 0, 1024);
			ifs.read(buff, 1024);
			cout << ifs.gcount() << endl;
		}
	}*/
	GetIp();
	//ifs.close();
	using namespace FTPSocket;
	FTPClient f;
	std::wstring  wServerAddr = String2WString(GetLocalIP());
	//std::wstring  wServerAddr = L"192.168.1.57"
	f.Login(wServerAddr.data(), L"root", L"1234");
	f.Pasv();
	std::vector<FileInfo> files;
	f.ListAllFileAndFolders(files);
	//f.Login("192.168.1.66", "root", "1234");
	//如果是127.0.0.1的话，主动模式需要报告客户端的ip
	//如果是本机的局域网ip，主动需要报告客户端的局域网ip，配套即可
	//f.Login(GetLocalIP(), "root", "1234");

	string line;
	char lineBuff[2048];
	//std::stringstream ss;
	ss.clear();
	ss.seekg(0);
	ss.str("");
	string cmd, arg0, arg1, arg2;

	while (line != "exit")
	{
		getline(cin, line);
		ss.str(line);
		ss >> cmd;
		if (cmd == "Pasv")
		{
			f.Pasv();
		}
		if (cmd == "Port")
		{
			f.Port();
		}
		if (cmd == "Stor")
		{
			cout << "上传文件" << arg0;
			ss >> arg0 >> arg1;
			f.Stor(String2WString(arg0).data(), String2WString(arg1).data(), 2048);
		}
		if (cmd == "List")
		{
			f.Pasv();
			int maxFileCount =	f.ListAllFileAndFolders();
			// cout<<"List"<<	f.List()<<endl;
			FileInfo fi;
			for (int i = 0; i < maxFileCount; i++)
			{
				if (f.GetFileInfo(fi, i))
				{
					const wchar_t* filetype = fi.isFile ? L"file" : L"dir";
					wcout << fi.author << L" " << fi.fileName << L" " << filetype << L" " << fi.fileSize << L" " << fi.authority << endl;
				}
				//wprintf(L"%s %s %s %d ", fi[i].authority, fi[i].isFile ? "file" : "dir", fi[i].fileSize, fi[i].fileName);
			}
			f.ClearFileList();

		}
		if (cmd == "Retr")
		{
			cout << "下载文件：服务器文件：" << arg0 << "客户端目标文件位置：" << arg1;
			ss >> arg0 >> arg1;
			f.Retr(String2WString(arg0).data(),
				String2WString(arg1).data(), 2048);
		}
		ss.str("");
		ss.seekg(0);
		ss.clear();
	}




	//cout << "服务器被动模式" << endl;
	////f.Pasv();
	////f.Stor("C:/1.ddd");
	//cout << "服务器主动模式" << endl;
	//::Sleep(1000);
	//f.Port();
	//f.Pasv();

	/*::Sleep(1000);
	cout<<"pwd:"<<f.Pwd()<<endl;
	f.Cwd("abc");
	cout<<"pwd:"<<f.Pwd() << endl;*/
	//f.MakeDiectory();
	/*f.List();
	f.List();
	f.List();

	::Sleep(1000);
	f.Retr("1.log", "2.log");*/
	//f.Stor(fileName);
	//string line;
	//cin >> line;
	cout << "结束" << endl;
	f.Exit();
	return 0;
}