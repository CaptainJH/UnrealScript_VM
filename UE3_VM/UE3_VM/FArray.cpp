#include "FArray.h"

FString::FString()
{
	ArrayMax = 0;
	ArrayNum = 0;
	Data = nullptr;
}

FString::FString(const char* In)
{
	ArrayNum = strlen(In) + 1;
	ArrayMax = ArrayNum;
	ResizeAllocation(0, ArrayMax, sizeof(char));
	memcpy(GetData(), In, ArrayNum * sizeof(char));
}

FString::FString(const TCHAR* In)
{
	ArrayNum = wcslen(In) + 1;
	ArrayMax = ArrayNum;
	ResizeAllocation(0, ArrayMax, sizeof(TCHAR));
	memcpy(GetData(), In, ArrayNum * sizeof(TCHAR));
}

std::string FString::ToStdString() const
{
	std::string result(Data);
	return result;
}