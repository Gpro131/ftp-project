#pragma once
#include "BaseInc.h"
namespace FTPSocket
{
	/*void Login(string usr, string pwd) {}
	void List() {}
	void Quit() {}*/
	enum FtpCmd {
		Pasv,
		Port,
		List,
		Stor,
		Retr,
		Cwd,
		Cdup,
		Pwd,
		MakeDir,
		Exit
	};
	

}