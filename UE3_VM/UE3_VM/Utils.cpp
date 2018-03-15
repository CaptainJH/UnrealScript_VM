#include "Utils.h"

std::string StringSlice(std::string& str, char sep, size_t& begin)
{
	size_t end = begin;
	while (end != str.length())
	{
		if (str[end++] == sep)
		{
			std::string ret = str.substr(begin, end - begin - 1);
			begin = end;
			return ret;
		}
	}

	return str.substr(begin, end - begin);
}