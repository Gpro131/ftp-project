#pragma once
#include "BaseInc.h"
#include "TcpClient.h"
#include "TcpServer.h"
#include "MyFtp.h"
#include "FTPCmd.h"
#include "tool.h"
namespace FTPSocket
{

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
	class Msg {
	public:
		int code;
		string content;
		Msg() {}
		Msg(const string& content)
		{
			ParseMsg(content);
		}
		//
		void ParseMsg(const string& content)
		{
			size_t firstspacepos = content.find_first_of(' ');
			std::string strMsgCode = content.substr(0, firstspacepos);
			if (strMsgCode.empty() && !IsDigital(strMsgCode)) return;
			code = atoi(strMsgCode.data());
			//this->content =  content.substr(firstspacepos, content.size());
		}
		Msg operator = (const string& content)
		{
			ParseMsg(content);
			return *this;
		}
	};


	struct FSCommand
	{
		string callType;
		bool getResult;
		//string content;
		std::vector<std::string> contents;
		std::vector<Msg> recvMsgs;
		Msg recvMsg;
		WaitEvent evt;
		int nRecvTimes{ 0 };	//考虑到多次响应 比如List,Stor,Retr
		int nMaxTimes{ 1 };	//最大次数
		FSCommand()
		{
			getResult = false;
		}
		bool WaitResult() {
			//cv.wait_for(locker,std::chrono::duration_cast<std::chrono::seconds>(5));
			return evt.WaitResult();
		}
		void RecvResult(const std::string& content)
		{
			nRecvTimes++;
			contents.push_back(content);
			//this->content = content;
			recvMsgs.push_back(content);
			if (IsFinished())
				evt.Notify();
		}
		virtual std::string GetContent()
		{
			if (nRecvTimes > 0)
				return contents.back();
			return "";
		}
		bool IsFinished() {
			return nRecvTimes == nMaxTimes;
		}
	};

	struct FSCommandList : FSCommand
	{
		int recvTimes{ 0 };
		//预计情况会有多次接受数据
		bool WaitResult() {
			//cv.wait_for(locker,std::chrono::duration_cast<std::chrono::seconds>(5));
			return evt.WaitResult();
		}
		void WaitResult(int ms) {
			//cv.wait_for(locker,std::chrono::duration_cast<std::chrono::seconds>(5));
			evt.WaitResultMillionSecond(ms);
		}
		void Notify()
		{
			evt.Notify();
		}
		void Reset() {
			recvTimes = 0;
			getResult = false;
			contents.clear();
			contents.shrink_to_fit();
			recvMsgs.clear();
			recvMsgs.shrink_to_fit();
		}
		virtual std::string GetContent()
		{
			std::string result;
			for (int i = 0; i < contents.size(); i++)
			{
				result += contents.back();
			}
			return result;
		}
	};
	struct ListFileInfo
	{
		char authority[10];
		char fileType;
		char author[255];
		int filesize;
		char date[12];
		char fileName[255];
		bool isFile;
		//Feb 18 21:06
		ListFileInfo(string strFileInfo)
		{
			//去除\r
			size_t n = strFileInfo.find_last_not_of('\r');
			if (n != std::string::npos)
			{
				strFileInfo.erase(n + 1, strFileInfo.size() - n);
			}

			char buff[2048];
			memset(this, 0, sizeof(ListFileInfo));
			stringstream ss(strFileInfo);
			ss.read(buff, 1);
			if (buff[0] == 'd')
			{
				isFile = false;
			}
			else {
				isFile = true;
			}
			ss >> authority;
			ss >> fileType;
			ss >> buff;	//组
			ss >> author;	//所有者
			ss >> filesize;
			ss.read(buff, 1);
			ss.read(date, 12);
			ss.read(buff, 1);
			ss.getline(fileName, 255);
		}
	};

	struct TransFileInfo
	{
		std::string fileName{ "" };
		int fileSize{ 0 };
		int curTransFileSize{ 0 };
		std::ofstream ofs;	
		std::ifstream ifs;
	};

	enum ServerMode
	{
		PasvMode,	//server connect to client
		PortMode	//client connect to server
	};

	class FTPClient : public IFtpClient
	{
	public:
		FTPClient() {
			dataServer.RecvHandler = std::bind(&FTPClient::OnRecvDataChannelPortMode, this,
				std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

			//dataClient.RecvHandler = std::bind(&FTPClient::OnRecvDataChannelPasvMode, this,
			//	std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

			cmdClient.RecvHandler = std::bind(&FTPClient::OnCmdClientRecv, this,
				std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

		}

		/**
		 * 登陆
		 */
		virtual bool Login(const wchar_t* addr, const wchar_t* usr, const wchar_t* pwd);
		/**
		 *服务器的被动模式，下载和上传文件需要被动模式 需要dataclient手动连接
		 */
		virtual bool Pasv(); ////
		/*
		* 切换模式 mode:0 TYPE I 
		*/
		virtual bool Type(int mode); ////
		/**
		 *列出文件列表 包括文件夹和文件 被动模式
		 */
		virtual const wchar_t* List(); //列出文件列表 包括文件夹和文件

		virtual void ListAllFileAndFolders(std::vector<FileInfo>& files);


		virtual int ListAllFileAndFolders() {
			if (listfileResult.size() > 0)
			{
				listfileResult.clear();
				listfileResult.shrink_to_fit();
			}
			ListAllFileAndFolders(listfileResult);
			return listfileResult.size();
		} //列出文件列表 包括文件夹和文件 返回文件数量
		virtual bool GetFileInfo(FileInfo& fi, int idx) {
			if (idx >= 0 && idx < listfileResult.size())
			{
				memcpy(&fi, &listfileResult[idx], sizeof(FileInfo));
				return true;
			}
			return false;
		} //获取需要列举的文件列表
		virtual void ClearFileList() {
			listfileResult.clear();
			listfileResult.shrink_to_fit();
		} //获取需要列举的文件列表



		/**
		 * 主动模式
		 */
		virtual void Port();
		virtual void Exit();	//退出
		/**
		 * 打印当前目录
		 */
		virtual	const wchar_t* Pwd();
		/**
		 * 改变服务器的工作目录
		 */
		virtual bool Cwd(const wchar_t* workDir);


		virtual bool Cdup();
		/**
		 * 创建目录
		 */
		virtual void MakeDiectory(const wchar_t* dir);
		/**
		 * 下载文件,服务器处于被动模式
		 */
		virtual void Retr(const wchar_t* serverFile, const wchar_t* dstFile, int fileSize, IFileTransferObserver* observer = nullptr);
		/**
		 * 上传文件 服务器处于被动模式
		 */
		 //virtual void Stor(string fileName, string destFileName, IFileTransferObserver* observer = nullptr);
		virtual void Stor(const wchar_t* localFile, const wchar_t* dstFileInServer, IFileTransferObserver* observer = nullptr);

		bool DeleteFile(const wchar_t* serverFile);
		bool RenameFile(const wchar_t* serverFile, const wchar_t* destFile);
		bool DeleteDirectory(const wchar_t* serverFile);
	private:
		char sendBuff[SendSize];
		char recvBuff[RecvSize];
		void OnRecvDataChannelPortMode(string, SOCKET, char*, int);
		void OnRecvDataChannelPasvMode(string, SOCKET, char*, int);
		void SendData(SOCKET);
		string GetPortCmd(int port)
		{
			int p1 = port / 256;
			int p2 = port % 256;
			// 13264 51 208 PORT  ip0,ip1,ip2,ip3,a,b
			// 端口计算是 4 * 256 +2 = 1026  a = 4 b = 2;
			int ip[4];
			GetIpData(ip, 4);
			char buff[1024];
			memset(buff, 0, 1024);
			sprintf(buff, "PORT %d,%d,%d,%d,%d,%d\r\n", ip[0], ip[1], ip[2], ip[3], p1, p2);
			return buff;
		}
		void ParseListAck(string msg);
		void GetFileListFromContent(const string& content, std::vector<FileInfo>& fileList);

	private:
		//only recv msg from ftp server
		void NewPasvConnect(std::function<void(string, SOCKET, char*, int)> recvFunc);
		void NewPortConnect(std::function<void(string, SOCKET, char*, int)> recvFunc);
		void RemovePasvConnect(int taskID);
		int NewCmdId() {
			int c = cmdId;
			cmdId++;
			return c;
		}
		int GetCmdId() { 
			return cmdId; }

		int cmdId{0};
		TcpClient cmdClient;
		//TcpClient dataClient;	//被动模式使用
		TcpServer dataServer;	//主动模式使用
		std::map<int,TcpClient> pasvConn;	//cmdID, tcpClient
		//std::deque<TcpServer> portConn;
		//key: taskId[pasvConn] value:FileInfo
		std::map<int, TransFileInfo> downloadFileTask;	//first is taskClient
		std::map<int, TransFileInfo> uploadFileTask;

		FSCommandList listCmd;
		//用于保存结果，每次调用玩需要清空
		std::vector<FileInfo> listfileResult;
		string curCmd;
		FTPSocket::FtpCmd curFtpCmd;
		string storFileName;
		ofstream ofs;
		void Enque(std::string type, FSCommand* cell);
		//return true if contains msgcode 
		bool ParseRecvInfo(std::string content, std::string& recvContnet);
		std::deque<FSCommand*>  callDeque;


		void OnCmdClientRecv(string, SOCKET s, char* buff, int size);

		ServerMode serverMode;

		GS g;
		//if pasvMode 就是服务器的ip和端口 否则就是
		string dataIp;
		int dataPort;
	};

}