#include "MyFtp.h"
#include "FTPClient.h"

IFtpClient* GetFtpClient()
{
	static FTPSocket::FTPClient ftpClient;
	return &ftpClient;
}

void GiveStr(std::wstring wstr)
{
	wcout << wstr << endl;
	wstr.push_back(L'a');
}

void GiveSS(const wchar_t* data)
{
	wcout << data << endl;
	std::wstring wss = data;
	wcout << wss << endl;
}

void UpdateRef(int& data)
{
	data = 2333;
}

void UpdatePointer(int* data)
{
	*data = 14443;
}
