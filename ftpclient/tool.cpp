#include "tool.h"
#include <iostream>
#include <winsock2.h>
using namespace std;
std::string SubString(std::string str, char start, char end)
{
	size_t posS = str.find_first_of(start);
	size_t posE = str.find_last_of(end);
	return  str.substr(posS + 1, posE - posS - 1);
}

std::string GetLocalIP()
{
	WORD winsock_version = MAKEWORD(2, 2);
	WSADATA wsa_data;
	if (WSAStartup(winsock_version, &wsa_data) != 0) {
		std::cout << "Failed to init socket dll!" << std::endl;
	}
	std::string localIP;
	char szHostName[MAX_PATH] = { 0 };
	::gethostname(szHostName, MAX_PATH);
	hostent* pHost = gethostbyname(szHostName);
	if (pHost != NULL)
	{
		char chIP[1024];
		memset(chIP, 0, 1024);
		in_addr* addr = (in_addr*)*pHost->h_addr_list;
		sprintf(chIP, "%s", inet_ntoa(addr[0]));
		localIP = chIP;
	}
	else {

	}

	return localIP;
}

void GetIpData(int* data, int size)
{
	std::string ip = GetLocalIP();

	size_t right = ip.find('.', 0);
	int left = 0;
	string ipdata[4];
	int idx = 0;
	while (right != std::string::npos || left < ip.size())
	{
		if (right == std::string::npos)
		{
			right = ip.size();
		}
		ipdata[idx] = ip.substr(left, right - left);
		data[idx] = atoi(ipdata[idx].data());
		idx++;
		left = right + 1;
		right = ip.find('.', left);
	}
}

bool IsDigital(string& str)
{
	auto it = str.begin();
	while (it != str.end() && std::isdigit(*it)) {
		it++;
	}
	return !str.empty() && it == str.end();
}


bool chars2wchars(const char* srcChar, wchar_t* dstChar, int size)
{
	std::string strLocale = setlocale(LC_ALL, "");
	size_t nDestSize = mbstowcs(NULL, srcChar, 0) + 1;
	if (nDestSize > size)
	{
		printf("chars2wchars not enough space,required %d bytes,buf in fact %d bytes.",nDestSize,size);
		return false;
	}

	//wchar_t* wchDest = new wchar_t[nDestSize];
	wmemset(dstChar, 0, size);
	mbstowcs(dstChar, srcChar, nDestSize);
	/*std::wstring wstrResult = wchDest;
	delete[] wchDest;*/
	setlocale(LC_ALL, strLocale.c_str());
	return true;
}
bool wchars2chars(const wchar_t* wchSrc, char* dstChar, int size)
{
	std::string strLocale = setlocale(LC_ALL, "");
	size_t nDestSize = wcstombs(NULL, wchSrc, 0) + 1;
	if (nDestSize > size)
	{
		printf("wchars2chars not enough space,required %d bytes,buf in fact %d bytes.", nDestSize, size);
		return false;
	}

	//char* chDest = new char[nDestSize];
	memset(dstChar, 0, size);
	wcstombs(dstChar, wchSrc, nDestSize);
	/*std::string strResult = chDest;
	delete[] chDest;*/
	setlocale(LC_ALL, strLocale.c_str());
	return true;
}


std::string WString2String(const std::wstring& ws)
{
	std::string strLocale = setlocale(LC_ALL, "");
	const wchar_t* wchSrc = ws.c_str();
	size_t nDestSize = wcstombs(NULL, wchSrc, 0) + 1;
	char* chDest = new char[nDestSize];
	memset(chDest, 0, nDestSize);
	wcstombs(chDest, wchSrc, nDestSize);
	std::string strResult = chDest;
	delete[] chDest;
	setlocale(LC_ALL, strLocale.c_str());
	return strResult;

}

std::wstring String2WString(const std::string& s)
{
	std::string strLocale = setlocale(LC_ALL, "");
	const char* chSrc = s.c_str();
	size_t nDestSize = mbstowcs(NULL, chSrc, 0) + 1;
	wchar_t* wchDest = new wchar_t[nDestSize];
	wmemset(wchDest, 0, nDestSize);
	mbstowcs(wchDest, chSrc, nDestSize);
	std::wstring wstrResult = wchDest;
	delete[] wchDest;
	setlocale(LC_ALL, strLocale.c_str());
	return wstrResult;

}

// wstring=&gt;string
//std::string WString2String(const std::wstring&  ws)
//{
//	
//}
//
//
//std::wstring String2WString(const std::string& s)
//{
//	
//
//}
