#pragma once
#include <string>
class IFtpClient
{
public:

	virtual	void Login(std::string addr, std::string usr, std::string pwd) {}
	/**
	 *服务器的被动模式，下载和上传文件需要被动模式 需要dataclient手动连接
	 */
	virtual void Pasv() {} ////
	/**
	 *列出文件列表 包括文件夹和文件
	 */
	virtual std::string List() { return ""; } //列出文件列表 包括文件夹和文件
	/**
	 * 主动模式,这个固定为1026
	 */
	virtual void Port() {}
	virtual void Exit() {}	//退出
	/**
	 * 打印当前目录
	 */
	virtual	std::string Pwd() { return ""; }
	/**
	 * 改变服务器的工作目录
	 */
	virtual void Cwd(std::string workDir) {}
	/**
	 * 创建目录
	 */
	virtual void MakeDiectory(std::string dir) {}
	/**
	 * 下载文件,服务器处于被动模式
	 */
	virtual void Retr(std::string serverFile, std::string dstFile) {}
	/**
	 * 上传文件 服务器处于被动模式
	 */
	virtual void Stor(std::string fileName) {}
};

IFtpClient* GetFtpClient();