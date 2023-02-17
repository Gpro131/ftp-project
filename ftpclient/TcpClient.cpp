#include "BaseInc.h"
#include "TcpClient.h"
void TcpClient::Init(string addr, int port)
{
	clientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSock == INVALID_SOCKET) {
		std::cout << "Failed to create server socket!" << std::endl;
	}

	// ��IP�Ͷ˿�  ����Ҫ���ӵ�Ŀ��IP���Ͷ˿�
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.S_un.S_addr = inet_addr(addr.data());

	// connect() ��������������󣬷�������IP��ַ�Ͷ˿ںű����� sockaddr_in �ṹ���С�ֱ���������������ݺ�connect() �����н�����
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
		cout <<"[recv]"<< buff << endl;
	}
}
string  TcpClient::ReceiveStr()
{
	string recvStr;
	int ret = 1;
	char buff[20480];
	while (ret > 0)
	{
		memset(buff,0,2048);
		ret = recv(clientSock, buff, 20480, 0);
		for (int i = 0;i < ret;i++)
		{
			recvStr.push_back(buff[i]);
		}
	}
	return	recvStr;
}

void TcpClient::ReceiveThread()
{
	char recvBuff[RecvSize];
	isRunning = true;
	while (isRunning)
	{
		int ret = recv(clientSock, recvBuff, RecvSize, 0);
		if (ret >= 0) {
			cout << recvBuff << endl;
			memset(recvBuff, 0, RecvSize);
		}
	}
}

void TcpClient::Close()
{
	isRunning = false;
	closesocket(clientSock);
}

