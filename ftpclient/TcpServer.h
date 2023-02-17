#pragma once
#include "BaseInc.h"
class TcpServer
{
public:
	void Init(int port);
	void StartReceiveThread(bool isBlocked=false);
	void StartAcceptThread(bool isBlocked=false);
	void Send(char* buff, int size);
	int Receive(char* buff, int size);
	void Close();
	bool GetIsRunning() { return isRunning; }
	char sendBuff[2048];
	char recvBuff[RecvSize];
	std::vector<SOCKET> s;
	function<void(SOCKET, char*, int)> RecvHandler;
private:
	void ReceiveThread();
	void AcceptThread();
	bool isRunning;

	SOCKET serverSock;
	int port;

};

