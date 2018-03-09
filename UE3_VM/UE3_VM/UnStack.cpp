#include "UnStack.h"

FFrame::FFrame(UObject* InObject)
	: Node(nullptr)
	, Object(nullptr)
	, Code(nullptr)
	, Locals(nullptr)
	, PreviousFrame(nullptr)
{}

FFrame::FFrame(UObject* InObject, UStruct* InNode, INT CodeOffset, void* InLocals, FFrame* InPreviousFrame)
	: Node(InNode)
	, Object(InObject)
	, Code(&InNode->Script[0])
	, Locals((BYTE*)InLocals)
	, PreviousFrame(InPreviousFrame)
{}

INT FFrame::ReadInt()
{
	INT Result;
	Result = *(INT*)Code;

	Code += sizeof(INT);
	return Result;
}

FLOAT FFrame::ReadFloat()
{
	FLOAT Result;
	Result = *(FLOAT*)Code;

	Code += sizeof(FLOAT);
	return Result;
}

INT FFrame::ReadWord()
{
	return ReadInt();
}

UObject* FFrame::ReadObject()
{
	ScriptPointerType TempCode;
	TempCode = *(ScriptPointerType*)Code;

	UObject* Result = (UObject*)TempCode;
	Code += sizeof(ScriptPointerType);
	return Result;
}

void FFrame::Step(UObject* Context, RESULT_DECL)
{
	INT B = *Code++;
	(Context->*GNatives[B])(*this, Result);
}