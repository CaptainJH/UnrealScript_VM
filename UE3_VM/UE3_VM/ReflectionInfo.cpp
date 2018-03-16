#include "ReflectionInfo.h"

INT UStruct::GetPropertiesSize() const
{
	return PropertiesSize;
}
INT UStruct::GetMinAlignment() const
{
	return MinAlignment;
}

void UProperty::CopyCompleteValue(void* Dest, void* Src)
{
	*(INT*)Dest = *(INT*)Src;
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