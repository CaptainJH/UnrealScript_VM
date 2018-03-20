#pragma once
#include <string>
#include <array>
#include "Types.h"
#include "UnStack.h"

//
// Evaluatable expression item types.
//
enum EExprToken
{
	// Variable references.
	EX_LocalVariable = 0x00,	// A local variable.
	EX_InstanceVariable = 0x01,	// An object variable.
	EX_DefaultVariable = 0x02,	//? Default variable for a concrete object.
	EX_StateVariable = 0x03, // State local variable.

		// Tokens.
		EX_Return = 0x04,	// Return from function.
		EX_Switch = 0x05,	// Switch.
		EX_Jump = 0x06,	// Goto a local address in code.
		EX_JumpIfNot = 0x07,	// Goto if not expression.
		EX_Stop = 0x08,	// Stop executing state code.
		EX_Assert = 0x09,	// Assertion.
		EX_Case = 0x0A,	// Case.
		EX_Nothing = 0x0B,	// No operation.
		EX_LabelTable = 0x0C,	// Table of labels.
		EX_GotoLabel = 0x0D,	// Goto a label. 
		EX_EatReturnValue = 0x0E, // destroy an unused return value
		EX_Let = 0x0F,	// Assign an arbitrary size value to a variable.
		EX_DynArrayElement = 0x10, // Dynamic array element.!! 
		EX_New = 0x11, //? New object allocation.
		EX_ClassContext = 0x12, //? Class default metaobject context.
		EX_MetaCast = 0x13, //? Metaclass cast.
		EX_LetBool = 0x14, // Let boolean variable.
		EX_EndParmValue = 0x15,	// end of default value for optional function parameter
		EX_EndFunctionParms = 0x16,	// End of function call parameters.
		EX_Self = 0x17,	// Self object.
		EX_Skip = 0x18,	// Skippable expression.
		EX_Context = 0x19,	// Call a function through an object context.
		EX_ArrayElement = 0x1A,	// Array element.
		EX_VirtualFunction = 0x1B,	// A function call with parameters.
		EX_FinalFunction = 0x1C,	// A prebound function call with parameters.
		EX_IntConst = 0x1D,	// Int constant.
		EX_FloatConst = 0x1E,	// Floating point constant.
		EX_StringConst = 0x1F,	// String constant.
		EX_ObjectConst = 0x20,	// An object constant.
		EX_NameConst = 0x21,	// A name constant.
		EX_RotationConst = 0x22,	// A rotation constant.
		EX_VectorConst = 0x23,	// A vector constant.
		EX_ByteConst = 0x24,	// A byte constant.
		EX_IntZero = 0x25,	// Zero.
		EX_IntOne = 0x26,	// One.
		EX_True = 0x27,	// Bool True.
		EX_False = 0x28,	// Bool False.
		EX_NativeParm = 0x29, // Native function parameter offset.
		EX_NoObject = 0x2A,	// NoObject.

		EX_IntConstByte = 0x2C,	// Int constant that requires 1 byte.
		EX_BoolVariable = 0x2D,	// A bool variable which requires a bitmask.
		EX_DynamicCast = 0x2E,	//? Safe dynamic class casting.
		EX_Iterator = 0x2F, // Begin an iterator operation.
		EX_IteratorPop = 0x30, // Pop an iterator level.
		EX_IteratorNext = 0x31, // Go to next iteration.
		EX_StructCmpEq = 0x32,	// Struct binary compare-for-equal.
		EX_StructCmpNe = 0x33,	// Struct binary compare-for-unequal.
		EX_UnicodeStringConst = 0x34, //? Unicode string constant.
		EX_StructMember = 0x35, // Struct member.
		EX_DynArrayLength = 0x36,	// A dynamic array length for setting/getting
		EX_GlobalFunction = 0x37, // Call non-state version of a function.
		EX_PrimitiveCast = 0x38,	//? A casting operator for primitives which reads the type as the subsequent byte
		EX_DynArrayInsert = 0x39,	// Inserts into a dynamic array
		EX_ReturnNothing = 0x3A, // failsafe for functions that return a value - returns the zero value for a property and logs that control reached the end of a non-void function
		EX_EqualEqual_DelDel = 0x3B,	//? delegate comparison for equality
		EX_NotEqual_DelDel = 0x3C, //? delegate comparison for inequality
		EX_EqualEqual_DelFunc = 0x3D,	//? delegate comparison for equality against a function
		EX_NotEqual_DelFunc = 0x3E,	//? delegate comparison for inequality against a function
		EX_EmptyDelegate = 0x3F,	//? delegate 'None'
		EX_DynArrayRemove = 0x40,	// Removes from a dynamic array
		EX_DebugInfo = 0x41,	//?DEBUGGER Debug information
		EX_DelegateFunction = 0x42, //? Call to a delegate function
		EX_DelegateProperty = 0x43, //? Delegate expression
		EX_LetDelegate = 0x44, //? Assignment to a delegate
		EX_Conditional = 0x45, // tertiary operator support
		EX_DynArrayFind = 0x46, // dynarray search for item index
		EX_DynArrayFindStruct = 0x47, //? dynarray<struct> search for item index
		EX_LocalOutVariable = 0x48, // local out (pass by reference) function parameter
		EX_DefaultParmValue = 0x49,	// default value of optional function parameter
		EX_EmptyParmValue = 0x4A,	// unspecified value for optional function parameter
		EX_InstanceDelegate = 0x4B,	//? const reference to a delegate or normal function object




		EX_InterfaceContext = 0x51,	// Call a function through a native interface variable
		EX_InterfaceCast = 0x52,	// Converting an object reference to native interface variable
		EX_EndOfScript = 0x53, // Last byte in script code
		EX_DynArrayAdd = 0x54, // Add to a dynamic array
		EX_DynArrayAddItem = 0x55, // Add an item to a dynamic array
		EX_DynArrayRemoveItem = 0x56, // Remove an item from a dynamic array
		EX_DynArrayInsertItem = 0x57, // Insert an item into a dynamic array
		EX_DynArrayIterator = 0x58, // Iterate through a dynamic array
		EX_DynArraySort = 0x59,	// Sort a list in place
		EX_JumpIfFilterEditorOnly = 0x5A,	// Skip the code block if the editor is not present.

			// Natives.
			EX_ExtendedNative = 0x60,
			EX_FirstNative = 0x70,
			EX_Max = 0x1000,
};

enum EPropertyType
{
	CPT_None = 0,
	CPT_Byte = 1,
	CPT_Int = 2,
	CPT_Bool = 3,
	CPT_Float = 4,
	CPT_ObjectReference = 5,
	CPT_Name = 6,
	CPT_Delegate = 7,
	CPT_Interface = 8,
	CPT_Range = 9,
	CPT_Struct = 10,
	CPT_Vector = 11,
	CPT_Rotation = 12,
	CPT_String = 13,
	CPT_Map = 14,

	// when you add new property types, make sure you add the corresponding entry
	// in the PropertyTypeToNameMap array in UnScrCom.cpp!!
	CPT_MAX = 15,
};

BYTE GRegisterNative(INT iNative, const Native& Func);

std::vector<BYTE> ScriptSerialize(std::vector<BYTE>& scriptIn, size_t BytecodeSize);

struct FScriptArchive
{
	FScriptArchive(std::vector<BYTE>& scriptIn);

	FScriptArchive& operator<< (BYTE& b);
	FScriptArchive& operator<< (WORD& w);
	FScriptArchive& operator<< (INT& i);
	FScriptArchive& operator<< (FLOAT& f);

	FScriptArchive& operator<< (UObject*& obj);
	FScriptArchive& operator<< (UClass*& cls);
	FScriptArchive& operator<< (UProperty*& prop);
	FScriptArchive& operator<< (UStruct*& st);
	FScriptArchive& operator<< (UField*& field);

	FScriptArchive& operator<< (FName& name);

	std::vector<BYTE> script;

	size_t pos = 0;

	void Serialize(void* ptr, size_t size);
};

class ScriptRuntimeContext
{
public:

	static void LoadFromFile(const std::string& path);
	static ScriptRuntimeContext* Get();

	UObject* IndexToObject(INT index);
	size_t FindIndex(std::string& name);

	template<class T>
	T RunFunction(int index)
	{
		auto obj = IndexToObject(index);
		UFunction* func = static_cast<UFunction*>(obj);
		func->Script = ScriptSerialize(func->ScriptStorage, func->BytecodeScriptSize);

		T result;
		UObject uobject;
		std::vector<BYTE> stackLocal(func->PropertiesSize);
		FFrame stack(&uobject, func, 0, &stackLocal[0]);

		std::array<BYTE, 8> buffer;
		while (*stack.Code != EX_Return)
		{
			stack.Step(stack.Object, &buffer[0]);
		}
		++stack.Code;
		stack.Step(stack.Object, &result);

		return result;
	}

private:
	std::vector<UObject*> m_reflectionInfo;

	static ScriptRuntimeContext* m_spContext;
};
