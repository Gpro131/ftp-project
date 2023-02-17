#pragma once
#include "BaseInc.h"
class TcpClient
{
public:
	~TcpClient() { isRunning = false; }
	void Init(string addr, int port);
	void StartReceiveThread();
	void Send(char* buff, int size);
	void Receive(char* buff, int size);
	string ReceiveStr();

	void Close();

	std::function<void(SOCKET, char*, int)> RecvHandler;

	SOCKET serverSock;
	SOCKET clientSock;
	int port;
	string addr;
	bool GetIsRunning() { return isRunning; }
private:
	void ReceiveThread();
	bool isRunning;
};