#pragma once
#include<vector>
#include <string>
#include<functional>
struct FileInfo
{
	wchar_t fileName[255];
	bool isFile{ false };
	wchar_t author[255];	//上传者
	int fileSize{ 0 };
	wchar_t authority[10];
	FileInfo()
	{
		memset(fileName, 0, sizeof(wchar_t));
		memset(authority, 0, sizeof(wchar_t));
		memset(author, 0, sizeof(wchar_t));
	}
};
using RecvProgressCallBack = std::function<void(float percent, const wchar_t* fileName)>;
using RecvFinishCallBack = std::function<void(const wchar_t* fileName)>;
class IFileTransferObserver {
public:
	virtual void DownloadFileProgressCallBack(int taskID, float percent, const wchar_t* fileName) {}
	virtual void DownloadFinishCallBack(int taskID, const wchar_t* fileName) {}
	virtual void UploadFileProgressCallBack(int taskID, float percent, const wchar_t* fileName) {}
	virtual void UploadFinishCallBack(int taskID, const wchar_t* fileName) {}

};


class IFtpClient
{
public:

	virtual	void Login(std::wstring  addr, std::wstring  usr, std::wstring  pwd) {}
	/**
	 *服务器的被动模式，下载和上传文件需要被动模式 需要dataclient手动连接
	 */
	virtual void Pasv() {} ////
	/**
	 *列出文件列表 包括文件夹和文件
	 */
	virtual std::wstring List() { return L""; } //列出文件列表 包括文件夹和文件
	virtual void ListAllFileAndFolders(std::vector<FileInfo>& files) {  } //列出文件列表 包括文件夹和文件

	/**
	 * 主动模式,这个固定为1026
	 */
	virtual void Port() {}
	virtual void Exit() {}	//退出
	/**
	 * 打印当前目录
	 */
	virtual	std::wstring Pwd() { return L""; }
	/**
	 * 改变服务器的工作目录
	 */
	virtual void Cwd(std::wstring workDir) {}
	/**
	 * 创建目录
	 */
	virtual void MakeDiectory(std::wstring workDir) {}
	/**
	 * 下载文件,服务器处于被动模式
	 */
	 //virtual void Retr(wchar_t* serverFile, wchar_t* dstFile) {}
	virtual void Retr(std::wstring serverFile, std::wstring dstFile, int fileSize, IFileTransferObserver* observer = nullptr) {}
	/**
	 * 上传文件 服务器处于被动模式
	 */
	virtual void Stor(std::wstring serverFile, std::wstring dstFile, int fileSize, IFileTransferObserver* observer = nullptr) {}
};

IFtpClient* GetFtpClient();