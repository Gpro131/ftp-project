#pragma once
#include <string>
class IFtpClient
{
public:

	virtual	void Login(std::string addr, std::string usr, std::string pwd) {}
	/**
	 *�������ı���ģʽ�����غ��ϴ��ļ���Ҫ����ģʽ ��Ҫdataclient�ֶ�����
	 */
	virtual void Pasv() {} ////
	/**
	 *�г��ļ��б� �����ļ��к��ļ�
	 */
	virtual std::string List() { return ""; } //�г��ļ��б� �����ļ��к��ļ�
	/**
	 * ����ģʽ,����̶�Ϊ1026
	 */
	virtual void Port() {}
	virtual void Exit() {}	//�˳�
	/**
	 * ��ӡ��ǰĿ¼
	 */
	virtual	std::string Pwd() { return ""; }
	/**
	 * �ı�������Ĺ���Ŀ¼
	 */
	virtual void Cwd(std::string workDir) {}
	/**
	 * ����Ŀ¼
	 */
	virtual void MakeDiectory(std::string dir) {}
	/**
	 * �����ļ�,���������ڱ���ģʽ
	 */
	virtual void Retr(std::string serverFile, std::string dstFile) {}
	/**
	 * �ϴ��ļ� ���������ڱ���ģʽ
	 */
	virtual void Stor(std::string fileName) {}
};

IFtpClient* GetFtpClient();