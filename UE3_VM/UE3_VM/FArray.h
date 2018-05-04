#pragma once
#include <vector>
#include <assert.h>
#include "Types.h"

template<class T>
class TArray
{
public:

	int Num()
	{
		return ArrayNum;
	}

	void* GetData()
	{
		return Data;
	}

	void InsertZeroed(INT Index, INT Count, INT NumBytesPerElement)
	{
		Insert(Index, Count, NumBytesPerElement);
		memset((BYTE*)GetData() + Index*NumBytesPerElement, 0, Count*NumBytesPerElement);
	}

	void Insert(INT Index, INT Count, INT NumBytesPerElement)
	{
		const INT OldNum = ArrayNum;
		ArrayNum += Count;
		ArrayMax = ArrayNum;
		
		ResizeAllocation(OldNum, ArrayMax, NumBytesPerElement);
		
		memmove
		(
			(BYTE*)GetData() + (Index + Count)*NumBytesPerElement,
			(BYTE*)GetData() + (Index)*NumBytesPerElement,
			(OldNum - Index)*NumBytesPerElement
		);
	}

	void Remove(INT Index, INT Count, INT NumBytesPerElement)
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
		ArrayMax = ArrayNum;
		ResizeAllocation(ArrayNum, ArrayMax, NumBytesPerElement);
		assert(ArrayNum >= 0);
	}

	INT AddZeroed(INT Count, INT NumBytesPerElement)
	{
		const INT Index = Add(Count, NumBytesPerElement);
		memset((BYTE*)GetData() + Index*NumBytesPerElement, 0, Count*NumBytesPerElement);
		return Index;
	}

	INT Add(INT Count, INT NumBytesPerElement)
	{
		assert(Count >= 0);
		assert(ArrayNum >= 0);
		
		const INT OldNum = ArrayNum;
		ArrayNum += Count;
		ArrayMax = ArrayNum;
		
		ResizeAllocation(OldNum, ArrayMax, NumBytesPerElement);
		
		return OldNum;
	}

	void ResizeAllocation(INT PreviousNumElements, INT NumElements, INT NumBytesPerElement)
	{
		if (Data || NumElements)
		{
			Data = (T*)realloc(Data, NumElements * NumBytesPerElement);
		}
	}

	bool operator==(const TArray<T>& rhs) const
	{
		if (ArrayMax != rhs.ArrayMax)
			return false;
		if (ArrayNum != rhs.ArrayNum)
			return false;

		return !memcmp(Data, rhs.Data, ArrayNum);
	}

protected:
	INT ArrayNum = 0;
	INT ArrayMax = 0;
	T*	Data = nullptr;
};

typedef TArray<BYTE> FScriptArray;

class FString : public TArray<char>
{
public:
	FString();
	FString(const char* In);
	FString(const TCHAR* In);

	std::string ToStdString() const;
};