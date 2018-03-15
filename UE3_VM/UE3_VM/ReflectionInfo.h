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

	int PropertiesSize;
	std::vector<BYTE> ScriptStorage;
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

	int PropertySize;

	virtual void CopyCompleteValue(void* Dest, void* Src); 
	virtual void DestroyValue(void* Dest) const;

};

class UFunction : public UStruct
{
public:
	int NumParams;
	int ParamSize;
	int ReturnValueOffset;
	int BytecodeScriptSize;
};