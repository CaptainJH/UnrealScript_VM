#include "UStruct.h"

void UProperty::CopyCompleteValue(void* Dest, void* Src)
{
	*(INT*)Dest = *(INT*)Src;
}

void UProperty::DestroyValue(void* dest) const
{

}