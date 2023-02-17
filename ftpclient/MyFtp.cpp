#include "MyFtp.h"
#include "FTPClient.h"

IFtpClient* GetFtpClient()
{
	static FTPClient ftpClient;
	return &ftpClient;
}
