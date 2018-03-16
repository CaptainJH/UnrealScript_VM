#pragma once
#include <string>
#include "Types.h"


template< class T > inline T Clamp(const T X, const T Min, const T Max)
{
	return X<Min ? Min : X<Max ? X : Max;
}

template< class T > inline T Align(const T Ptr, INT Alignment)
{
	return (T)(((PTRINT)Ptr + Alignment - 1) & ~(Alignment - 1));
}


std::string StringSlice(std::string& str, char sep, size_t& begin);