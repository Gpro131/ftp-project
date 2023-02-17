#pragma once
#include "BaseInc.h"
#include "TcpClient.h"
#include "TcpServer.h"
#include "MyFtp.h"
enum FileColumnAttrib {
	LC_AUTHORITY = 0,
	LC_FileType = 1,
	LC_USER = 2,
	LC_GROUP = 3,
	LC_FILESIZE = 4,
	LC_DATE_MONTH = 5,
	LC_DATE_DAY = 6,
	LC_FILENAME = 7
};
struct APICell
{
	string callType;
	bool getResult;
	string content;
	std::mutex mtx;
	std::unique_lock<std::mutex> locker;
	std::condition_variable cv;
	APICell()
	{
		getResult = false;
		std::unique_lock<std::mutex> locker1(mtx);
		locker.swap(locker1);
	}

	void WaitResult() {
		//cv.wait_for(locker,std::chrono::duration_cast<std::chrono::seconds>(5));
		cv.wait(locker);
	}
	void Notify()
	{
		cv.notify_one();
	}
};

class FTPClient : public IFtpClient
{
public:
	FTPClient() {
		dataServer.RecvHandler = std::bind(&FTPClient::OnRecvDataChannel, this,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

		cmdClient.RecvHandler = std::bind(&FTPClient::OnCmdClientRecv, this,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		
	}
	/**
	 *
	 */
	virtual void Login(string addr, string usr, string pwd);
	/**
	 *�������ı���ģʽ�����غ��ϴ��ļ���Ҫ����ģʽ ��Ҫdataclient�ֶ�����
	 */
	virtual void Pasv(); ////
	/**
	 *�г��ļ��б� �����ļ��к��ļ� ����ģʽ
	 */
	virtual std::string  List(); //�г��ļ��б� �����ļ��к��ļ�
	/**
	 * ����ģʽ
	 */
	virtual void Port();
	virtual void Exit();	//�˳�
	/**
	 * ��ӡ��ǰĿ¼
	 */
	virtual std::string Pwd();
	/**
	 * �ı�������Ĺ���Ŀ¼
	 */
	virtual void Cwd(std::string workDir);
	/**
	 * ����Ŀ¼
	 */
	virtual void MakeDiectory();
	/**
	 * �����ļ�,���������ڱ���ģʽ
	 */
	virtual void Retr(string serverFile, string dstFile);
	/**
	 * �ϴ��ļ� ���������ڱ���ģʽ
	 */
	virtual void Stor(string fileName,string destFileName);
private:
	char sendBuff[SendSize];
	char recvBuff[RecvSize];
	void OnRecvDataChannel(SOCKET, char*, int);
	void SendData(SOCKET);
	string GetPortCmd(int port)
	{
		int p1 = port / 256;
		int p2 = port % 256;
		// 13264 51 208
		// �˿��� 4 * 256 +2 = 1026
		char buff[1024];
		memset(buff, 0, 1024);
		sprintf(buff, "PORT 127,0,0,1,%d,%d\r\n", p1, p2);
		return buff;
	}
	void ParseListAck(char* buff, int size);
private:
	TcpClient cmdClient;
	TcpClient dataClient;	//����ģʽʹ��
	TcpServer dataServer;	//����ģʽʹ��
	string curCmd;
	string downFileName;
	ofstream ofs;
	void Enque(std::string type, APICell* cell);
	bool ParseRecvInfo(std::string content, std::string& recvContnet);
	std::deque<APICell*>  callDeque;


	void OnCmdClientRecv(SOCKET s,char* buff,int size);
};