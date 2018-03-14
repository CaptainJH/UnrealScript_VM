#pragma once
#include <vector>
#include "Types.h"
#include "UObject.h"

class UField : public UObject
{

};

class UStruct : public UField
{
public:

	std::vector<BYTE> Script;
};

class UClass : public UStruct
{
public:
};

class UProperty : public UField
{
public:

	int Offset;
	INT ArrayDim;
	INT ElementSize;

	virtual void CopyCompleteValue(void* Dest, void* Src); 
	virtual void DestroyValue(void* Dest) const;

};