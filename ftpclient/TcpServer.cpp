#include "TcpServer.h"

void TcpServer::Init(int port)
{
	serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSock == INVALID_SOCKET) {
		std::cout << "Failed to create server socket!" << std::endl;
	}

	// 绑定IP和端口  及需要连接的目的IP，和端口
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons((u_short)port);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	if (::bind(serverSock, (sockaddr*)&server_addr, sizeof(server_addr)) != 0)
	{
		std::cout << "Failed to bind server!" << std::endl;
	}
	if (::listen(serverSock, 10) != 0)
	{
		std::cout << "Failed to listen server!" << std::endl;
	}

	//服务器已监听
	cout << "start listen 21" << endl;
}

void TcpServer::StartReceiveThread(bool isBlocked)
{
	if (isBlocked)
	{
		//th.join();
		ReceiveThread();
	}
	else
	{
		thread th(&TcpServer::ReceiveThread, this);
		th.detach();
	}
}

void TcpServer::StartAcceptThread(bool isBlocked/*=false*/)
{
	thread th(&TcpServer::AcceptThread, this);
	th.detach();
}

void TcpServer::Send(char* buff, int size)
{
	if (size > 0)
	{ 
		if (send(s[0], buff, size, 0) < 0)
		{
			std::cout << "Failed to send data!" << std::endl;
		}
	}
}

int TcpServer::Receive(char* buff, int size)
{
	int nRecvSize = recv(serverSock, buff, RecvSize, 0);
	if (nRecvSize <= 0) {
		std::cout << "Failed to receive data!" << std::endl;
	}
	return nRecvSize;
}

void TcpServer::Close()
{
	isRunning = false;
	closesocket(serverSock);
}

void TcpServer::ReceiveThread()
{
	int addrlen = 0;
	sockaddr_in  sa;
	addrlen = sizeof(sa);
	SOCKET clntSock = INVALID_SOCKET;
	cout << "TCP SERVER" << endl;
	while (true)
	{
		memset(&sa, 0, sizeof(sa));
		clntSock = accept(serverSock, (struct sockaddr*)(&sa), &addrlen);
		if (clntSock != INVALID_SOCKET || addrlen > 0)
		{
			cout << "server connect to client" << endl;
			this->s.push_back(clntSock);
			break;
		}
		cout << "." << endl;
	}

	memset(recvBuff, 0, RecvSize);
	cout << "client is connected" << endl;
	isRunning = true;
	while (isRunning)
	{
		int nRecvLen = recv(clntSock, recvBuff, RecvSize, 0);
		if (nRecvLen >=0)
		{
		//	cout <<recvBuff;
			this->RecvHandler(clntSock, recvBuff, RecvSize);
			memset(recvBuff, 0, RecvSize);
		}
	}
}

void TcpServer::AcceptThread()
{
	int addrlen = 0;
	sockaddr_in  sa;
	addrlen = sizeof(sa);
	SOCKET clntSock = INVALID_SOCKET;
	while (true)
	{
		memset(&sa, 0, sizeof(sa));
		clntSock = accept(serverSock, (struct sockaddr*)(&sa), &addrlen);
		if (clntSock != INVALID_SOCKET || addrlen > 0)
		{
			this->s.push_back(clntSock);
			break;
		}
		//cout << "." << endl;
	}
}
