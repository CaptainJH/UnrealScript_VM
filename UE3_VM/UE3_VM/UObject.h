#pragma once
#include <string>
#include "Types.h"


struct FFrame;
class FName;
class UFunction;

class UObject
{
public:
	virtual ~UObject() {}

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
	DECLARE_FUNCTION(execDynArrayInsert);
	DECLARE_FUNCTION(execDynArrayRemove);
	DECLARE_FUNCTION(execDynArrayFind);
	DECLARE_FUNCTION(execDynArrayAdd);
	DECLARE_FUNCTION(execDynArrayIterator);

	DECLARE_FUNCTION(execSelf);
	DECLARE_FUNCTION(execContext);

	DECLARE_FUNCTION(execArrayElement);

	DECLARE_FUNCTION(execVirtualFunction);
	DECLARE_FUNCTION(execFinalFunction);
	DECLARE_FUNCTION(execGlobalFunction);

	DECLARE_FUNCTION(execNothing);

	DECLARE_FUNCTION(execLet);
	DECLARE_FUNCTION(execLetBool);

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
	DECLARE_FUNCTION(execStringConst);

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

	DECLARE_FUNCTION(execMultiply_IntInt);
	DECLARE_FUNCTION(execDivide_IntInt);
	DECLARE_FUNCTION(execPercent_IntInt);
	DECLARE_FUNCTION(execAdd_IntInt);
	DECLARE_FUNCTION(execSubtract_IntInt);
	DECLARE_FUNCTION(execLessLess_IntInt);
	DECLARE_FUNCTION(execGreaterGreater_IntInt);
	DECLARE_FUNCTION(execGreaterGreaterGreater_IntInt);
	DECLARE_FUNCTION(execLess_IntInt);
	DECLARE_FUNCTION(execGreater_IntInt);
	DECLARE_FUNCTION(execLessEqual_IntInt);
	DECLARE_FUNCTION(execGreaterEqual_IntInt);
	DECLARE_FUNCTION(execEqualEqual_IntInt);
	DECLARE_FUNCTION(execNotEqual_IntInt);
	DECLARE_FUNCTION(execAnd_IntInt);
	DECLARE_FUNCTION(execXor_IntInt);
	DECLARE_FUNCTION(execOr_IntInt);
	//DECLARE_FUNCTION(execMultiplyEqual_IntFloat);
	//DECLARE_FUNCTION(execDivideEqual_IntFloat);
	DECLARE_FUNCTION(execAddEqual_IntInt);
	DECLARE_FUNCTION(execSubtractEqual_IntInt);
	DECLARE_FUNCTION(execAddAdd_PreInt);

	void CallFunction(FFrame& Stack, RESULT_DECL, UFunction* Function); 
	UFunction* FindFunction(FName& name);
	void ProcessInternal(FFrame& Stack, RESULT_DECL);

	std::string Name;

	/** Main script execution stack. */
	FFrame*					StateFrame;
	
};