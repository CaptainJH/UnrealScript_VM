#pragma once
#include "UStruct.h"

//
// UnrealScript intrinsic return value declaration.
//
#define RESULT_DECL void*const Result
typedef long long ScriptPointerType;


//
// Information about script execution at one stack level.
//
struct FFrame
{
	// Variables.
	UStruct*	Node;
	UObject*	Object;
	BYTE*		Code;
	BYTE*		Locals;

	/** Previous frame on the stack */
	FFrame* PreviousFrame;
	/** contains information on any out parameters */
	//FOutParmRec* OutParms;

	// Constructors.
	FFrame(UObject* InObject);
	FFrame(UObject* InObject, UStruct* InNode, INT CodeOffset, void* InLocals, FFrame* InPreviousFrame = nullptr);

	virtual ~FFrame()
	{}

	// Functions.
	void Step(UObject* Context, RESULT_DECL);
	//void Serialize(const TCHAR* V, enum EName Event);

	INT ReadInt();
	FLOAT ReadFloat();
	//FName ReadName();
	UObject* ReadObject();
	INT ReadWord();

	/**
	* Reads a value from the bytestream, which represents the number of bytes to advance
	* the code pointer for certain expressions.
	*
	* @param	ExpressionField		receives a pointer to the field representing the expression; used by various execs
	*								to drive VM logic
	*/
	//CodeSkipSizeType ReadCodeSkipCount();

	/**
	* Reads a value from the bytestream which represents the number of bytes that should be zero'd out if a NULL context
	* is encountered
	*
	* @param	ExpressionField		receives a pointer to the field representing the expression; used by various execs
	*								to drive VM logic
	*/
	//VariableSizeType ReadVariableSize(UField** ExpressionField = NULL);

	/**
	* This will return the StackTrace of the current callstack from .uc land
	**/
	//FString GetStackTrace() const;
};

/** The type of a native function callable by script */
typedef void (UObject::*Native)(FFrame& TheStack, RESULT_DECL);

Native GNatives[1000];