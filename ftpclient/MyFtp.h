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
		memset(fileName, 0, sizeof(wchar_t)*255);
		memset(author, 0, sizeof(wchar_t)*255);
		memset(authority, 0, sizeof(wchar_t)*10);
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

	virtual	void Login(const wchar_t* addr, const wchar_t* usr, const wchar_t* pwd) {}
	/**
	 *服务器的被动模式，下载和上传文件需要被动模式 需要dataclient手动连接
	 */
	virtual void Pasv() {} ////
	/**
	 *列出文件列表 包括文件夹和文件
	 */
	virtual const wchar_t* List() { return L""; } //列出文件列表 包括文件夹和文件
	virtual void ListAllFileAndFolders(std::vector<FileInfo>& files) {  } //列出文件列表 包括文件夹和文件
	virtual int ListAllFileAndFolders() { return 0; } //列出文件列表 包括文件夹和文件 返回文件数量
	virtual bool GetFileInfo(FileInfo& fi,int idx) { return false; } //获取需要列举的文件列表
	virtual void ClearFileList() {} //获取需要列举的文件列表

	/**
	 * 主动模式,这个固定为1026
	 */
	virtual void Port() {}
	virtual void Exit() {}	//退出
	/**
	 * 打印当前目录
	 */
	virtual	const wchar_t* Pwd() { return L""; }
	/**
	 * 改变服务器的工作目录
	 */
	virtual void Cwd(const wchar_t* workDir) {}
	/**
	 * 进入上一层工作目录
	 */
	virtual void Cdup() {}
	/**
	 * 创建目录
	 */
	virtual void MakeDiectory(const wchar_t* workDir) {}
	/**
	 * 下载文件,服务器处于被动模式
	 */
	 //virtual void Retr(wchar_t* serverFile, wchar_t* dstFile) {}
	virtual void Retr(const wchar_t* serverFile, const wchar_t* dstFile, int fileSize, IFileTransferObserver* observer = nullptr) {}
	/**
	 * 上传文件 服务器处于被动模式
	 */
	virtual void Stor(const wchar_t* serverFile, const wchar_t* dstFile, int fileSize, IFileTransferObserver* observer = nullptr) {}
};

IFtpClient* GetFtpClient();


void GiveStr(std::wstring wstr);

void GiveSS(const wchar_t* data);
void UpdateRef(int& data);
void UpdatePointer(int* data);