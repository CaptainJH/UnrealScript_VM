#pragma once
#include <string>
#include <vector>


class FName
{
public:
	FName(int idx=0);
	std::string ToString();

private:
	int index;
	int number = 0;

public:
	static void LoadNames(const std::string& path);
	static std::vector<std::string> Names;
};