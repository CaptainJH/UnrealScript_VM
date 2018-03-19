#include "FArray.h"
#include <assert.h>

int FScriptArray::Num()
{
	return ArrayNum;
}

void* FScriptArray::GetData()
{
	return &m_vector[0];
}

void FScriptArray::InsertZeroed(INT Index, INT Count, INT NumBytesPerElement)
{
	Insert(Index, Count, NumBytesPerElement);
	memset((BYTE*)GetData() + Index*NumBytesPerElement, 0, Count*NumBytesPerElement);
}

void FScriptArray::Insert(INT Index, INT Count, INT NumBytesPerElement)
{
	const INT OldNum = ArrayNum;
	ArrayNum += Count;

	std::vector<unsigned char> newVector(ArrayNum * NumBytesPerElement);
	memcpy(&newVector[0], &m_vector[0], m_vector.size());

	memmove
	(
		(BYTE*)GetData() + (Index + Count)*NumBytesPerElement,
		(BYTE*)GetData() + (Index)*NumBytesPerElement,
		(OldNum - Index)*NumBytesPerElement
	);

	m_vector = newVector;
}

void FScriptArray::Remove(INT Index, INT Count, INT NumBytesPerElement)
{
	assert(Count >= 0);
	assert(Index >= 0);
	assert(Index <= ArrayNum);
	assert(Index + Count <= ArrayNum);

	// Skip memmove in the common case that there is nothing to move.
	INT NumToMove = ArrayNum - Index - Count;
	if (NumToMove)
	{
		memmove
		(
			(BYTE*)GetData() + (Index)* NumBytesPerElement,
			(BYTE*)GetData() + (Index + Count) * NumBytesPerElement,
			NumToMove * NumBytesPerElement
		);
	}
	ArrayNum -= Count;

	std::vector<unsigned char> newVector(ArrayNum * NumBytesPerElement);
	memcpy(&newVector[0], GetData(), newVector.size());
	m_vector = newVector;
	assert(ArrayNum >= 0);
}

INT FScriptArray::AddZeroed(INT Count, INT NumBytesPerElement)
{
	const INT Index = Add(Count, NumBytesPerElement);
	memset((BYTE*)GetData() + Index*NumBytesPerElement, 0, Count*NumBytesPerElement);
	return Index;
}

INT FScriptArray::Add(INT Count, INT NumBytesPerElement)
{
	assert(Count >= 0);
	assert(ArrayNum >= 0);

	const INT OldNum = ArrayNum;
	ArrayNum += Count;
	std::vector<unsigned char> newVector(ArrayNum * NumBytesPerElement);
	memcpy(&newVector[0], GetData(), newVector.size());
	m_vector = newVector;

	return OldNum;
}