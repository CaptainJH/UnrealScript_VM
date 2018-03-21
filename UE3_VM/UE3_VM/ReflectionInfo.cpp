#include "ReflectionInfo.h"

INT UStruct::GetPropertiesSize() const
{
	return PropertiesSize;
}
INT UStruct::GetMinAlignment() const
{
	return MinAlignment;
}

INT UClass::GetDefaultsCount()
{
	return GetPropertiesSize();
}

UClass::UClass(size_t InSize, size_t flag, size_t castFlag, const char* name)
{
	PropertiesSize = InSize;
	Name = name;
}

void UProperty::CopySingleValue(void* Dest, void* Src)
{
	memcpy(Dest, Src, ElementSize);
}

void UProperty::CopyCompleteValue(void* Dest, void* Src)
{
	for (auto i = 0; i < ArrayDim; ++i)
	{
		CopySingleValue((BYTE*)Dest + i * ElementSize, (BYTE*)Src + i * ElementSize);
	}
}

void UProperty::DestroyValue(void* dest) const
{

}

INT UProperty::GetSize() const
{
	return ArrayDim * ElementSize;
}

void UProperty::CopySingleValue(void* Dest, void* Src, UObject* SubobjectRoot/*=NULL*/, UObject* DestOwnerObject/*=NULL, FObjectInstancingGraph* InstanceGraph=NULL*/) const
{
	memcpy(Dest, Src, ElementSize);
}

bool UBoolProperty::Identical(const void* A, const void* B, DWORD PortFlags) const
{
	return ((*(BITFIELD*)A ^ (B ? *(BITFIELD*)B : 0)) & BitMask) == 0;
}

bool UIntProperty::Identical(const void* A, const void* B, DWORD PortFlags) const
{
	return *(INT*)A == (B ? *(INT*)B : 0);
}

bool UFloatProperty::Identical(const void* A, const void* B, DWORD PortFlags) const
{
	return *(FLOAT*)A == (B ? *(FLOAT*)B : 0);
}

bool UByteProperty::Identical(const void* A, const void* B, DWORD PortFlags) const
{
	return *(BYTE*)A == (B ? *(BYTE*)B : 0);
}