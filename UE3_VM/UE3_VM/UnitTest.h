#pragma once
#include <iostream>

template<class T>
void UnitTestFunction(const char* functionName, const T& expectedResult)
{
	std::cout << "=============== Begin Test: " << functionName << " ===============" << std::endl;
	auto id = ScriptRuntimeContext::Get()->FindIndex(std::string(functionName));
	auto result = ScriptRuntimeContext::Get()->RunFunction<T>(id);
	if (expectedResult == result)
	{
		std::cout << "PASS!\n";
	}
	else
	{
		std::cout << "FAILED!!!\n";
	}
	std::cout << "=============== End Test ===============\n" << std::endl;
}
