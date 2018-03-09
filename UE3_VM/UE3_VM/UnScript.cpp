#include <array>
#include "UnScript.h"
#include "UObject.h"

extern std::array<Native, 1000> GNatives;
INT GNativeDuplicate = 0;

UProperty*				GProperty = NULL;
BYTE*					GPropAddr = NULL;						/* Property address for UnrealScript interpreter */
UObject*				GPropObject = NULL;

/*-----------------------------------------------------------------------------
Native registry.
-----------------------------------------------------------------------------*/

//
// Register a native function.
// Warning: Called at startup time, before engine initialization.
//
BYTE GRegisterNative(INT iNative, const Native& Func)
{
	static int Initialized = 0;
	if (!Initialized)
	{
		Initialized = 1;
		for (auto i = 0U; i< GNatives.size(); i++)
			GNatives[i] = &UObject::execUndefined;
	}
	if (iNative != INDEX_NONE)
	{
		if (iNative<0U || (unsigned int)iNative > GNatives.size() || GNatives[iNative] != &UObject::execUndefined)
			GNativeDuplicate = iNative;
		GNatives[iNative] = Func;
	}
	return 0;
}