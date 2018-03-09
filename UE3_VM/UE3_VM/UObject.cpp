#include "UObject.h"
#include "UnScript.h"
#include <assert.h>

extern UProperty*	GProperty;
extern BYTE*		GPropAddr;
extern UObject*		GPropObject;

//
// Registering a native function.
//
#define IMPLEMENT_FUNCTION(cls,num,func) \
	extern "C" { Native int##cls##func = (Native)&cls::func; } \
	static BYTE cls##func##Temp = GRegisterNative( num, int##cls##func );


//////////////////////////////
// Undefined native handler //
//////////////////////////////
void UObject::execUndefined(FFrame& Stack, RESULT_DECL)
{
	Stack.Log("Unknown code token");
}


/////////////
// Nothing //
/////////////

void UObject::execNothing(FFrame& Stack, RESULT_DECL)
{
	// Do nothing.
}
IMPLEMENT_FUNCTION(UObject, EX_Nothing, execNothing);


///////////////
// Variables //
///////////////

void UObject::execLocalVariable(FFrame& Stack, RESULT_DECL)
{
	assert(Stack.Object == this);
	assert(Stack.Locals != NULL);
	GProperty = (UProperty*)Stack.ReadObject();
	GPropAddr = Stack.Locals + GProperty->Offset;
	GPropObject = NULL;
	if (Result)
		GProperty->CopyCompleteValue(Result, GPropAddr);
}
IMPLEMENT_FUNCTION(UObject, EX_LocalVariable, execLocalVariable);


//////////////
// Commands //
//////////////

void UObject::execStop(FFrame& Stack, RESULT_DECL)
{
	Stack.Code = NULL;
}
IMPLEMENT_FUNCTION(UObject, EX_Stop, execStop);

//@warning: Does not support UProperty's fully, will break when TArray's are supported in UnrealScript!
void UObject::execSwitch(FFrame& Stack, RESULT_DECL)
{
	//// Get switch size.
	//UField* ExpressionField = NULL;
	//VariableSizeType bSize = Stack.ReadVariableSize(&ExpressionField);
	//if (bSize == 0)
	//{
	//	if (Cast<UIntProperty>(ExpressionField) != NULL)
	//	{
	//		// DynamicArray.Length
	//		bSize = sizeof(INT);
	//	}

	//	// add additional handling required for non-property expressions here
	//}

	//// Get switch expression.
	//BYTE SwitchBuffer[1024];
	//BYTE Buffer[1024];
	//memset(Buffer, 0, sizeof(1024));
	//memset(SwitchBuffer, 0, sizeof(1024));
	//Stack.Step(Stack.Object, SwitchBuffer);

	//UStrProperty* StringProp = Cast<UStrProperty>(ExpressionField);

	//// Check each case clause till we find a match.
	//for (; ; )
	//{
	//	// Skip over case token.
	//	assert(*(int*)Stack.Code[0] == EX_Case);
	//	Stack.Code++;

	//	// Get address of next handler.
	//	INT wNext = Stack.ReadWord();
	//	if (wNext == MAXWORD) // Default case or end of cases.
	//	{
	//		break;
	//	}

	//	// Get case expression.
	//	Stack.Step(Stack.Object, Buffer);

	//	// Compare.
	//	if (StringProp == NULL ? (memcmp(SwitchBuffer, Buffer, bSize) == 0) : (*(FString*)SwitchBuffer == *(FString*)Buffer))
	//	{
	//		break;
	//	}

	//	// Jump to next handler.
	//	Stack.Code = &Stack.Node->Script[wNext];
	//}
	//if (StringProp != NULL)
	//{
	//	(*(FString*)SwitchBuffer).~FString();
	//	(*(FString*)Buffer).~FString();
	//}
}
IMPLEMENT_FUNCTION(UObject, EX_Switch, execSwitch);

void UObject::execCase(FFrame& Stack, RESULT_DECL)
{
	INT wNext = Stack.ReadWord();
	//if (wNext != MAXWORD)
	{
		// Skip expression.
		BYTE Buffer[1024];
		memset(Buffer, 0, 1024);
		Stack.Step(Stack.Object, Buffer);
	}
}
IMPLEMENT_FUNCTION(UObject, EX_Case, execCase);

void UObject::execJump(FFrame& Stack, RESULT_DECL)
{
	// Jump immediate.
	INT Offset = Stack.ReadWord();
	Stack.Code = &Stack.Node->Script[Offset];
}
IMPLEMENT_FUNCTION(UObject, EX_Jump, execJump);

void UObject::execJumpIfNot(FFrame& Stack, RESULT_DECL)
{

	// Get code offset.
	INT Offset = Stack.ReadWord();

	// Get boolean test value.
	unsigned int Value = 0;
	Stack.Step(Stack.Object, &Value);

	// Jump if false.
	if (!Value)
	{
		Stack.Code = &Stack.Node->Script[Offset];
	}
}
IMPLEMENT_FUNCTION(UObject, EX_JumpIfNot, execJumpIfNot);


void UObject::execGotoLabel(FFrame& Stack, RESULT_DECL)
{
	//P_GET_NAME(N);
	//if (!GotoLabel(N))
	//{
	//	Stack.Log("GotoLabel (%s): Label not found");
	//}
}
IMPLEMENT_FUNCTION(UObject, EX_GotoLabel, execGotoLabel);

void UObject::execEatReturnValue(FFrame& Stack, RESULT_DECL)
{
	// get the return value property
	UProperty* Property = (UProperty*)Stack.ReadObject();
	// allocate a temporary buffer for the return value
	INT BufferSize = Property->ArrayDim * Property->ElementSize;
	BYTE* Buffer = (BYTE*)malloc(BufferSize);
	memset(Buffer, 0, BufferSize);
	// call the function, storing the unused return value in our temporary buffer
	Stack.Step(Stack.Object, Buffer);
	// destroy the return value
	Property->DestroyValue(Buffer);
}
IMPLEMENT_FUNCTION(UObject, EX_EatReturnValue, execEatReturnValue);


////////////////
// Assignment //
////////////////

void UObject::execLet(FFrame& Stack, RESULT_DECL)
{

	// Get variable address.
	GPropAddr = NULL;
	Stack.Step(Stack.Object, NULL); // Evaluate variable.
	if (!GPropAddr)
	{
		Stack.Log("Attempt to assign variable through None");
	}
	//else if (GPropObject && GProperty && (GProperty->PropertyFlags & CPF_Net))
	//{
	//	GPropObject->NetDirty(GProperty); //FIXME - use object property instead for performance
	//}

	//if (GRuntimeUCFlags & RUC_ArrayLengthSet)
	//{
	//	GRuntimeUCFlags &= ~RUC_ArrayLengthSet;
	//	FScriptArray* Array = (FScriptArray*)GPropAddr;
	//	UArrayProperty* ArrayProp = (UArrayProperty*)GProperty;
	//	INT NewSize = 0;
	//	Stack.Step(Stack.Object, &NewSize); // Evaluate expression into variable.
	//	if (NewSize > Array->Num())
	//	{
	//		INT OldSize = Array->Num();
	//		INT Count = NewSize - OldSize;
	//		Array->AddZeroed(Count, ArrayProp->Inner->ElementSize);

	//		// if this is an array of structs, and the struct has defaults, propagate those now
	//		UStructProperty* StructInner = Cast<UStructProperty>(ArrayProp->Inner, CLASS_IsAUStructProperty);
	//		if (StructInner && StructInner->Struct->GetDefaultsCount())
	//		{
	//			for (INT i = OldSize; i < NewSize; i++)
	//			{
	//				BYTE* Dest = (BYTE*)Array->GetData() + i * ArrayProp->Inner->ElementSize;
	//				StructInner->InitializeValue(Dest);
	//			}
	//		}
	//	}
	//	else if (NewSize < Array->Num())
	//	{
	//		for (INT i = Array->Num() - 1; i >= NewSize; i--)
	//			ArrayProp->Inner->DestroyValue((BYTE*)Array->GetData() + ArrayProp->Inner->ElementSize*i);
	//		Array->Remove(NewSize, Array->Num() - NewSize, ArrayProp->Inner->ElementSize);
	//	}
	//}
	//else
		Stack.Step(Stack.Object, GPropAddr); // Evaluate expression into variable.
}
IMPLEMENT_FUNCTION(UObject, EX_Let, execLet);

void UObject::execEndFunctionParms(FFrame& Stack, RESULT_DECL)
{
	// For skipping over optional function parms without values specified.
	GPropObject = NULL;
	Stack.Code--;
}
IMPLEMENT_FUNCTION(UObject, EX_EndFunctionParms, execEndFunctionParms);

/** failsafe for functions that return a value - returns the zero value for a property and logs that control reached the end of a non-void function */
void UObject::execReturnNothing(FFrame& Stack, RESULT_DECL)
{
	// send a warning to the log
	Stack.Log("Control reached the end of non-void function (make certain that all paths through the function 'return <value>'");

	// copy a zero value into Result, which is the return value data
	UProperty* ReturnProperty = (UProperty*)Stack.ReadObject();
	assert(ReturnProperty != NULL);
	// destroy old value if necessary
	//if (ReturnProperty->PropertyFlags & CPF_NeedCtorLink)
	//{
	//	ReturnProperty->DestroyValue(Result);
	//}
	memset(Result, 0, ReturnProperty->ArrayDim * ReturnProperty->ElementSize);
}
IMPLEMENT_FUNCTION(UObject, EX_ReturnNothing, execReturnNothing);

void UObject::execIntConstByte(FFrame& Stack, RESULT_DECL)
{
	*(INT*)Result = *Stack.Code++;
}
IMPLEMENT_FUNCTION(UObject, EX_IntConstByte, execIntConstByte);


void UObject::execByteConst(FFrame& Stack, RESULT_DECL)
{
	*(BYTE*)Result = *Stack.Code++;
}
IMPLEMENT_FUNCTION(UObject, EX_ByteConst, execByteConst);

void UObject::execIntZero(FFrame& Stack, RESULT_DECL)
{
	*(INT*)Result = 0;
}
IMPLEMENT_FUNCTION(UObject, EX_IntZero, execIntZero);

void UObject::execIntOne(FFrame& Stack, RESULT_DECL)
{
	*(INT*)Result = 1;
}
IMPLEMENT_FUNCTION(UObject, EX_IntOne, execIntOne);

void UObject::execTrue(FFrame& Stack, RESULT_DECL)
{
	*(INT*)Result = 1;
}
IMPLEMENT_FUNCTION(UObject, EX_True, execTrue);

void UObject::execFalse(FFrame& Stack, RESULT_DECL)
{
	*(INT*)Result = 0;
}
IMPLEMENT_FUNCTION(UObject, EX_False, execFalse);

void UObject::execNoObject(FFrame& Stack, RESULT_DECL)
{
	*(UObject**)Result = NULL;
}
IMPLEMENT_FUNCTION(UObject, EX_NoObject, execNoObject);