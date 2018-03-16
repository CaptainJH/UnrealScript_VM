#pragma once
#include <vector>
#include "Types.h"
#include "UObject.h"

class UField : public UObject
{
public:

	// Variables.
	UField*			Next = nullptr;
};

class UStruct : public UField
{
public:

	INT GetPropertiesSize() const;
	INT GetMinAlignment() const;

	int PropertiesSize;
	int MinAlignment;
	std::vector<BYTE> ScriptStorage;
	std::vector<BYTE> Script;

	UField*				Children = nullptr;
};

class UClass : public UStruct
{
public:
};

class UFunction : public UStruct
{
public:
	int NumParams;
	int ParamSize;
	int ReturnValueOffset;
	int BytecodeScriptSize;
};

class UEnum : public UField
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
	virtual void CopySingleValue(void* Dest, void* Src, UObject* SubobjectRoot=NULL, UObject* DestOwnerObject=NULL/*, FObjectInstancingGraph* InstanceGraph=NULL*/) const;
	virtual void DestroyValue(void* Dest) const;

	INT GetSize() const;

};

class UIntProperty : public UProperty
{
public:

};

class UStrProperty : public UProperty
{
public:
};

class UBoolProperty : public UProperty
{
public:
	BITFIELD BitMask;
};