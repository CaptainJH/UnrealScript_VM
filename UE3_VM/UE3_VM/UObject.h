#pragma once
#include <string>
#include "Types.h"


struct FFrame;
class FName;
class UFunction;

class UObject
{
public:

	// UnrealScript intrinsics.
	#define DECLARE_FUNCTION(func) void func( FFrame& Stack, RESULT_DECL )
	DECLARE_FUNCTION(execUndefined);
	DECLARE_FUNCTION(execLocalVariable);
	DECLARE_FUNCTION(execInstanceVariable);
	//DECLARE_FUNCTION(execDefaultVariable);
	DECLARE_FUNCTION(execStateVariable);

	DECLARE_FUNCTION(execStop);
	DECLARE_FUNCTION(execCase);
	DECLARE_FUNCTION(execJump);
	DECLARE_FUNCTION(execSwitch);
	DECLARE_FUNCTION(execJumpIfNot);
	DECLARE_FUNCTION(execGotoLabel);
	DECLARE_FUNCTION(execEatReturnValue);
	DECLARE_FUNCTION(execReturnNothing);
	DECLARE_FUNCTION(execAssert);
	DECLARE_FUNCTION(execDynArrayElement);

	DECLARE_FUNCTION(execSelf);
	DECLARE_FUNCTION(execContext);

	DECLARE_FUNCTION(execArrayElement);

	DECLARE_FUNCTION(execVirtualFunction);
	DECLARE_FUNCTION(execFinalFunction);
	DECLARE_FUNCTION(execGlobalFunction);

	DECLARE_FUNCTION(execNothing);

	DECLARE_FUNCTION(execLet);

	DECLARE_FUNCTION(execEndFunctionParms);

	DECLARE_FUNCTION(execIntConstByte);
	DECLARE_FUNCTION(execIntConst);
	DECLARE_FUNCTION(execFloatConst);
	DECLARE_FUNCTION(execObjectConst);
	DECLARE_FUNCTION(execRotationConst);
	DECLARE_FUNCTION(execVectorConst);
	DECLARE_FUNCTION(execNativeParm);
	DECLARE_FUNCTION(execByteConst);
	DECLARE_FUNCTION(execIntZero);
	DECLARE_FUNCTION(execIntOne);
	DECLARE_FUNCTION(execTrue);
	DECLARE_FUNCTION(execFalse);
	DECLARE_FUNCTION(execNoObject);
	DECLARE_FUNCTION(execNameConst);

	DECLARE_FUNCTION(execBoolVariable);

	DECLARE_FUNCTION(execIterator);
	DECLARE_FUNCTION(execIteratorPop);

	DECLARE_FUNCTION(execStructCmpEq);
	DECLARE_FUNCTION(execStructCmpNe);
	DECLARE_FUNCTION(execStructMember);

	DECLARE_FUNCTION(execDynArrayLength);

	DECLARE_FUNCTION(execConditional);

	DECLARE_FUNCTION(execLocalOutVariable);

	DECLARE_FUNCTION(execDefaultParmValue);

	DECLARE_FUNCTION(execEmptyParmValue);

	DECLARE_FUNCTION(execEndOfScript);

	DECLARE_FUNCTION(execAdd_IntInt);
	DECLARE_FUNCTION(execGreater_IntInt);

	void CallFunction(FFrame& Stack, RESULT_DECL, UFunction* Function); 
	UFunction* FindFunction(FName& name);
	void ProcessInternal(FFrame& Stack, RESULT_DECL);

	std::string Name;

	/** Main script execution stack. */
	FFrame*					StateFrame;
	
};