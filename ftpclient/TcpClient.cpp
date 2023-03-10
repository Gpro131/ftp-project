#include "BaseInc.h"
#include "TcpClient.h"
void TcpClient::Init(string addr, int port)
{
	clientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSock == INVALID_SOCKET) {
		std::cout << "Failed to create server socket!" << std::endl;
	}

	// 绑定IP和端口  及需要连接的目的IP，和端口
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.S_un.S_addr = inet_addr(addr.data());

	// connect() 向服务器发起请求，服务器的IP地址和端口号保存在 sockaddr_in 结构体中。直到服务器传回数据后，connect() 才运行结束。
	if (connect(clientSock, (LPSOCKADDR)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
		std::cout << "Failed to connect server!" << std::endl;
	}
}

void TcpClient::StartReceiveThread()
{
	thread th(&TcpClient::ReceiveThread, this);
	th.detach();
}

void TcpClient::Send(char* buff, int size)
{
	if (send(clientSock, buff, size, 0) < 0)
	{
		std::cout << "Failed to send data!" << std::endl;
	}
}

void TcpClient::Receive(char* buff, int size)
{
	int ret = recv(clientSock, buff, size, 0);
	if (ret < 0) {
		std::cout << "Failed to receive data!" << std::endl;
	}
	else {
		cout << "[recv]" << buff << endl;
	}
}
string  TcpClient::ReceiveStr()
{
	string recvStr;
	int ret = 1;
	char buff[20480];
	while (ret > 0)
	{
		memset(buff, 0, 2048);
		ret = recv(clientSock, buff, 20480, 0);
		for (int i = 0;i < ret;i++)
		{
			recvStr.push_back(buff[i]);
		}
	}
	return	recvStr;
}

void TcpClient::Shutdown()
{
	/*shutdown(clientSock, SD_SEND);
	::Sleep(200);
	shutdown(clientSock, SD_RECEIVE);*/
	shutdown(clientSock,SD_BOTH);
}

void TcpClient::ReceiveThread()
{
	char recvBuff[20480];
	isRunning = true;
	while (isRunning)
	{
		memset(recvBuff, 0, 20480);
		int ret = recv(clientSock, recvBuff, 20480, 0);
		if (ret > 0) {
			//cout << "client["<< clientSock <<"] thread:" << recvBuff << endl;
			RecvHandler("msg", clientSock, recvBuff, ret);
		}
		else {
			cout << "disconnect" << endl;
			RecvHandler("disconnect", clientSock, recvBuff, ret);
			Shutdown();
			break;
		}
		::Sleep(5);
	}
	Close();
}

void TcpClient::Close()
{
	isRunning = false;
	closesocket(clientSock);
}

