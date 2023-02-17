#pragma once
#include "BaseInc.h"
class TcpClient
{
public:
	void Init(string addr, int port);
	void StartReceiveThread();
	void Send(char* buff, int size);
	void Receive(char* buff, int size);
	string ReceiveStr();

	void Close();


	SOCKET serverSock;
	SOCKET clientSock;
	int port;
	string addr;
	bool GetIsRunning() { return isRunning; }
private:
	void ReceiveThread();
	bool isRunning;
};