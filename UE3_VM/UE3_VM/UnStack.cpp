#include <iostream>
#include <array>
#include <assert.h>
#include "UnStack.h"
#include "UObject.h"
#include "UnScript.h"

std::array<Native, 1000> GNatives;

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

void FFrame::Log(const std::string& str) const
{
	std::cout << str << std::endl;
}

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
	INT Result;
//#ifdef REQUIRES_ALIGNED_INT_ACCESS
//	WORD Temporary;
//	appMemcpy(&Temporary, Code, sizeof(WORD));
//	Result = Temporary;
//#else
	Result = *(WORD*)Code;
//#endif
	Code += sizeof(WORD);
	return Result;
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

VariableSizeType FFrame::ReadVariableSize(UField** ExpressionField/*=NULL*/)
{
	VariableSizeType Result = 0;

	UObject* Field = ReadObject();
	BYTE NullPropertyType = *Code++;

	if (Field != NULL)
	{
		UProperty* Property = static_cast<UProperty*>(Field);
		if (Property != NULL)
		{
			Result = Property->GetSize();
		}
		else
		{
			UEnum* ExplicitEnumValue = static_cast<UEnum*>(Field);
			if (ExplicitEnumValue != NULL)
			{
				Result = 1;
			}
			else
			{
				UFunction* FunctionRef = static_cast<UFunction*>(Field);
				if (FunctionRef != NULL)
				{
					Result = sizeof(ScriptPointerType);
				}
			}
		}
	}
	else
	{
		switch (NullPropertyType)
		{
		case CPT_None:
			// nothing...
			break;
		case CPT_Byte:		Result = sizeof(BYTE);
			break;
		case CPT_Int:		Result = sizeof(INT);
			break;
		case CPT_Bool:		Result = sizeof(UBOOL);
			break;
		case CPT_Float:		Result = sizeof(FLOAT);
			break;
		//case CPT_Name:		Result = sizeof(FName);
		//	break;
		//case CPT_Vector:	Result = sizeof(FVector);
		//	break;
		//case CPT_Rotation:	Result = sizeof(FRotator);
		//	break;
		//case CPT_Delegate:	Result = sizeof(FScriptDelegate);
		//	break;
		default:
			assert(false);
			//appErrorf(TEXT("Unhandled property type in FFrame::ReadVariableSize(): %u"), NullPropertyType);
			break;
		}
	}

	if (ExpressionField != NULL)
	{
		*ExpressionField = static_cast<UField*>(Field);
	}

	return Result;
}

CodeSkipSizeType FFrame::ReadCodeSkipCount()
{
	CodeSkipSizeType Result;

//#ifdef REQUIRES_ALIGNED_INT_ACCESS
//	appMemcpy(&Result, Code, sizeof(CodeSkipSizeType));
//#else
	Result = *(CodeSkipSizeType*)Code;
//#endif

	Code += sizeof(CodeSkipSizeType);
	return Result;
}

FName FFrame::ReadName()
{
	FName Result;
//#ifdef REQUIRES_ALIGNED_ACCESS
//	appMemcpy(&Result, Code, sizeof(FName));
//#else
	Result = *(FName*)Code;
//#endif
	Code += sizeof(FName);
	return Result;
}