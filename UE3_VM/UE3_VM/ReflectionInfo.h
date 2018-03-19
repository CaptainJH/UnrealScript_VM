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

	INT GetDefaultsCount();
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

	virtual bool Identical(const void* A, const void* B, DWORD PortFlags = 0) const { return false; }

};

class UIntProperty : public UProperty
{
public:
	bool Identical(const void* A, const void* B, DWORD PortFlags = 0) const override;
};

class UStrProperty : public UProperty
{
public:
};

class UBoolProperty : public UProperty
{
public:
	bool Identical(const void* A, const void* B, DWORD PortFlags = 0) const override;

	BITFIELD BitMask;
};

class UFloatProperty : public UProperty
{
public:
	bool Identical(const void* A, const void* B, DWORD PortFlags = 0) const override;
};

class UByteProperty : public UProperty
{
public:
	bool Identical(const void* A, const void* B, DWORD PortFlags = 0) const override;
};

class UArrayProperty : public UProperty
{
public:
	UProperty* Inner;
};

class UStructProperty : public UProperty
{
public:

};