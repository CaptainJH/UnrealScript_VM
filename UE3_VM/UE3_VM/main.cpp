#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <string>

#include "UnStack.h"
#include "UnScript.h"

std::vector<BYTE> ReadByteCode(const std::string& path, const std::string& symbol)
{
	std::vector<BYTE> ret;

	std::ifstream ifile(path);
	std::string line;
	while (std::getline(ifile, line))
	{
		if (line.find(symbol) == symbol.npos)
		{
			continue;
		}

		// get next line
		std::getline(ifile, line);
		//std::cout << line << std::endl;

		size_t begin = 0;
		size_t end = 0;
		while (end != line.length())
		{
			if (line[end++] == ' ')
			{
				std::string str = line.substr(begin, end);
				ret.push_back((BYTE)strtol(str.c_str(), NULL, 16));

				begin = end;
			}
		}
	}

	return ret;
}


int main()
{
	std::cout << "==============UE3_VM==============" << std::endl;

	auto byteCode = ReadByteCode("D:\\Repo\\UnrealScript_VM\\UnrealScript_Wiki\\bytecode\\CompiledCode.txt", "HelloJHQ0");

	UStruct ustruct;
	ustruct.Script = byteCode;
	UObject uobject;
	void* temp = malloc(128);
	FFrame stack(&uobject, &ustruct, 0, temp);

	std::array<int, 1024> Buffer = { 0 };
	while (*stack.Code != EX_Return)
	{
		stack.Step(stack.Object, &Buffer[0]);
	}
	++stack.Code;
}