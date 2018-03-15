#pragma once
#include <string>
#include "Types.h"


struct FFrame;

class UObject
{
public:

	// UnrealScript intrinsics.
	#define DECLARE_FUNCTION(func) void func( FFrame& Stack, RESULT_DECL )
	DECLARE_FUNCTION(execUndefined);
	DECLARE_FUNCTION(execLocalVariable);

	DECLARE_FUNCTION(execStop);
	DECLARE_FUNCTION(execCase);
	DECLARE_FUNCTION(execJump);
	DECLARE_FUNCTION(execSwitch);
	DECLARE_FUNCTION(execJumpIfNot);
	DECLARE_FUNCTION(execGotoLabel);
	DECLARE_FUNCTION(execEatReturnValue);
	DECLARE_FUNCTION(execReturnNothing);

	DECLARE_FUNCTION(execNothing);

	DECLARE_FUNCTION(execLet);

	DECLARE_FUNCTION(execEndFunctionParms);

	DECLARE_FUNCTION(execIntConstByte);
	DECLARE_FUNCTION(execByteConst);
	DECLARE_FUNCTION(execIntZero);
	DECLARE_FUNCTION(execIntOne);
	DECLARE_FUNCTION(execTrue);
	DECLARE_FUNCTION(execFalse);
	DECLARE_FUNCTION(execNoObject);

	DECLARE_FUNCTION(execAdd_IntInt);

	std::string Name;
	
};