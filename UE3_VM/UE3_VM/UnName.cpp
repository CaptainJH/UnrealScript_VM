#include <fstream>
#include "UnName.h"
#include "Utils.h"


FName::FName(int idx)
	: index(idx)
{}

std::string FName::ToString()
{
	return Names[index];
}

std::vector<std::string> FName::Names;

void FName::LoadNames(const std::string& path)
{
	const char sep = ':';

	std::ifstream ifile(path);
	std::string line;
	while (std::getline(ifile, line))
	{
		size_t begin = 0;
		auto str0 = StringSlice(line, sep, begin);
		auto str1 = StringSlice(line, sep, begin);

		Names.push_back(str1);
	}
}