#include "tool.h"

std::string SubString(std::string str,char start, char end)
{
	size_t posS = str.find_first_of("(");
	size_t posE = str.find_last_of(")");
	return  str.substr(posS + 1, posE - posS - 1);
}