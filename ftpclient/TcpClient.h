#pragma once
#include "BaseInc.h"
class TcpClient
{
public:
	~TcpClient() { isRunning = false; }
	void Init(std::string addr, int port);
	void StartReceiveThread();
	void Send(char* buff, int size);
	void Receive(char* buff, int size);
	std::string ReceiveStr();

	void Close();
	//第一个参数  msg 收到信息，如果是断开连接，则是disconnect
	std::function<void(std::string,SOCKET, char*, int)> RecvHandler;

	SOCKET serverSock;
	SOCKET clientSock;
	int port;
	std::string addr;
	std::string name;
	bool GetIsRunning() { return isRunning; }
private:
	void ReceiveThread();
	bool isRunning;
};