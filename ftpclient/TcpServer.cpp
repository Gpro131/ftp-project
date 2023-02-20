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
}

void TcpServer::RecvThread()
{

	fd_set readfds;
	fd_set writefds;
	fd_set execeptfds;
	while (isRecvThreadRunnning)
	{
		//清理集合
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		FD_ZERO(&execeptfds);

		//将描述符 加入集合

		FD_SET(serverSock, &readfds);
		FD_SET(serverSock, &writefds);
		FD_SET(serverSock, &execeptfds);


		for (int n = clientList.size() - 1; n >= 0; n--)
		{
			FD_SET(clientList[n], &readfds);
		}
		timeval timeout = { 100,0 };
		int ret = select(serverSock + 1, &readfds, &writefds, &execeptfds, nullptr);
		if (ret < 0)
		{
			break;
			//printf("select 任务结束，called failed:%s.\r\n", WSAGetLastError());
		}
		//cout << readfds.fd_count << endl;
		//判断是否有数据可以读取

		for (u_int i = 0; i < readfds.fd_count; i++)
		{
			if (readfds.fd_array[i] == serverSock)
			{
				SOCKET s = AcceptClient();
				//FD_SET(s, &readfds);
				//FD_SET(s, &writefds);
			}
			else {
				const int nMaxSize = 21500;
				char buff[11500] = { 0 };
				SOCKET s = readfds.fd_array[i];
				int nRecv = recv(s, buff, 21500, 0);
				if (nRecv > 0)
				{
					RecvHandler("msg",s, buff, nRecv);
				}
				else if (nRecv == 0)
				{
					//continue;
					printf("client off\r\n");
					printf("client :[%d]", clientList.size());
					//客户端下线
					FD_CLR(s, &readfds);
					//FD_CLR(s, &writefds);
					ClientOffline(s);

				}
				else {
					//continue;
					printf("client err\r\n");
					//printf("client :[%d]", clientList.size());
					//出错
					int a = WSAGetLastError();
					switch (a)
					{
					case 10014:
					case 10054:
					{
						SOCKET s = readfds.fd_array[i];
						FD_CLR(s, &readfds);
						//FD_CLR(s, &writefds);
						ClientOffline(s);
					}
					}
				}

			}
		}

		//if (FD_ISSET(serverSock, &readfds))
		//{
		//	//FD_CLR
		//	AcceptClient();
		//}

		////判断是否有数据可以读取
		//if (FD_ISSET(serverSock, &readfds))
		//{
		//	//FD_CLR
		//	AcceptClient();
		//}
	}
}

void TcpServer::StartRecvThread()
{
	isRecvThreadRunnning = true;
	std::thread th(std::bind(&TcpServer::RecvThread, this));
	th.detach();
}

void TcpServer::Send(char* buff, int size)
{
	if (clientList.size() > 0)
	{
		int nRet = send(clientList.back(), buff, size,0);
		if (nRet < 0) {
			printf("send failed");
		}
	}
}

SOCKET TcpServer::AcceptClient()
{
	int addrlen = 0;
	sockaddr_in  sa;
	addrlen = sizeof(sa);
	SOCKET clntSock = INVALID_SOCKET;
	memset(&sa, 0, sizeof(sa));
	clntSock = accept(serverSock, (struct sockaddr*)(&sa), &addrlen);
	if (clntSock != INVALID_SOCKET || addrlen > 0)
	{
		printf("新客户端<Sokcet=%d>加入,Ip地址：%s,端口号：%d\n", clntSock, inet_ntoa(sa.sin_addr),
			ntohs(sa.sin_port));
		cout << "new client connect" << endl;
		clientList.push_back(clntSock);
	}
	return clntSock;
	//cout << "." << endl;
}

void TcpServer::ClientOffline(SOCKET s)
{
	//FD_CLR(s, &writefds);
	closesocket(s);
	auto eraseIt = std::remove_if(clientList.begin(), clientList.end(), [s](SOCKET itemS)
		{
			return s == itemS;
		});
	clientList.erase(eraseIt);
}

