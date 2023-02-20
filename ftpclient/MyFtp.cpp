#include "MyFtp.h"
#include "FTPClient.h"

IFtpClient* GetFtpClient()
{
	static FTPSocket::FTPClient ftpClient;
	return &ftpClient;
}
