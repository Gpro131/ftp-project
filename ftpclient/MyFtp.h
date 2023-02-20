#pragma once
#include<vector>
#include <string>
#include<functional>
struct FileInfo
{
	wchar_t fileName[255];
	bool isFile{ false };
	wchar_t author[255];	//�ϴ���
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
	 *�������ı���ģʽ�����غ��ϴ��ļ���Ҫ����ģʽ ��Ҫdataclient�ֶ�����
	 */
	virtual void Pasv() {} ////
	/**
	 *�г��ļ��б� �����ļ��к��ļ�
	 */
	virtual std::wstring List() { return L""; } //�г��ļ��б� �����ļ��к��ļ�
	virtual void ListAllFileAndFolders(std::vector<FileInfo>& files) {  } //�г��ļ��б� �����ļ��к��ļ�

	/**
	 * ����ģʽ,����̶�Ϊ1026
	 */
	virtual void Port() {}
	virtual void Exit() {}	//�˳�
	/**
	 * ��ӡ��ǰĿ¼
	 */
	virtual	std::wstring Pwd() { return L""; }
	/**
	 * �ı�������Ĺ���Ŀ¼
	 */
	virtual void Cwd(std::wstring workDir) {}
	/**
	 * ����Ŀ¼
	 */
	virtual void MakeDiectory(std::wstring workDir) {}
	/**
	 * �����ļ�,���������ڱ���ģʽ
	 */
	 //virtual void Retr(wchar_t* serverFile, wchar_t* dstFile) {}
	virtual void Retr(std::wstring serverFile, std::wstring dstFile, int fileSize, IFileTransferObserver* observer = nullptr) {}
	/**
	 * �ϴ��ļ� ���������ڱ���ģʽ
	 */
	virtual void Stor(std::wstring serverFile, std::wstring dstFile, int fileSize, IFileTransferObserver* observer = nullptr) {}
};

IFtpClient* GetFtpClient();