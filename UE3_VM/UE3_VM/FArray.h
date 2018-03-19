#pragma once
#include <vector>
#include "Types.h"

class FScriptArray
{
public:

	int Num();
	void* GetData();
	void InsertZeroed(INT Index, INT Count, INT NumBytesPerElement);
	void Insert(INT Index, INT Count, INT NumBytesPerElement);
	void Remove(INT Index, INT Count, INT NumBytesPerElement);
	INT AddZeroed(INT Count, INT NumBytesPerElement);
	INT Add(INT Count, INT NumBytesPerElement);

private:
	int ArrayNum = 0;
	std::vector<unsigned char> m_vector;
};