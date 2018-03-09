#pragma once
#include <vector>
#include "Types.h"
#include "UObject.h"

class UField : public UObject
{

};

class UStruct : UField
{
public:

	std::vector<BYTE> Script;
};

class UProperty : UField
{
public:

	int Offset;
	INT ArrayDim;
	INT ElementSize;

	virtual void CopyCompleteValue(void* Dest, void* Src); 
	virtual void DestroyValue(void* Dest) const;

};