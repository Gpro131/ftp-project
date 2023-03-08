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

	virtual	bool Login(const wchar_t* addr, const wchar_t* usr, const wchar_t* pwd) { return true; }
	/**
	 *列出文件列表 包括文件夹和文件
	 */
	virtual const wchar_t* List() { return L""; } //列出文件列表 包括文件夹和文件
	//列出文件列表 包括文件夹和文件，这个不要用，传不到ue中
	virtual void ListAllFileAndFolders(std::vector<FileInfo>& files) {  } 
	//列出文件列表 包括文件夹和文件
	virtual int ListAllFileAndFolders() { return 0; }
	//列出文件列表 包括文件夹和文件 返回文件数量
	virtual bool GetFileInfo(FileInfo& fi,int idx) { return false; }
	//获取需要列举的文件列表
	virtual void ClearFileList() {} //获取需要列举的文件列表
	//
	virtual void Exit() {}	//退出
	/** 
	 * 打印当前目录
	 */
	virtual	const wchar_t* Pwd() { return L""; }
	/**
	 * 改变服务器的工作目录
	 */
	virtual bool Cwd(const wchar_t* workDir) { return false; }
	/**
	 * 进入上一层工作目录
	 */
	virtual bool Cdup() { return false; }
	/**
	 * 创建目录
	 */
	virtual void MakeDiectory(const wchar_t* workDir) {}
	/**
	 * 下载文件，fileSize通过GetFileInfo获取才能获取到正确的进度
	 */
	virtual void Retr(const wchar_t* serverFile, const wchar_t* dstFile, int fileSize, IFileTransferObserver* observer = nullptr) {}
	/**
	 * 上传文件
	 */
	virtual void Stor(const wchar_t* localFile, const wchar_t* dstFileInServer, IFileTransferObserver* observer = nullptr) {}
	/**
	 * 删除文件
	 */
	virtual bool DeleteFile(const wchar_t* serverFile) { return false; }
	/**
	 * 删除文件夹
	 */
	virtual bool DeleteDirectory(const wchar_t* serverFile) { return false; }
	/**
	 * 重命名文件夹
	 */
	virtual bool RenameFile(const wchar_t* serverFile, const wchar_t* destFile) { return false; }
};

IFtpClient* GetFtpClient();
