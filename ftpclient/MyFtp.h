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
	 *�������ı���ģʽ�����غ��ϴ��ļ���Ҫ����ģʽ ��Ҫdataclient�ֶ�����
	 */
	virtual void Pasv() {} ////
	/**
	 *�г��ļ��б� �����ļ��к��ļ�
	 */
	virtual const wchar_t* List() { return L""; } //�г��ļ��б� �����ļ��к��ļ�
	virtual void ListAllFileAndFolders(std::vector<FileInfo>& files) {  } //�г��ļ��б� �����ļ��к��ļ�
	virtual int ListAllFileAndFolders() { return 0; } //�г��ļ��б� �����ļ��к��ļ� �����ļ�����
	virtual bool GetFileInfo(FileInfo& fi,int idx) { return false; } //��ȡ��Ҫ�оٵ��ļ��б�
	virtual void ClearFileList() {} //��ȡ��Ҫ�оٵ��ļ��б�

	/**
	 * ����ģʽ,����̶�Ϊ1026
	 */
	virtual void Port() {}
	virtual void Exit() {}	//�˳�
	/**
	 * ��ӡ��ǰĿ¼
	 */
	virtual	const wchar_t* Pwd() { return L""; }
	/**
	 * �ı�������Ĺ���Ŀ¼
	 */
	virtual void Cwd(const wchar_t* workDir) {}
	/**
	 * ������һ�㹤��Ŀ¼
	 */
	virtual void Cdup() {}
	/**
	 * ����Ŀ¼
	 */
	virtual void MakeDiectory(const wchar_t* workDir) {}
	/**
	 * �����ļ�,���������ڱ���ģʽ
	 */
	 //virtual void Retr(wchar_t* serverFile, wchar_t* dstFile) {}
	virtual void Retr(const wchar_t* serverFile, const wchar_t* dstFile, int fileSize, IFileTransferObserver* observer = nullptr) {}
	/**
	 * �ϴ��ļ� ���������ڱ���ģʽ
	 */
	virtual void Stor(const wchar_t* serverFile, const wchar_t* dstFile, int fileSize, IFileTransferObserver* observer = nullptr) {}
};

IFtpClient* GetFtpClient();


void GiveStr(std::wstring wstr);

void GiveSS(const wchar_t* data);
void UpdateRef(int& data);
void UpdatePointer(int* data);