#include "UObject.h"
#include "UnScript.h"
#include "Utils.h"
#include "UnName.h"
#include "FArray.h"
#include "ReflectionInfo.h"
#include <assert.h>
#include <iostream>
#include <array>

extern UProperty*	GProperty;
extern BYTE*		GPropAddr;
extern UObject*		GPropObject;
extern std::array<Native, 1000> GNatives;
extern std::array<Native, 1000> GCasts;

void InitializePrivateStaticClass(class UClass* TClass_Super_StaticClass, class UClass* TClass_PrivateStaticClass/*, class UClass* TClass_WithinClass_StaticClass*/)
{
	/* No recursive ::StaticClass calls allowed. Setup extras. */
	if (TClass_Super_StaticClass != TClass_PrivateStaticClass)
	{
		TClass_PrivateStaticClass->SuperStruct = TClass_Super_StaticClass;
	}
	else
	{
		TClass_PrivateStaticClass->SuperStruct = nullptr;
	}
	//TClass_PrivateStaticClass->ClassWithin = TClass_WithinClass_StaticClass;
	//TClass_PrivateStaticClass->SetClass(UClass::StaticClass());

	/* Perform UObject native registration. */
	//if (TClass_PrivateStaticClass->GetInitialized() /*&& TClass_PrivateStaticClass->GetClass() == TClass_PrivateStaticClass->StaticClass()*/)
	{
		TClass_PrivateStaticClass->Register();
	}
}

IMPLEMENT_CLASS(UObject)

std::vector<UClass*> UObject::GObjObjects;
void UObject::Register()
{

}

//
// Registering a native function.
//
#define IMPLEMENT_FUNCTION(cls,num,func) \
	extern "C" { Native int##cls##func = (Native)&cls::func; } \
	static BYTE cls##func##Temp = GRegisterNative( num, int##cls##func );

#define IMPLEMENT_CAST_FUNCTION(cls,num,func) \
		IMPLEMENT_FUNCTION(cls,-1,func); \
		static BYTE cls##func##CastTemp = GRegisterCast( num, int##cls##func );

#define INIT_OPTX_EVAL GRuntimeUCFlags&=~RUC_SkippedOptionalParm;

#define P_GET_INT(var)                     INT   var=0;												Stack.Step( Stack.Object, &var    );
#define P_GET_NAME(var)                    FName var(-1);											Stack.Step( Stack.Object, &var    );
#define P_FINISH                           Stack.Code++;
#define P_GET_INT_REF(var)                 INT   var##T=0; GPropAddr=0;								Stack.Step( Stack.Object, &var##T ); INT*     p##var = (INT    *)GPropAddr; INT&     var = GPropAddr ? *(INT    *)GPropAddr:var##T;

#define P_GET_DELEGATE(var)                FScriptDelegate var(EC_EventParm);						Stack.Step( Stack.Object, &var    );
#define P_GET_DELEGATE_OPTX(var,def)       FScriptDelegate var=def;                 INIT_OPTX_EVAL	Stack.Step( Stack.Object, &var    );
#define P_GET_DELEGATE_REF(var)            FScriptDelegate var##T(EC_EventParm); GPropAddr=0;		Stack.Step( Stack.Object, &var##T ); FScriptDelegate* p##var = (FScriptDelegate*)GPropAddr; FScriptDelegate& var = GPropAddr ? *(FScriptDelegate*)GPropAddr:var##T;
#define P_GET_DELEGATE_OPTX_REF(var,def)   FScriptDelegate var##T=def; GPropAddr=0; INIT_OPTX_EVAL	Stack.Step( Stack.Object, &var##T ); FScriptDelegate* p##var = (FScriptDelegate*)GPropAddr; FScriptDelegate& var = GPropAddr ? *(FScriptDelegate*)GPropAddr:var##T;

//
// Iterator macros.
//
#define PRE_ITERATOR \
	INT wEndOffset = Stack.ReadWord(); \
	BYTE B=0; \
	DWORD Buffer[16]; \
	BYTE *StartCode = Stack.Code; \
	do {
#define POST_ITERATOR \
		while( (B=*Stack.Code)!=EX_IteratorPop && B!=EX_IteratorNext ) \
			Stack.Step( Stack.Object, Buffer ); \
		if( *Stack.Code++==EX_IteratorNext ) \
			Stack.Code = StartCode; \
	} while( B != EX_IteratorPop );

// Exits an iterator within PRE/POST_ITERATOR
#define EXIT_ITERATOR \
	Stack.Code = &Stack.Node->Script[wEndOffset + 1];

// Skips iterator execution, without PRE/POST_ITERATOR
#define SKIP_ITERATOR \
	INT wEndOffset = Stack.ReadWord(); \
	Stack.Code = &Stack.Node->Script(wEndOffset + 1);

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

void UObject::execInstanceVariable(FFrame& Stack, RESULT_DECL)
{
	GProperty = (UProperty*)Stack.ReadObject();
	GPropAddr = (BYTE*)this + GProperty->Offset;
	GPropObject = this;
	if (Result)
		GProperty->CopyCompleteValue(Result, GPropAddr);
}
IMPLEMENT_FUNCTION(UObject, EX_InstanceVariable, execInstanceVariable);

//void UObject::execDefaultVariable(FFrame& Stack, RESULT_DECL)
//{
//	GProperty = (UProperty*)Stack.ReadObject();
//
//	UObject* DefaultObject = NULL;
//	if (HasAnyFlags(RF_ClassDefaultObject))
//	{
//		// we will already be in the context of the class default object
//		// if the variable is being accessed from a static function
//		DefaultObject = this;
//	}
//	else
//	{
//		DefaultObject = GetArchetype();
//
//		// an ObjectArchetype doesn't necessarily have to be of the same class and
//		// it's possible that the ObjectArchetype for this object doesn't contain
//		// the property we're referencing.  If that's the case, we'll have to use
//		// the class defaults instead
//		if (GProperty->Offset >= DefaultObject->GetClass()->GetPropertiesSize())
//		{
//			DefaultObject = GetClass()->GetDefaultObject();
//		}
//	}
//
//	assert(DefaultObject);
//	BYTE* DefaultData = (BYTE*)DefaultObject;
//	GPropAddr = DefaultData + GProperty->Offset;
//
//	GPropObject = NULL;
//	if (Result)
//		GProperty->CopyCompleteValue(Result, GPropAddr);
//}
//IMPLEMENT_FUNCTION(UObject, EX_DefaultVariable, execDefaultVariable);

void UObject::execStateVariable(FFrame& Stack, RESULT_DECL)
{
	assert(StateFrame != NULL);
	assert(StateFrame->Locals != NULL);
	GProperty = (UProperty*)Stack.ReadObject();
	GPropAddr = StateFrame->Locals + GProperty->Offset;
	GPropObject = NULL;
	if (Result)
	{
		GProperty->CopyCompleteValue(Result, GPropAddr);
	}
}
IMPLEMENT_FUNCTION(UObject, EX_StateVariable, execStateVariable);


void UObject::execNameConst(FFrame& Stack, RESULT_DECL)
{
	*(FName*)Result = Stack.ReadName();
}
IMPLEMENT_FUNCTION(UObject, EX_NameConst, execNameConst);

void UObject::execDynArrayElement(FFrame& Stack, RESULT_DECL)
{
	// Get array index expression.
	INT Index = 0;
	Stack.Step(Stack.Object, &Index);

	GProperty = NULL;
	Stack.Step(this, NULL);
	GPropObject = this;

	// Add scaled offset to base pointer.
	if (GProperty && GPropAddr)
	{
		UArrayProperty* ArrayProp = (UArrayProperty*)(GProperty);
		assert(ArrayProp);

		FScriptArray* Array = (FScriptArray*)GPropAddr;
		if (Index >= Array->Num() || Index<0)
		{
			//if we are returning a value, check for out-of-bounds
			if (Result || Index < 0)
			{
				GPropAddr = 0;
				GPropObject = NULL;
				if (Result)
				{
					memset(Result, 0, ArrayProp->Inner->ElementSize);
				}

				return;
			}
			else
			{
				INT OrigSize = Array->Num();
				INT Count = Index - OrigSize + 1;
				Array->AddZeroed(Count, ArrayProp->Inner->ElementSize);

				// if this is an array of structs, and the struct has defaults, propagate those now
				//UStructProperty* StructInner = dynamic_cast<UStructProperty*>(ArrayProp->Inner);
				//if (StructInner && StructInner->Struct->GetDefaultsCount())
				//{
				//	// no need to initialize the element at Index, since this element is being assigned a new value in the next step
				//	for (INT i = OrigSize; i < Index; i++)
				//	{
				//		BYTE* Dest = (BYTE*)Array->GetData() + i * ArrayProp->Inner->ElementSize;
				//		StructInner->InitializeValue(Dest);
				//	}
				//}
			}
		}

		GPropAddr = (BYTE*)Array->GetData() + Index * ArrayProp->Inner->ElementSize;

		// Add scaled offset to base pointer.
		if (Result)
		{
			ArrayProp->Inner->CopySingleValue(Result, GPropAddr);
		}
	}
}
IMPLEMENT_FUNCTION(UObject, EX_DynArrayElement, execDynArrayElement);

void UObject::execDynArrayInsert(FFrame& Stack, RESULT_DECL)
{
	GPropObject = this;
	GProperty = NULL;
	Stack.Step(this, NULL);
	UArrayProperty* ArrayProperty = dynamic_cast<UArrayProperty*>(GProperty);
	FScriptArray* Array = (FScriptArray*)GPropAddr;

	P_GET_INT(Index);
	P_GET_INT(Count);
	P_FINISH;
	if (Array && Count)
	{
		assert(ArrayProperty);
		if (Count < 0)
		{		
			return;
		}
		if (Index < 0 || Index > Array->Num())
		{
			Index = Clamp(Index, 0, Array->Num());
		}
		Array->InsertZeroed(Index, Count, ArrayProperty->Inner->ElementSize);

		// if this is an array of structs, and the struct has defaults, propagate those now
		//UStructProperty* StructInner = dynamic_cast<UStructProperty*>(ArrayProperty->Inner);
		//if (StructInner && StructInner->Struct->GetDefaultsCount())
		//{
		//	for (INT i = Index; i < Index + Count; i++)
		//	{
		//		BYTE* Dest = (BYTE*)Array->GetData() + i * ArrayProperty->Inner->ElementSize;
		//		StructInner->InitializeValue(Dest);
		//	}
		//}
	}
}
IMPLEMENT_FUNCTION(UObject, EX_DynArrayInsert, execDynArrayInsert);

void UObject::execDynArrayRemove(FFrame& Stack, RESULT_DECL)
{
	GProperty = NULL;
	GPropObject = this;
	Stack.Step(this, NULL);
	UArrayProperty* ArrayProperty = dynamic_cast<UArrayProperty*>(GProperty);
	FScriptArray* Array = (FScriptArray*)GPropAddr;

	P_GET_INT(Index);
	P_GET_INT(Count);
	P_FINISH;
	if (Array && Count)
	{
		assert(ArrayProperty);
		if (Count < 0)
		{
			return;
		}
		if (Index < 0 || Index >= Array->Num() || Index + Count > Array->Num())
		{
			Index = Clamp(Index, 0, Array->Num());
			if (Index + Count > Array->Num())
				Count = Array->Num() - Index;
		}

		for (INT i = Index + Count - 1; i >= Index; i--)
		{
			ArrayProperty->Inner->DestroyValue((BYTE*)Array->GetData() + ArrayProperty->Inner->ElementSize*i);
		}
		Array->Remove(Index, Count, ArrayProperty->Inner->ElementSize);
	}
}
IMPLEMENT_FUNCTION(UObject, EX_DynArrayRemove, execDynArrayRemove);

void UObject::execDynArrayFind(FFrame& Stack, RESULT_DECL)
{
	// the script compiler doesn't allow calling Find on a dynamic array unless it's being assigned to something, so we should _always_ have a Result buffer
	assert(Result);

	GProperty = NULL;
	GPropAddr = NULL;
	GPropObject = this;

	// read the array property off of the stack; if the array is being accessed through a NULL context, we'll just skip past the
	// bytecodes corresponding to the expressions for the parameters of the 'find'
	Stack.Step(this, NULL);

	// if GPropAddr is NULL, we weren't able to read a valid property address from the stream, which should mean that we evaluted a NULL
	// context expression (accessed none)
	if (GPropAddr != NULL)
	{
		// advance past the word used to hold the skip offset - we won't need it
		Stack.Code += sizeof(CodeSkipSizeType);

		// got it
		UArrayProperty* ArrayProp = dynamic_cast<UArrayProperty*>(GProperty);
		assert(ArrayProp);

		FScriptArray* Array = (FScriptArray*)GPropAddr;

		// figure out what type to read off the stack
		UProperty *InnerProp = ArrayProp->Inner;
		assert(InnerProp != NULL);

		// evaluate the value that we should search for, store it in SearchItem
		BYTE *SearchItem = (BYTE*)malloc(InnerProp->ElementSize);
		memset(SearchItem, 0, InnerProp->ElementSize);
		Stack.Step(Stack.Object, SearchItem);
		P_FINISH;
		// for bools, we need to convert the SearchItem to match the property's BitMask so Identical() works as expected
		if (dynamic_cast<UBoolProperty*>(InnerProp) && *(BITFIELD*)SearchItem != 0)
		{
			*(BITFIELD*)SearchItem = ((UBoolProperty*)InnerProp)->BitMask;
		}

		// compare against each element in the array
		INT ResultIndex = INDEX_NONE;
		for (INT Idx = 0; Idx < Array->Num() && ResultIndex == INDEX_NONE; Idx++)
		{
			BYTE *CheckItem = ((BYTE*)Array->GetData() + (Idx * InnerProp->ElementSize));
			if (InnerProp->Identical(SearchItem, CheckItem))
			{
				ResultIndex = Idx;
			}
		}

		//if (InnerProp->HasAnyPropertyFlags(CPF_NeedCtorLink))
		//{
		//	InnerProp->DestroyValue(SearchItem);
		//}

		// assign the resulting index
		*(INT*)Result = ResultIndex;
	}
	else
	{
		// accessed none while trying to evaluate the address of the array - skip over the parameter expression bytecodes
		CodeSkipSizeType NumBytesToSkip = Stack.ReadCodeSkipCount();
		Stack.Code += NumBytesToSkip;

		// set the return value to the expected "invalid" value
		*(INT*)Result = INDEX_NONE;
	}
}
IMPLEMENT_FUNCTION(UObject, EX_DynArrayFind, execDynArrayFind);

void UObject::execDynArrayAdd(FFrame& Stack, RESULT_DECL)
{
	GProperty = NULL;
	GPropObject = this;
	Stack.Step(this, NULL);
	UArrayProperty* ArrayProperty = dynamic_cast<UArrayProperty*>(GProperty);
	FScriptArray* Array = (FScriptArray*)GPropAddr;

	P_GET_INT(Count);
	P_FINISH;
	if (Array && Count)
	{
		assert(ArrayProperty);
		if (Count < 0)
		{
			//Stack.Logf(TEXT("Attempt to add a negative number of elements '%s'"), *ArrayProperty->GetName());
			return;
		}
		INT Index = Array->AddZeroed(Count, ArrayProperty->Inner->ElementSize);

		// if this is an array of structs, and the struct has defaults, propagate those now
		//UStructProperty* StructInner = dynamic_cast<UStructProperty*>(ArrayProperty->Inner);
		//if (StructInner && StructInner->Struct->GetDefaultsCount())
		//{
		//	for (INT i = Index; i < Index + Count; i++)
		//	{
		//		BYTE* Dest = (BYTE*)Array->GetData() + i * ArrayProperty->Inner->ElementSize;
		//		StructInner->InitializeValue(Dest);
		//	}
		//}
		// set the return as the index of the first added
		*(INT*)Result = Index;
	}
}
IMPLEMENT_FUNCTION(UObject, EX_DynArrayAdd, execDynArrayAdd);


void UObject::execDynArrayIterator(FFrame& Stack, RESULT_DECL)
{
	// grab the array
	GPropObject = this;
	GProperty = NULL;
	Stack.Step(this, NULL);
	// if we accessed None in the executed expression, execContext() will skip to the end of the iterator block
	// so we can simply return from here
	if (GProperty == NULL)
	{
		return;
	}
	
	UArrayProperty* ArrayProperty = (UArrayProperty*)GProperty;
	FScriptArray* Array = (FScriptArray*)GPropAddr;
	UProperty* InnerProperty = ArrayProperty->Inner;

	// grab the out item
	Stack.Step(this, NULL);
	UProperty* ItemProperty = GProperty;
	BYTE* ItemPropAddr = GPropAddr;

	// see if an index was specified
	BYTE IndexByte = *(BYTE*)Stack.Code;
	Stack.Code += sizeof(BYTE);

	// serialize the index expression
	GProperty = NULL;
	GPropAddr = NULL;
	Stack.Step(this, NULL);
	// and grab the property, will quite possibly be null
	UProperty* IndexProperty = GProperty;
	BYTE* IndexPropAddr = GPropAddr;

	// should the iterator skip null items?  currently only for object properties 
	const UBOOL bSkipNullItems = false;//InnerProperty->IsA(UObjectProperty::StaticClass());

	INT Index = 0;
	PRE_ITERATOR;
	if (bSkipNullItems)
	{
		// iterate till we find a valid value
		*(UObject**)ItemPropAddr = NULL;
		while (*(UObject**)ItemPropAddr == NULL && Index < Array->Num())
		{
			InnerProperty->CopySingleValue(ItemPropAddr, ((BYTE*)Array->GetData() + (Index * InnerProperty->ElementSize)));
			// if there is an index property then assign the current index
			if (IndexProperty != NULL)
			{
				IndexProperty->CopySingleValue(IndexPropAddr, &Index);
			}
			Index++;
		}
		// if we don't have a valid value then exit the iterator
		if (*(UObject**)ItemPropAddr == NULL)
		{
			EXIT_ITERATOR;
			break;
		}
	}
	else
	{
		// otherwise iterate to the end of the array
		if (Index < Array->Num())
		{
			InnerProperty->CopySingleValue(ItemPropAddr, ((BYTE*)Array->GetData() + (Index * InnerProperty->ElementSize)));
			// if there is an index property then assign the current index
			if (IndexProperty != NULL)
			{
				IndexProperty->CopySingleValue(IndexPropAddr, &Index);
			}
			Index++;
		}
		else
		{
			EXIT_ITERATOR;
			break;
		}
	}
	POST_ITERATOR;
}
IMPLEMENT_FUNCTION(UObject, EX_DynArrayIterator, execDynArrayIterator);

void UObject::execStructMember(FFrame& Stack, RESULT_DECL)
{
	// Get structure element.
	UProperty* Property = (UProperty*)Stack.ReadObject();
	assert(Property);

	// Read the struct we're accessing
	UStruct* Struct = (UStruct*)Stack.ReadObject();
	assert(Struct);

	// read the byte that indicates whether we must make a copy of the struct in order to access its members
	BYTE bMemberAccessRequiresStructCopy = *Stack.Code++;

	BYTE* Buffer = NULL;
	if (bMemberAccessRequiresStructCopy != 0)
	{
		// We must use the struct's aligned size so that if Struct's aligned size is larger than its PropertiesSize, we don't overrun the buffer when
		// UStructProperty::CopyCompleteValue performs an appMemcpy using the struct property's ElementSize (which is always aligned)
		const INT BufferSize = Align(Struct->GetPropertiesSize(), Struct->GetMinAlignment());

		// allocate a buffer that will contain the copy of the struct we're accessing
		Buffer = (BYTE*)malloc(BufferSize);
		memset(Buffer, 0, BufferSize);
	}

	// read the byte that indicates whether the struct will be modified by the expression that's using it
	BYTE bStructWillBeModified = *Stack.Code++;

	// now evaluate the expression corresponding to the struct value.
	// If bMemberRequiresStructCopy is 1, Buffer will be non-NULL and the value of the struct will be copied into Buffer
	// Otherwise, this will set GPropAddr to the address of the value for the struct, and GProperty to the struct variable
	GPropAddr = NULL;
	Stack.Step(this, Buffer);

	// Set result.
	GProperty = Property;
	GPropObject = this;

	// GPropAddr is non-NULL if the struct evaluated in the previous call to Step is an instance, local, or default variable
	if (GPropAddr)
	{
		GPropAddr += Property->Offset;
	}

	if (Buffer != NULL)
	{
		// Result is non-NULL when we're accessing the struct member as an r-value;  Result is where we're copying the value of the struct member to
		if (Result)
		{
			Property->CopyCompleteValue(Result, Buffer + Property->Offset);
		}

		// if we allocated a temporary buffer, clean up any properties which may have allocated heap memory
		//for (UProperty* P = Struct->ConstructorLink; P; P = P->ConstructorLinkNext)
		//{
		//	P->DestroyValue(Buffer + P->Offset);
		//}
	}
	else if (Result != NULL)
	{
		if (GPropAddr != NULL)
		{
			// Result is non-NULL when we're accessing the struct member as an r-value;  Result is where we're copying the value of the struct member to
			Property->CopyCompleteValue(Result, GPropAddr);
		}
		else
		{
			//if (Property->PropertyFlags & CPF_NeedCtorLink)
			//{
			//	Property->DestroyValue(Result);
			//}
			//appMemzero(Result, Property->ArrayDim * Property->ElementSize);
		}
	}
}
IMPLEMENT_FUNCTION(UObject, EX_StructMember, execStructMember);

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
	// Get switch size.
	UField* ExpressionField = NULL;
	VariableSizeType bSize = Stack.ReadVariableSize(&ExpressionField);
	if (bSize == 0)
	{
		if (dynamic_cast<UIntProperty*>(ExpressionField) != NULL)
		{
			// DynamicArray.Length
			bSize = sizeof(INT);
		}

		// add additional handling required for non-property expressions here
	}

	// Get switch expression.
	BYTE SwitchBuffer[1024];
	BYTE Buffer[1024];
	memset(Buffer, 0, sizeof(1024));
	memset(SwitchBuffer, 0, sizeof(1024));
	Stack.Step(Stack.Object, SwitchBuffer);

	UStrProperty* StringProp = dynamic_cast<UStrProperty*>(ExpressionField);

	// Check each case clause till we find a match.
	for (; ; )
	{
		// Skip over case token.
		assert(*Stack.Code == EX_Case);
		Stack.Code++;

		// Get address of next handler.
		INT wNext = Stack.ReadWord();
		if (wNext == MAXWORD) // Default case or end of cases.
		{
			break;
		}

		// Get case expression.
		Stack.Step(Stack.Object, Buffer);

		//if (StringProp == NULL ? (memcmp(SwitchBuffer, Buffer, bSize) == 0) : StringProp/*(*(FString*)SwitchBuffer == *(FString*)Buffer)*/)
		assert(StringProp == NULL);
		if(memcmp(SwitchBuffer, Buffer, bSize) == 0)
		{
			break;
		}

		// Jump to next handler.
		Stack.Code = &Stack.Node->Script[wNext];
	}
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
	if (wNext != MAXWORD)
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
	P_GET_NAME(N);
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

void UObject::execAssert(FFrame& Stack, RESULT_DECL)
{
	// Get line number.
	INT wLine = Stack.ReadWord();

	// find out whether we are in debug mode and therefore should crash on failure
	BYTE bDebug = *(BYTE*)Stack.Code++;

	// Get boolean assert value.
	DWORD Value = 0;
	Stack.Step(Stack.Object, &Value);

	// Check it.
	//if (!Value && (!GDebugger || !GDebugger->NotifyAssertionFailed(wLine)))
	//{
	//	Stack.Logf(TEXT("%s"), *Stack.GetStackTrace());
	//	Stack.Logf(bDebug ? NAME_Critical : NAME_ScriptWarning, TEXT("Assertion failed, line %i"), wLine);
	//}
}
IMPLEMENT_FUNCTION(UObject, EX_Assert, execAssert);


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

void UObject::execLetBool(FFrame& Stack, RESULT_DECL)
{
	GPropAddr = NULL;
	GProperty = NULL;
	GPropObject = NULL;

	// Get the variable and address to place the data.
	Stack.Step(Stack.Object, NULL);

	/*
	Class bool properties are packed together as bitfields, so in order
	to set the value on the correct bool, we need to mask it against
	the bool property's BitMask.

	Local bool properties (declared inside functions) are not packed, thus
	their bitmask is always 1.

	Bool properties inside dynamic arrays and tmaps are also not packed together.
	If the bool property we're accessing is an element in a dynamic array, GProperty
	will be pointing to the dynamic array that has a UBoolProperty as its inner, so
	we'll need to check for that.
	*/
	BITFIELD*      BoolAddr = (BITFIELD*)GPropAddr;
	UBoolProperty* BoolProperty = dynamic_cast<UBoolProperty*>(GProperty);
	if (BoolProperty == NULL)
	{
		assert(false);
		//UArrayProperty* ArrayProp = ExactCast<UArrayProperty>(GProperty);
		//if (ArrayProp != NULL)
		//{
		//	BoolProperty = ExactCast<UBoolProperty>(ArrayProp->Inner);
		//}
		/* @todo tmaps
		else
		{
		UMapProperty* MapProp = ExactCast<UMapProperty>(GProperty);
		}
		*/
	}

	INT NewValue = 0;  // must be INT...execTrue/execFalse return 32 bits. --ryan.

	// evaluate the r-value for this expression into Value
	Stack.Step(Stack.Object, &NewValue);
	if (BoolAddr)
	{
		if (NewValue)
		{
			*BoolAddr |= BoolProperty->BitMask;
		}
		else
		{
			*BoolAddr &= ~BoolProperty->BitMask;
		}
	}
}
IMPLEMENT_FUNCTION(UObject, EX_LetBool, execLetBool);

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

void UObject::execIntConst(FFrame& Stack, RESULT_DECL)
{
	*(INT*)Result = Stack.ReadInt();
}
IMPLEMENT_FUNCTION(UObject, EX_IntConst, execIntConst);

void UObject::execFloatConst(FFrame& Stack, RESULT_DECL)
{
	*(FLOAT*)Result = Stack.ReadFloat();
}
IMPLEMENT_FUNCTION(UObject, EX_FloatConst, execFloatConst);

void UObject::execStringConst(FFrame& Stack, RESULT_DECL)
{
	*(FString*)Result = (char*)Stack.Code;
	while (*Stack.Code)
		Stack.Code++;
	Stack.Code++;
}
IMPLEMENT_FUNCTION(UObject, EX_StringConst, execStringConst);

void UObject::execUnicodeStringConst(FFrame& Stack, RESULT_DECL)
{
	*(FString*)Result = FString((UNICHAR*)Stack.Code);

	while (*(WORD*)Stack.Code)
	{
		Stack.Code += sizeof(WORD);
	}
	Stack.Code += sizeof(WORD);
}
IMPLEMENT_FUNCTION(UObject, EX_UnicodeStringConst, execUnicodeStringConst);


void UObject::execObjectConst(FFrame& Stack, RESULT_DECL)
{
	*(UObject**)Result = (UObject*)Stack.ReadObject();
}
IMPLEMENT_FUNCTION(UObject, EX_ObjectConst, execObjectConst);


void UObject::execByteConst(FFrame& Stack, RESULT_DECL)
{
	*(BYTE*)Result = *Stack.Code++;
}
IMPLEMENT_FUNCTION(UObject, EX_ByteConst, execByteConst);


void UObject::execRotationConst(FFrame& Stack, RESULT_DECL)
{
	auto Pitch = Stack.ReadInt();
	auto Yaw = Stack.ReadInt();
	auto Roll = Stack.ReadInt();
}
IMPLEMENT_FUNCTION(UObject, EX_RotationConst, execRotationConst);

void UObject::execVectorConst(FFrame& Stack, RESULT_DECL)
{
	auto X = Stack.ReadFloat();
	auto Y = Stack.ReadFloat();
	auto Z = Stack.ReadFloat();
}
IMPLEMENT_FUNCTION(UObject, EX_VectorConst, execVectorConst);

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


void UObject::execBoolVariable(FFrame& Stack, RESULT_DECL)
{
	// Get bool variable.
	BYTE B = *Stack.Code++;

	// we pull 32-bits of data out, which is really a UBoolProperty* in some representation (depending on platform)
	ScriptPointerType TempCode;
//#ifdef REQUIRES_ALIGNED_INT_ACCESS
//	appMemcpy(&TempCode, Stack.Code, sizeof(ScriptPointerType));
//#else
	TempCode = *(ScriptPointerType*)Stack.Code;
//#endif
	// turn that DWORD into a UBoolProperty pointer
	UBoolProperty* Property = (UBoolProperty*)(TempCode);

	(this->*GNatives[B])(Stack, NULL);
	GProperty = Property;
	GPropObject = this;

	// Note that we're not returning an in-place pointer to the bool, so EX_Let 
	// must take special precautions with bools.
	if (Result)
	{
		*(BITFIELD*)Result = (GPropAddr && (*(BITFIELD*)GPropAddr & ((UBoolProperty*)GProperty)->BitMask)) ? 1 : 0;
	}
}
IMPLEMENT_FUNCTION(UObject, EX_BoolVariable, execBoolVariable);


void UObject::execNativeParm(FFrame& Stack, RESULT_DECL)
{
	UProperty* Property = (UProperty*)Stack.ReadObject();
	if (Result)
	{
		GPropObject = NULL;
		//if (Property->PropertyFlags & CPF_OutParm)
		//{
		//	// look through the out parameter infos and find the one that has the address of this property
		//	FOutParmRec* Out = Stack.OutParms;
		//	checkSlow(Out);
		//	while (Out->Property != Property)
		//	{
		//		Out = Out->NextOutParm;
		//		checkSlow(Out);
		//	}
		//	GPropAddr = Out->PropAddr;
		//	// no need to copy property value, since the caller is just looking for GPropAddr
		//}
		//else
		{
			GPropAddr = Stack.Locals + Property->Offset;
			Property->CopyCompleteValue(Result, Stack.Locals + Property->Offset);
		}
	}
}
IMPLEMENT_FUNCTION(UObject, EX_NativeParm, execNativeParm);

/////////////////////////
// Context expressions //
/////////////////////////

void UObject::execSelf(FFrame& Stack, RESULT_DECL)
{
	// Get Self actor for this context.
	*(UObject**)Result = this;
}
IMPLEMENT_FUNCTION(UObject, EX_Self, execSelf);

void UObject::execContext(FFrame& Stack, RESULT_DECL)
{
	GProperty = NULL;

	// Get object variable.
	UObject* NewContext = NULL;
	Stack.Step(this, &NewContext);

	// Execute or skip the following expression in the object's context.
	if (NewContext != NULL)
	{
		Stack.Code += sizeof(CodeSkipSizeType) + sizeof(ScriptPointerType) + sizeof(BYTE);
		Stack.Step(NewContext, Result);
	}
	else
	{
		if (GProperty != NULL)
		{
			//Stack.Logf(NAME_ScriptWarning, TEXT("Accessed None '%s'"), *GProperty->GetName());
		}
		else
		{
			// GProperty will be NULL under the following conditions:
			// 1. the context expression was a function call which returned an object
			// 2. the context expression was a literal object reference
			//Stack.Logf(NAME_ScriptWarning, TEXT("Accessed None"));
		}

		// DEBUGGER
		//if (GDebugger)
		//{
		//	GDebugger->NotifyAccessedNone();
		//}

		CodeSkipSizeType wSkip = Stack.ReadCodeSkipCount();
		VariableSizeType bSize = Stack.ReadVariableSize();
		Stack.Code += wSkip;
		GPropAddr = NULL;
		GProperty = NULL;
		GPropObject = NULL;
		if (Result)
		{
			memset(Result, 0, bSize);
		}
	}
}
IMPLEMENT_FUNCTION(UObject, EX_Context, execContext);


void UObject::execArrayElement(FFrame& Stack, RESULT_DECL)
{
	// Get array index expression.
	INT Index = 0;
	Stack.Step(Stack.Object, &Index);

	// Get base element (must be a variable!!).
	GProperty = NULL;
	Stack.Step(this, NULL);
	GPropObject = this;

	// Add scaled offset to base pointer.
	if (GProperty && GPropAddr)
	{
		// Bounds check.
		if (Index >= GProperty->ArrayDim || Index<0)
		{
			// Display out-of-bounds warning and continue on with index clamped to valid range.
			//Stack.Logf(NAME_Error, TEXT("Accessed array '%s.%s' out of bounds (%i/%i)"), *GetName(), *GProperty->GetName(), Index, GProperty->ArrayDim);
			Index = Clamp(Index, 0, GProperty->ArrayDim - 1);
		}

		// Update address.
		GPropAddr += Index * GProperty->ElementSize;
		if (Result)
		{
			GProperty->CopySingleValue(Result, GPropAddr);
		}
	}
}
IMPLEMENT_FUNCTION(UObject, EX_ArrayElement, execArrayElement);


void UObject::execVirtualFunction(FFrame& Stack, RESULT_DECL)
{
	auto name = Stack.ReadName();
	std::cout << "Call function: " << name.ToString() << std::endl;
	// Call the virtual function.
	CallFunction(Stack, Result, FindFunction(name));
}
IMPLEMENT_FUNCTION(UObject, EX_VirtualFunction, execVirtualFunction);

void UObject::execFinalFunction(FFrame& Stack, RESULT_DECL)
{
	// Call the final function.
	//CallFunction(Stack, Result, (UFunction*)Stack.ReadObject());
}
IMPLEMENT_FUNCTION(UObject, EX_FinalFunction, execFinalFunction);

void UObject::execGlobalFunction(FFrame& Stack, RESULT_DECL)
{
	// Call global version of virtual function.
	//CallFunction(Stack, Result, FindFunctionChecked(Stack.ReadName(), 1));
}
IMPLEMENT_FUNCTION(UObject, EX_GlobalFunction, execGlobalFunction);


/*-----------------------------------------------------------------------------
Native iterator functions.
-----------------------------------------------------------------------------*/

void UObject::execIterator(FFrame& Stack, RESULT_DECL)
{}
IMPLEMENT_FUNCTION(UObject, EX_Iterator, execIterator);

void UObject::execIteratorPop(FFrame& Stack, RESULT_DECL)
{
	// this bytecode should be handled by the iterator functions themselves and never actually be executed
	// therefore, if we got here, it most likely means the script compiler is emitting a bad offset for skipping the iterator body
	//appErrorf(TEXT("Unexpected iterator pop command at %s:%04X"), *Stack.Node->GetFullName(), Stack.Code - &Stack.Node->Script(0));
}
IMPLEMENT_FUNCTION(UObject, EX_IteratorPop, execIteratorPop);

///////////////////////
// Struct comparison //
///////////////////////

void UObject::execStructCmpEq(FFrame& Stack, RESULT_DECL)
{
	UStruct* Struct = (UStruct*)Stack.ReadObject();
	assert(Struct);

	// We must use the struct's aligned size so that if Struct's aligned size is larger than its PropertiesSize, we don't overrun the buffer when
	// UStructProperty::CopyCompleteValue performs an appMemcpy using the struct property's ElementSize (which is always aligned)
	const INT BufferSize = Align(Struct->GetPropertiesSize(), Struct->GetMinAlignment());

	BYTE*    Buffer1 = (BYTE*)malloc(BufferSize);
	BYTE*    Buffer2 = (BYTE*)malloc(BufferSize);
	memset(Buffer1, 0, BufferSize);
	memset(Buffer2, 0, BufferSize);
	Stack.Step(this, Buffer1);
	Stack.Step(this, Buffer2);
	*(DWORD*)Result = 1;// Struct->StructCompare(Buffer1, Buffer2);

	// destruct any of the struct's properties that require it
	//for (UProperty* Prop = Struct->ConstructorLink; Prop != NULL; Prop = Prop->ConstructorLinkNext)
	//{
	//	Prop->DestroyValue((BYTE*)Buffer1 + Prop->Offset);
	//	Prop->DestroyValue((BYTE*)Buffer2 + Prop->Offset);
	//}
}
IMPLEMENT_FUNCTION(UObject, EX_StructCmpEq, execStructCmpEq);

void UObject::execStructCmpNe(FFrame& Stack, RESULT_DECL)
{
	UStruct* Struct = (UStruct*)Stack.ReadObject();
	assert(Struct);

	// We must use the struct's aligned size so that if Struct's aligned size is larger than its PropertiesSize, we don't overrun the buffer when
	// UStructProperty::CopyCompleteValue performs an appMemcpy using the struct property's ElementSize (which is always aligned)
	const INT BufferSize = Align(Struct->GetPropertiesSize(), Struct->GetMinAlignment());

	BYTE*    Buffer1 = (BYTE*)malloc(BufferSize);
	BYTE*    Buffer2 = (BYTE*)malloc(BufferSize);
	memset(Buffer1, 0, BufferSize);
	memset(Buffer2, 0, BufferSize);
	Stack.Step(this, Buffer1);
	Stack.Step(this, Buffer2);
	*(DWORD*)Result = 0;// !Struct->StructCompare(Buffer1, Buffer2);

	// destruct any of the struct's properties that require it
	//for (UProperty* Prop = Struct->ConstructorLink; Prop != NULL; Prop = Prop->ConstructorLinkNext)
	//{
	//	Prop->DestroyValue((BYTE*)Buffer1 + Prop->Offset);
	//	Prop->DestroyValue((BYTE*)Buffer2 + Prop->Offset);
	//}
}
IMPLEMENT_FUNCTION(UObject, EX_StructCmpNe, execStructCmpNe);

void UObject::execDynArrayLength(FFrame& Stack, RESULT_DECL)
{
	GProperty = NULL;
	Stack.Step(this, NULL);
	GPropObject = this;

	if (GPropAddr)
	{
		FScriptArray* Array = (FScriptArray*)GPropAddr;
		if (!Result)
		{
			//GRuntimeUCFlags |= RUC_ArrayLengthSet; //so that EX_Let knows that this is a length 'set'-ting
		}
		else
		{
			*(INT*)Result = Array->Num();
		}
	}
}
IMPLEMENT_FUNCTION(UObject, EX_DynArrayLength, execDynArrayLength);

void UObject::execConditional(FFrame& Stack, RESULT_DECL)
{
	// Get test expression
	UBOOL CondValue = 0;
	Stack.Step(Stack.Object, &CondValue);

	// Get first skip offset
	CodeSkipSizeType SkipOffset = Stack.ReadCodeSkipCount();

	// Skip ahead if false.
	if (!CondValue)
	{
		Stack.Code += SkipOffset + 2;
	}

	Stack.Step(Stack.Object, Result);

	// Get second skip offset
	if (CondValue)
	{
		SkipOffset = Stack.ReadCodeSkipCount();
	}

	// Skip ahead if true
	if (CondValue)
	{
		Stack.Code += SkipOffset;
	}
}
IMPLEMENT_FUNCTION(UObject, EX_Conditional, execConditional);


void UObject::execLocalOutVariable(FFrame& Stack, RESULT_DECL)
{
	assert(Stack.Object == this);
	// get the property we need to find
	GProperty = (UProperty*)Stack.ReadObject();
	GPropObject = NULL;

	// look through the out parameter infos and find the one that has the address of this property
	FOutParmRec* Out = Stack.OutParms;
	assert(Out);
	while (Out->Property != GProperty)
	{
		Out = Out->NextOutParm;
		assert(Out);
	}
	GPropAddr = Out->PropAddr;

	// if desired, copy the value in that address to Result
	if (Result)
	{
		GProperty->CopyCompleteValue(Result, GPropAddr);
	}
}
IMPLEMENT_FUNCTION(UObject, EX_LocalOutVariable, execLocalOutVariable);

void UObject::execDefaultParmValue(FFrame& Stack, RESULT_DECL)
{
	assert(Stack.Object == this);
	assert(Result != NULL);

	CodeSkipSizeType Offset = Stack.ReadCodeSkipCount();

	// a value was specified for this optional parameter when the function was called
	//if ((GRuntimeUCFlags&RUC_SkippedOptionalParm) == 0)
	//{
	//	// in this case, just skip over the default value expression
	//	Stack.Code += Offset;
	//}
	//else
	{
		// evaluate the default value into the local property address space
		while (*Stack.Code != EX_EndParmValue)
		{
			Stack.Step(Stack.Object, Result);
		}

		// advance past the EX_EndParmValue
		Stack.Code++;
	}

	// reset the runtime flag
	//GRuntimeUCFlags &= ~RUC_SkippedOptionalParm;
}
IMPLEMENT_FUNCTION(UObject, EX_DefaultParmValue, execDefaultParmValue);

void UObject::execEmptyParmValue(FFrame& Stack, RESULT_DECL)
{
	// indicates that no value was specified in the function call for this optional parameter
	//GRuntimeUCFlags |= RUC_SkippedOptionalParm;

	GPropObject = NULL;
	GPropAddr = NULL;
	GProperty = NULL;
}
IMPLEMENT_FUNCTION(UObject, EX_EmptyParmValue, execEmptyParmValue);

void UObject::execEndOfScript(FFrame& Stack, RESULT_DECL)
{
	//appErrorf(TEXT("Execution beyond end of script in %s on %s"), *Stack.Node->GetFullName(), *Stack.Object->GetFullName());
}
IMPLEMENT_FUNCTION(UObject, EX_EndOfScript, execEndOfScript);

void UObject::execAdd_IntInt(FFrame& Stack, RESULT_DECL)
{
	P_GET_INT(A);
	P_GET_INT(B);
	P_FINISH;

	*(INT*)Result = A + B;
}
IMPLEMENT_FUNCTION(UObject, 146, execAdd_IntInt);

void UObject::execGreater_IntInt(FFrame& Stack, RESULT_DECL)
{
	P_GET_INT(A);
	P_GET_INT(B);
	P_FINISH;

	*(DWORD*)Result = A > B;
}
IMPLEMENT_FUNCTION(UObject, 151, execGreater_IntInt);

void UObject::execMultiply_IntInt(FFrame& Stack, RESULT_DECL)
{
	P_GET_INT(A);
	P_GET_INT(B);
	P_FINISH;

	*(INT*)Result = A * B;
}
IMPLEMENT_FUNCTION(UObject, 144, execMultiply_IntInt);

void UObject::execDivide_IntInt(FFrame& Stack, RESULT_DECL)
{
	P_GET_INT(A);
	P_GET_INT(B);
	P_FINISH;

	if (B == 0)
	{
		assert(false);
		//Stack.Logf(NAME_ScriptWarning, TEXT("Divide by zero"));
	}

	*(INT*)Result = B ? A / B : 0;
}
IMPLEMENT_FUNCTION(UObject, 145, execDivide_IntInt);

void UObject::execPercent_IntInt(FFrame& Stack, RESULT_DECL)
{
	P_GET_INT(A);
	P_GET_INT(B);
	P_FINISH;

	if (B == 0)
	{
		assert(false);
		//Stack.Logf(NAME_ScriptWarning, TEXT("Modulo by zero"));
	}

	*(INT*)Result = (B != 0) ? (A % B) : 0;
}
IMPLEMENT_FUNCTION(UObject, 253, execPercent_IntInt);

void UObject::execSubtract_IntInt(FFrame& Stack, RESULT_DECL)
{
	P_GET_INT(A);
	P_GET_INT(B);
	P_FINISH;

	*(INT*)Result = A - B;
}
IMPLEMENT_FUNCTION(UObject, 147, execSubtract_IntInt);

void UObject::execLessLess_IntInt(FFrame& Stack, RESULT_DECL)
{
	P_GET_INT(A);
	P_GET_INT(B);
	P_FINISH;

	*(INT*)Result = A << B;
}
IMPLEMENT_FUNCTION(UObject, 148, execLessLess_IntInt);

void UObject::execGreaterGreater_IntInt(FFrame& Stack, RESULT_DECL)
{
	P_GET_INT(A);
	P_GET_INT(B);
	P_FINISH;

	*(INT*)Result = A >> B;
}
IMPLEMENT_FUNCTION(UObject, 149, execGreaterGreater_IntInt);

void UObject::execLess_IntInt(FFrame& Stack, RESULT_DECL)
{
	P_GET_INT(A);
	P_GET_INT(B);
	P_FINISH;

	*(DWORD*)Result = A < B;
}
IMPLEMENT_FUNCTION(UObject, 150, execLess_IntInt);

void UObject::execLessEqual_IntInt(FFrame& Stack, RESULT_DECL)
{
	P_GET_INT(A);
	P_GET_INT(B);
	P_FINISH;

	*(DWORD*)Result = A <= B;
}
IMPLEMENT_FUNCTION(UObject, 152, execLessEqual_IntInt);

void UObject::execGreaterEqual_IntInt(FFrame& Stack, RESULT_DECL)
{
	P_GET_INT(A);
	P_GET_INT(B);
	P_FINISH;

	*(DWORD*)Result = A >= B;
}
IMPLEMENT_FUNCTION(UObject, 153, execGreaterEqual_IntInt);

void UObject::execEqualEqual_IntInt(FFrame& Stack, RESULT_DECL)
{
	P_GET_INT(A);
	P_GET_INT(B);
	P_FINISH;

	*(DWORD*)Result = A == B;
}
IMPLEMENT_FUNCTION(UObject, 154, execEqualEqual_IntInt);

void UObject::execNotEqual_IntInt(FFrame& Stack, RESULT_DECL)
{
	P_GET_INT(A);
	P_GET_INT(B);
	P_FINISH;

	*(DWORD*)Result = A != B;
}
IMPLEMENT_FUNCTION(UObject, 155, execNotEqual_IntInt);

void UObject::execAnd_IntInt(FFrame& Stack, RESULT_DECL)
{
	P_GET_INT(A);
	P_GET_INT(B);
	P_FINISH;

	*(INT*)Result = A & B;
}
IMPLEMENT_FUNCTION(UObject, 156, execAnd_IntInt);

void UObject::execXor_IntInt(FFrame& Stack, RESULT_DECL)
{
	P_GET_INT(A);
	P_GET_INT(B);
	P_FINISH;

	*(INT*)Result = A ^ B;
}
IMPLEMENT_FUNCTION(UObject, 157, execXor_IntInt);

void UObject::execOr_IntInt(FFrame& Stack, RESULT_DECL)
{
	P_GET_INT(A);
	P_GET_INT(B);
	P_FINISH;

	*(INT*)Result = A | B;
}
IMPLEMENT_FUNCTION(UObject, 158, execOr_IntInt);

//void UObject::execMultiplyEqual_IntFloat(FFrame& Stack, RESULT_DECL)
//{
//	P_GET_INT_REF(A);
//	P_GET_FLOAT(B);
//	P_FINISH;
//
//	*(INT*)Result = A = appTrunc(A * B);
//}
//IMPLEMENT_FUNCTION(UObject, 159, execMultiplyEqual_IntFloat);

//void UObject::execDivideEqual_IntFloat(FFrame& Stack, RESULT_DECL)
//{
//	P_GET_INT_REF(A);
//	P_GET_FLOAT(B);
//	P_FINISH;
//
//	*(INT*)Result = A = appTrunc(B ? A / B : 0.f);
//}
//IMPLEMENT_FUNCTION(UObject, 160, execDivideEqual_IntFloat);

void UObject::execAddEqual_IntInt(FFrame& Stack, RESULT_DECL)
{
	P_GET_INT_REF(A);
	P_GET_INT(B);
	P_FINISH;

	*(INT*)Result = (A += B);
}
IMPLEMENT_FUNCTION(UObject, 161, execAddEqual_IntInt);

void UObject::execSubtractEqual_IntInt(FFrame& Stack, RESULT_DECL)
{
	P_GET_INT_REF(A);
	P_GET_INT(B);
	P_FINISH;

	*(INT*)Result = (A -= B);
}
IMPLEMENT_FUNCTION(UObject, 162, execSubtractEqual_IntInt);

void UObject::execAddAdd_PreInt(FFrame& Stack, RESULT_DECL)
{
	P_GET_INT_REF(A);
	P_FINISH;

	*(INT*)Result = ++(A);
}
IMPLEMENT_FUNCTION(UObject, 163, execAddAdd_PreInt);

/////////////////////////
// Delegate comparison //
/////////////////////////
void UObject::execEqualEqual_DelegateDelegate(FFrame& Stack, RESULT_DECL)
{
	P_GET_DELEGATE(A);
	P_GET_DELEGATE(B);
	P_FINISH;			// required because the compiler emits a EX_EndFunctionParms token for this bytecode
	if (A.Object == NULL && A.FunctionName != NAME_None)
	{
		A.Object = this;
	}
	if (B.Object == NULL && B.FunctionName != NAME_None)
	{
		B.Object = this;
	}
	*(UBOOL*)Result = (A == B);
}
IMPLEMENT_FUNCTION(UObject, EX_EqualEqual_DelDel, execEqualEqual_DelegateDelegate);

void UObject::execNotEqual_DelegateDelegate(FFrame& Stack, RESULT_DECL)
{
	P_GET_DELEGATE(A);
	P_GET_DELEGATE(B);
	P_FINISH;			// required because the compiler emits a EX_EndFunctionParms token for this bytecode
	if (A.Object == NULL && A.FunctionName != NAME_None)
	{
		A.Object = this;
	}
	if (B.Object == NULL && B.FunctionName != NAME_None)
	{
		B.Object = this;
	}
	*(UBOOL*)Result = (A != B);
}
IMPLEMENT_FUNCTION(UObject, EX_NotEqual_DelDel, execNotEqual_DelegateDelegate);

void UObject::execEqualEqual_DelegateFunction(FFrame& Stack, RESULT_DECL)
{
	P_GET_DELEGATE(A);
	P_GET_DELEGATE(B);
	P_FINISH;			// required because the compiler emits a EX_EndFunctionParms token for this bytecode
	if (A.Object == NULL && A.FunctionName != NAME_None)
	{
		A.Object = this;
	}
	if (B.Object == NULL && B.FunctionName != NAME_None)
	{
		B.Object = this;
	}
	*(UBOOL*)Result = (A == B);
}
IMPLEMENT_FUNCTION(UObject, EX_EqualEqual_DelFunc, execEqualEqual_DelegateFunction);

void UObject::execNotEqual_DelegateFunction(FFrame& Stack, RESULT_DECL)
{
	P_GET_DELEGATE(A);
	P_GET_DELEGATE(B);
	P_FINISH;			// required because the compiler emits a EX_EndFunctionParms token for this bytecode
	if (A.Object == NULL && A.FunctionName != NAME_None)
	{
		A.Object = this;
	}
	if (B.Object == NULL && B.FunctionName != NAME_None)
	{
		B.Object = this;
	}
	*(UBOOL*)Result = (A != B);
}
IMPLEMENT_FUNCTION(UObject, EX_NotEqual_DelFunc, execNotEqual_DelegateFunction);

void UObject::execEmptyDelegate(FFrame& Stack, RESULT_DECL)
{
	((FScriptDelegate*)Result)->Object = NULL;
	((FScriptDelegate*)Result)->FunctionName = NAME_None;
}
IMPLEMENT_FUNCTION(UObject, EX_EmptyDelegate, execEmptyDelegate);


///////////////////////
// Delegates         //
///////////////////////

void UObject::execDelegateFunction(FFrame& Stack, RESULT_DECL)
{
	BYTE bLocalProp = *(BYTE*)Stack.Code;
	Stack.Code += sizeof(BYTE);

	// Look up delegate property.
	UDelegateProperty* DelegateProperty = (UDelegateProperty*)Stack.ReadObject();
	assert(DelegateProperty);
	//assert(DelegateProperty->IsA(UDelegateProperty::StaticClass()));

	// read the delegate info
	FScriptDelegate* Delegate = NULL;
	if (bLocalProp)
	{
		// read off the stack 
		Delegate = (FScriptDelegate*)(Stack.Locals + DelegateProperty->Offset);
	}
	else
	{
		// read off the object
		Delegate = (FScriptDelegate*)((BYTE*)this + DelegateProperty->Offset);
	}
	FName DelegateName = Stack.ReadName();
	if (Delegate->Object /*&& Delegate->Object->IsPendingKill()*/)
	{
		// disallow delegates being called on deleted objects
		Delegate->Object = NULL;
		Delegate->FunctionName = NAME_None;
	}
	if (Delegate->Object != NULL)
	{
		// attempt to call the delegate in the specified object
		Delegate->Object->CallFunction(Stack, Result, Delegate->Object->FindFunctionChecked(Delegate->FunctionName));
	}
	else if (Delegate->FunctionName != NAME_None)
	{
		// attempt to call the delegate in this object
		CallFunction(Stack, Result, FindFunctionChecked(Delegate->FunctionName));
	}
	// if we are calling a delegate function (not a delegate property), invoke the default implementation
	else if (DelegateProperty->SourceDelegate == NULL)
	{
		// default to the original default delegate
		CallFunction(Stack, Result, FindFunctionChecked(DelegateName));
	}
	else
	{
		// attempt to call a function through a delegate property, but the value is NULL
		// technically, we could call the default implementation if the property is using a delegate function
		// declared in this class's heirarchy, but the difference would likely be confusing and cause unexpected bugs,
		// so we always fail instead
		//Stack.Logf(NAME_ScriptWarning, TEXT("Attempt to call None through delegate property '%s'"), *DelegateProperty->GetName());
		SkipFunction(Stack, Result, DelegateProperty->SourceDelegate);
	}
}
IMPLEMENT_FUNCTION(UObject, EX_DelegateFunction, execDelegateFunction);

void UObject::execDelegateProperty(FFrame& Stack, RESULT_DECL)
{
	FName FunctionName = Stack.ReadName();
	UDelegateProperty* DelegateProp = (UDelegateProperty*)Stack.ReadObject();
	assert(DelegateProp == NULL /*|| DelegateProp->IsA(UDelegateProperty::StaticClass())*/);

	// if DelegateProp != NULL, we're assigning a delegate to another delegate
	// in this case, if the source delegate has a function assigned already, assign that function to the destination delegate as well
	// otherwise, assign the source delegate's default body to the destination delegate
	if (DelegateProp != NULL && ((FScriptDelegate*)((BYTE*)this + DelegateProp->Offset))->IsCallable(NULL))
	{
		*(FScriptDelegate*)Result = *(FScriptDelegate*)((BYTE*)this + DelegateProp->Offset);
	}
	else
	{
		((FScriptDelegate*)Result)->FunctionName = FunctionName;
		((FScriptDelegate*)Result)->Object = (FunctionName == NAME_None) ? NULL : this;
	}
}
IMPLEMENT_FUNCTION(UObject, EX_DelegateProperty, execDelegateProperty);

void UObject::execLetDelegate(FFrame& Stack, RESULT_DECL)
{
	// Get variable address.
	GPropAddr = NULL;
	GProperty = NULL;
	GPropObject = NULL;
	Stack.Step(Stack.Object, NULL); // Variable.
	FScriptDelegate* DelegateAddr = (FScriptDelegate*)GPropAddr;
	FScriptDelegate Delegate;
	Stack.Step(Stack.Object, &Delegate);
	if (DelegateAddr)
	{
		DelegateAddr->FunctionName = Delegate.FunctionName;
		DelegateAddr->Object = Delegate.Object;
	}
}
IMPLEMENT_FUNCTION(UObject, EX_LetDelegate, execLetDelegate);

void UObject::execInstanceDelegate(FFrame& Stack, RESULT_DECL)
{
	((FScriptDelegate*)Result)->FunctionName = Stack.ReadName();
	((FScriptDelegate*)Result)->Object = this;
}
IMPLEMENT_FUNCTION(UObject, EX_InstanceDelegate, execInstanceDelegate);

void UObject::CallFunction(FFrame& Stack, RESULT_DECL, UFunction* Function)
{
	// Make new stack frame in the current context.
	BYTE* Frame = (BYTE*)malloc(Function->PropertiesSize);
	memset(Frame, 0, Function->PropertiesSize);
	FFrame NewStack(this, Function, 0, Frame, &Stack);
	FOutParmRec** LastOut = &NewStack.OutParms;
	UProperty* Property;

	for (Property = (UProperty*)Function->Children; *Stack.Code != EX_EndFunctionParms; Property = (UProperty*)Property->Next)
	{
		BYTE* CurrentPropAddr = NULL;
		GPropAddr = NULL;
		GPropObject = NULL;

		{
			// copy the result of the expression for this parameter into the appropriate part of the local variable space
			BYTE* Param = NewStack.Locals + Property->Offset;
			assert(Param);

			CurrentPropAddr = Param;
			Stack.Step(Stack.Object, Param);
		}
	}
	Stack.Code++;

	// Execute the code.
	ProcessInternal(NewStack, Result);
}

void UObject::execDebugInfo(FFrame& Stack, RESULT_DECL) //DEBUGGER
{
	INT DebugToken = Stack.ReadInt();

	// We check for this to guarantee that we don't attempt to call GDebugger->DebugInfo in situations where
	// the next byte in the stream happened to be the same value as EX_DebugInfo, even though it isn't a real
	// debug opcode.  This often happens when running with release mode script where the next byte in the stream
	// is part of e.g. a pointer address.
	if (DebugToken != 100)
	{
		Stack.Code -= sizeof(INT);
		Stack.Code--;
		return;
	}

	INT LineNumber = Stack.ReadInt();
	INT InputPos = Stack.ReadInt();
	BYTE OpCode = *Stack.Code++;

	// Only valid when the debugger is running.
	assert(false);
	//if (GDebugger != NULL)
	//	GDebugger->DebugInfo(this, &Stack, OpCode, LineNumber, InputPos);
}
IMPLEMENT_FUNCTION(UObject, EX_DebugInfo, execDebugInfo);


void UObject::execMetaCast(FFrame& Stack, RESULT_DECL)
{
	UClass* MetaClass = (UClass*)Stack.ReadObject();

	// Compile actor expression.
	UObject* Castee = NULL;
	Stack.Step(Stack.Object, &Castee);
	*(UObject**)Result = (Castee && Castee->IsA(UClass::StaticClass()) && ((UClass*)Castee)->IsChildOf(MetaClass)) ? Castee : NULL;
}
IMPLEMENT_FUNCTION(UObject, EX_MetaCast, execMetaCast);


void UObject::execPrimitiveCast(FFrame& Stack, RESULT_DECL)
{
	INT B = *(Stack.Code)++;
	(Stack.Object->*GCasts[B])(Stack, Result);
}
IMPLEMENT_FUNCTION(UObject, EX_PrimitiveCast, execPrimitiveCast);


void UObject::execByteToInt(FFrame& Stack, RESULT_DECL)
{
	BYTE B = 0;
	Stack.Step(Stack.Object, &B);
	*(INT*)Result = B;
}
IMPLEMENT_CAST_FUNCTION(UObject, CST_ByteToInt, execByteToInt);

UFunction* UObject::FindFunction(FName name, UBOOL Global) const
{
	auto id = ScriptRuntimeContext::Get()->FindIndex(name.ToString());
	auto ret = static_cast<UFunction*>(ScriptRuntimeContext::Get()->IndexToObject(id));
	ret->Script = ScriptSerialize(ret->ScriptStorage, ret->BytecodeScriptSize);
	return ret;
}

UFunction* UObject::FindFunctionChecked(FName InName, UBOOL Global) const
{
	UFunction* Result = FindFunction(InName, Global);
	if (!Result)
	{
		//appErrorf(TEXT("Failed to find function %s in %s"), *InName.ToString(), *GetFullName());
	}
	return Result;
}

void UObject::ProcessInternal(FFrame& Stack, RESULT_DECL)
{
	enum { MAX_SIMPLE_RETURN_VALUE_SIZE_IN_DWORDS = (64 / sizeof(DWORD)) };
	DWORD Buffer[MAX_SIMPLE_RETURN_VALUE_SIZE_IN_DWORDS];

	while (*Stack.Code != EX_Return)
		Stack.Step(Stack.Object, Buffer);
	Stack.Code++;
	Stack.Step(Stack.Object, Result);
}


/** advances Stack's code past the parameters to the given Function and if the function has a return value, copies the zero value for that property to the memory for the return value
* @param Stack the script stack frame
* @param Result pointer to where the return value should be written
* @param Function the function being called
*/
void UObject::SkipFunction(FFrame& Stack, RESULT_DECL, UFunction* Function)
{
	// allocate temporary memory on the stack for evaluating parameters
	BYTE* Frame = (BYTE*)malloc(Function->PropertiesSize);
	memset(Frame, 0, Function->PropertiesSize);
	for (UProperty* Property = (UProperty*)Function->Children; *Stack.Code != EX_EndFunctionParms; Property = (UProperty*)Property->Next)
	{
		GPropAddr = NULL;
		GPropObject = NULL;
		// evaluate the expression into our temporary memory space
		// it'd be nice to be able to skip the copy, but most native functions assume a non-NULL Result pointer
		// so we can only do that if we know the expression is an l-value (out parameter)
		Stack.Step(Stack.Object, (Property->PropertyFlags & CPF_OutParm) ? NULL : Frame + Property->Offset);
	}
	// advance the code past EX_EndFunctionParms
	Stack.Code++;
#if !CONSOLE // no script debugger on console
	// check for debug info
	if (*Stack.Code == EX_DebugInfo)
	{
		Stack.Step(Stack.Object, NULL);
	}
#endif
	// destruct properties requiring it for which we had to use our temporary memory 
	// @warning: conditions for skipping DestroyValue() here must match conditions for passing NULL to Stack.Step() above
	for (UProperty* Destruct = Function->ConstructorLink; Destruct; Destruct = Destruct->ConstructorLinkNext)
	{
		if (!(Destruct->PropertyFlags & CPF_OutParm))
		{
			Destruct->DestroyValue(Frame + Destruct->Offset);
		}
	}

	UProperty* ReturnProp = Function->GetReturnProperty();
	if (ReturnProp != NULL)
	{
		// destroy old value if necessary
		if (ReturnProp->PropertyFlags & CPF_NeedCtorLink)
		{
			ReturnProp->DestroyValue(Result);
		}
		// copy zero value for return property into Result
		memset(Result, 0, ReturnProp->ArrayDim * ReturnProp->ElementSize);
	}
}

/**
* @return	TRUE if this object is of the specified type.
*/
inline UBOOL UObject::IsA(const UClass* SomeBase) const
{
	if (SomeBase == NULL)
	{
		return TRUE;
	}

	for (UClass* TempClass = Class; TempClass; TempClass = static_cast<UClass*>(TempClass->SuperStruct))
	{
		if (TempClass == SomeBase)
		{
			return TRUE;
		}
	}

	return FALSE;
}

FString FScriptDelegate::ToString(const UObject* OwnerObject) const
{
	const UObject* DelegateObject = Object;
	if (DelegateObject == NULL)
	{
		DelegateObject = OwnerObject;
	}

	//return DelegateObject->GetPathName() + TEXT(".") + FunctionName.ToString();
	return "TODO";
}