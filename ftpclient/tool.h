#pragma once
#include <string>
#include <locale.h>
std::string SubString(std::string str,char start, char end);

std::string GetLocalIP();

void GetIpData(int* data,int size);
bool IsDigital(std::string& str);

bool chars2wchars(const char* srcChar,wchar_t* dstChar,int size);
bool wchars2chars(const wchar_t* ch,  char* dstWChar, int size);

// wstring=&gt;string
std::string WString2String(const std::wstring& ws);

// string =&gt; wstring
std::wstring String2WString(const std::string& s);
