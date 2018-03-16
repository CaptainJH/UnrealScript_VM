#include <array>
#include <assert.h>
#include <fstream>
#include <algorithm>
#include "UnScript.h"
#include "UObject.h"
#include "Utils.h"

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

FScriptArchive::FScriptArchive(std::vector<BYTE>& scriptIn)
	: script(scriptIn)
	, pos(0)
{}

FScriptArchive& FScriptArchive::operator<<(UObject*& obj)
{
	return *this;
}

FScriptArchive& FScriptArchive::operator<<(UClass*& cls)
{
	return *this;
}

FScriptArchive& FScriptArchive::operator<<(UProperty*& prop)
{
	INT Index;
	FScriptArchive& Ar = *this;
	Ar << Index;

	UObject* Temporary = NULL;
	Temporary = ScriptRuntimeContext::Get()->IndexToObject(Index);
	memcpy(&prop, &Temporary, sizeof(UProperty*));

	return *this;
}

FScriptArchive& FScriptArchive::operator<<(UStruct*& st)
{
	return *this;
}

FScriptArchive& FScriptArchive::operator<<(UField*& field)
{
	return *this;
}

FScriptArchive& FScriptArchive::operator<<(BYTE& b)
{
	Serialize(&b, sizeof(b));
	return *this;
}

FScriptArchive& FScriptArchive::operator<<(WORD& w)
{
	Serialize(&w, sizeof(w));
	return *this;
}

FScriptArchive& FScriptArchive::operator<<(INT& i)
{
	Serialize(&i, sizeof(i));
	return *this;
}

FScriptArchive& FScriptArchive::operator<<(FLOAT& f)
{
	Serialize(&f, sizeof(f));
	return *this;
}

FScriptArchive& FScriptArchive::operator<<(FName& name)
{
	int NameIndex;
	FScriptArchive& Ar = *this;
	Ar << NameIndex;

	{
		INT Number;
		Ar << Number;
		// simply create the name from the NameMap's name index and the serialized instance number
		name = FName(NameIndex);
	}
	return *this;
}

void FScriptArchive::Serialize(void* ptr, size_t size)
{
	memcpy(ptr, &this->script[pos], size);
	pos += size;
}

BYTE* Script = nullptr;
EExprToken SerializeExpr(INT& iCode, FScriptArchive& Ar);
std::vector<BYTE> ScriptSerialize(std::vector<BYTE>& scriptIn, size_t BytecodeSize)
{
	const auto BytecodeStorageSize = scriptIn.size();
	std::vector<BYTE> out(BytecodeSize);
	Script = &out[0];
	FScriptArchive Ar(scriptIn);

	auto iCode = 0;
	while (iCode < BytecodeSize)
	{
		SerializeExpr(iCode, Ar);
	}

	return out;
}


#define XFER(T) { Ar << *(T*)&Script[iCode]; iCode += sizeof(T); }
#define XFERPTR(T) \
	{ \
   	    T AlignedPtr = NULL; \
		ScriptPointerType TempCode; \
		Ar << AlignedPtr; \
		{ \
			TempCode = (ScriptPointerType)(AlignedPtr); \
			memcpy( &Script[iCode], &TempCode, sizeof(ScriptPointerType) ); \
		} \
		iCode += sizeof(ScriptPointerType); \
	}
#define XFER_PROP_POINTER	XFERPTR(UProperty*)
#define XFER_FUNC_POINTER	XFERPTR(UStruct*)
#define XFERNAME() XFER(FName)
#define XFER_FUNC_NAME		XFERNAME()

EExprToken SerializeExpr(INT& iCode, FScriptArchive& Ar)
{
	/** UStruct::SerializeExpr() */
	EExprToken Expr = (EExprToken)0;

	// Get expr token.
	XFER(BYTE);
	Expr = (EExprToken)Script[iCode - 1];
	if (Expr >= EX_FirstNative)
	{
		// Native final function with id 1-127.
		while (SerializeExpr(iCode, Ar) != EX_EndFunctionParms);
		//HANDLE_OPTIONAL_DEBUG_INFO; //DEBUGGER
	}
	else if (Expr >= EX_ExtendedNative)
	{
		// Native final function with id 256-16383.
		XFER(BYTE);
		while (SerializeExpr(iCode, Ar) != EX_EndFunctionParms);
		//HANDLE_OPTIONAL_DEBUG_INFO; //DEBUGGER
	}
	else switch (Expr)
	{
	case EX_PrimitiveCast:
	{
		// A type conversion.
		XFER(BYTE); //which kind of conversion
		SerializeExpr(iCode, Ar);
		break;
	}
	case EX_InterfaceCast:
	{
		// A conversion from an object varible to a native interface variable.  We use a different bytecode to avoid the branching each time we process a cast token
		XFERPTR(UClass*);	// the interface class to convert to
		SerializeExpr(iCode, Ar);
		break;
	}
	case EX_Let:
	case EX_LetBool:
	case EX_LetDelegate:
	{
		SerializeExpr(iCode, Ar); // Variable expr.
		SerializeExpr(iCode, Ar); // Assignment expr.
		break;
	}
	case EX_Jump:
	{
		XFER(CodeSkipSizeType); // Code offset.
		break;
	}
	case EX_LocalVariable:
	case EX_InstanceVariable:
	case EX_DefaultVariable:
	case EX_LocalOutVariable:
	case EX_StateVariable:
	{
		XFER_PROP_POINTER;
		break;
	}
	case EX_DebugInfo:
	{
		XFER(INT);	// Version
		XFER(INT);	// Line number
		XFER(INT);	// Character pos
		XFER(BYTE); // OpCode
		break;
	}
	case EX_BoolVariable:
	case EX_InterfaceContext:
	{
		SerializeExpr(iCode, Ar);
		break;
	}
	case EX_Nothing:
	case EX_EndOfScript:
	case EX_EndFunctionParms:
	case EX_EmptyParmValue:
	case EX_IntZero:
	case EX_IntOne:
	case EX_True:
	case EX_False:
	case EX_NoObject:
	case EX_EmptyDelegate:
	case EX_Self:
	case EX_IteratorPop:
	case EX_Stop:
	case EX_IteratorNext:
	case EX_EndParmValue:
	{
		break;
	}
	case EX_ReturnNothing:
	{
		XFER_PROP_POINTER; // the return value property
		break;
	}
	case EX_EatReturnValue:
	{
		XFER_PROP_POINTER; // the return value property
		break;
	}
	case EX_Return:
	{
		SerializeExpr(iCode, Ar); // Return expression.
		break;
	}
	case EX_FinalFunction:
	{
		XFER_FUNC_POINTER;											// Stack node.
		while (SerializeExpr(iCode, Ar) != EX_EndFunctionParms); // Parms.
		//HANDLE_OPTIONAL_DEBUG_INFO;									// DEBUGGER
		break;
	}
	case EX_VirtualFunction:
	case EX_GlobalFunction:
	{
		XFER_FUNC_NAME;												// Virtual function name.
		while (SerializeExpr(iCode, Ar) != EX_EndFunctionParms);	// Parms.
		//HANDLE_OPTIONAL_DEBUG_INFO;									//DEBUGGER
		break;
	}
	case EX_DelegateFunction:
	{
		XFER(BYTE);													// local prop
		XFER_PROP_POINTER;											// Delegate property
		//XFERNAME();												// Delegate function name (in case the delegate is NULL)
		assert(false);
		break;
	}
	case EX_NativeParm:
	{
		XFERPTR(UProperty*);
		break;
	}
	case EX_ClassContext:
	case EX_Context:
	{
		SerializeExpr(iCode, Ar); // Object expression.
		XFER(CodeSkipSizeType);		// Code offset for NULL expressions.
		XFERPTR(UField*);			// Property corresponding to the r-value data, in case the l-value needs to be mem-zero'd
		XFER(BYTE);					// Property type, in case the r-value is a non-property such as dynamic array length
		SerializeExpr(iCode, Ar); // Context expression.
		break;
	}
	case EX_DynArrayIterator:
	{
		SerializeExpr(iCode, Ar); // Array expression
		SerializeExpr(iCode, Ar); // Array item expression
		XFER(BYTE);					// Index parm present
		SerializeExpr(iCode, Ar); // Index parm
		XFER(CodeSkipSizeType);		// Code offset
		break;
	}
	case EX_ArrayElement:
	case EX_DynArrayElement:
	{
		SerializeExpr(iCode, Ar); // Index expression.
		SerializeExpr(iCode, Ar); // Base expression.
		break;
	}
	case EX_DynArrayLength:
	{
		SerializeExpr(iCode, Ar); // Base expression.
		break;
	}
	case EX_DynArrayAdd:
	{
		SerializeExpr(iCode, Ar); // Base expression
		SerializeExpr(iCode, Ar);	// Count
		SerializeExpr(iCode, Ar); // EX_EndFunctionParms
		//HANDLE_OPTIONAL_DEBUG_INFO;									// DEBUGGER
		break;
	}
	case EX_DynArrayInsert:
	case EX_DynArrayRemove:
	{
		SerializeExpr(iCode, Ar); // Base expression
		SerializeExpr(iCode, Ar); // Index
		SerializeExpr(iCode, Ar); // Count
		SerializeExpr(iCode, Ar); // EX_EndFunctionParms
		//HANDLE_OPTIONAL_DEBUG_INFO;									// DEBUGGER
		break;
	}
	case EX_DynArrayAddItem:
	case EX_DynArrayRemoveItem:
	{
		SerializeExpr(iCode, Ar); // Array property expression
		XFER(CodeSkipSizeType);		// Number of bytes to skip if NULL context encountered
		SerializeExpr(iCode, Ar); // Item
		SerializeExpr(iCode, Ar); // EX_EndFunctionParms
		//HANDLE_OPTIONAL_DEBUG_INFO;									// DEBUGGER
		break;
	}
	case EX_DynArrayInsertItem:
	{
		SerializeExpr(iCode, Ar);	// Base expression
		XFER(CodeSkipSizeType);		// Number of bytes to skip if NULL context encountered
		SerializeExpr(iCode, Ar); // Index
		SerializeExpr(iCode, Ar);	// Item
		SerializeExpr(iCode, Ar); // EX_EndFunctionParms
		//HANDLE_OPTIONAL_DEBUG_INFO;									// DEBUGGER
		break;
	}
	case EX_DynArrayFind:
	{
		SerializeExpr(iCode, Ar); // Array property expression
		XFER(CodeSkipSizeType);		// Number of bytes to skip if NULL context encountered
		SerializeExpr(iCode, Ar); // Search item
		SerializeExpr(iCode, Ar); // EX_EndFunctionParms
		//HANDLE_OPTIONAL_DEBUG_INFO;									// DEBUGGER
		break;
	}
	case EX_DynArrayFindStruct:
	{
		SerializeExpr(iCode, Ar); // Array property expression
		XFER(CodeSkipSizeType);		// Number of bytes to skip if NULL context encountered
		SerializeExpr(iCode, Ar);	// Property name
		SerializeExpr(iCode, Ar); // Search item
		SerializeExpr(iCode, Ar); // EX_EndFunctionParms
		//HANDLE_OPTIONAL_DEBUG_INFO;									// DEBUGGER
		break;
	}
	case EX_DynArraySort:
	{
		SerializeExpr(iCode, Ar);	// Array property
		XFER(CodeSkipSizeType);		// Number of bytes to skip if NULL context encountered
		SerializeExpr(iCode, Ar);	// Sort compare delegate
		SerializeExpr(iCode, Ar); // EX_EndFunctionParms
		//HANDLE_OPTIONAL_DEBUG_INFO;									// DEBUGGER
		break;
	}
	case EX_Conditional:
	{
		SerializeExpr(iCode, Ar); // Bool Expr
		XFER(CodeSkipSizeType);		// Skip
		SerializeExpr(iCode, Ar); // Result Expr 1
		XFER(CodeSkipSizeType);		// Skip2
		SerializeExpr(iCode, Ar); // Result Expr 2
		break;
	}
	case EX_New:
	{
		SerializeExpr(iCode, Ar); // Parent expression.
		SerializeExpr(iCode, Ar); // Name expression.
		SerializeExpr(iCode, Ar); // Flags expression.
		SerializeExpr(iCode, Ar); // Class expression.
		break;
	}
	case EX_IntConst:
	{
		XFER(INT);
		break;
	}
	case EX_FloatConst:
	{
		XFER(FLOAT);
		break;
	}
	case EX_StringConst:
	{
		do XFER(BYTE) while (Script[iCode - 1]);
		break;
	}
	case EX_UnicodeStringConst:
	{
		do XFER(WORD) while (Script[iCode - 1] || Script[iCode - 2]);
		break;
	}
	case EX_ObjectConst:
	{
		assert(false);
		//XFER_OBJECT_POINTER(UObject*);
		break;
	}
	case EX_NameConst:
	{
		assert(false);
		//XFERNAME();
		break;
	}
	case EX_RotationConst:
	{
		XFER(INT); XFER(INT); XFER(INT);
		break;
	}
	case EX_VectorConst:
	{
		XFER(FLOAT); XFER(FLOAT); XFER(FLOAT);
		break;
	}
	case EX_ByteConst:
	case EX_IntConstByte:
	{
		XFER(BYTE);
		break;
	}
	case EX_MetaCast:
	{
		//XFER_OBJECT_POINTER(UClass*);
		SerializeExpr(iCode, Ar);
		break;
	}
	case EX_DynamicCast:
	{
		//XFER_OBJECT_POINTER(UClass*);
		SerializeExpr(iCode, Ar);
		break;
	}
	case EX_JumpIfNot:
	{
		XFER(CodeSkipSizeType);		// Code offset.
		SerializeExpr(iCode, Ar); // Boolean expr.
		break;
	}
	case EX_Iterator:
	{
		SerializeExpr(iCode, Ar); // Iterator expr.
		XFER(CodeSkipSizeType);		// Code offset.
		break;
	}
	case EX_Switch:
	{
		XFER_PROP_POINTER;			// Size of property value, in case null context is encountered.
		XFER(BYTE);					// Property type, in case the r-value is a non-property such as dynamic array length
		SerializeExpr(iCode, Ar); // Switch expr.
		break;
	}
	case EX_Assert:
	{
		XFER(WORD); // Line number.
		XFER(BYTE); // debug mode or not
		SerializeExpr(iCode, Ar); // Assert expr.
		break;
	}
	case EX_Case:
	{
		CodeSkipSizeType W;
		XFER(CodeSkipSizeType);			// Code offset.
		memcpy(&W, &Script[iCode - sizeof(CodeSkipSizeType)], sizeof(CodeSkipSizeType));
		if (W != MAXWORD)
		{
			SerializeExpr(iCode, Ar); // Boolean expr.
		}
		break;
	}
	case EX_LabelTable:
	{
		assert((iCode & 3) == 0);

		assert(false);
		//XFER_LABELTABLE
			break;
	}
	case EX_GotoLabel:
	{
		SerializeExpr(iCode, Ar); // Label name expr.
		break;
	}
	case EX_Skip:
	{
		XFER(CodeSkipSizeType);		// Skip size.
		SerializeExpr(iCode, Ar); // Expression to possibly skip.
		break;
	}
	case EX_DefaultParmValue:
	{
		XFER(CodeSkipSizeType);		// Size of the expression for this default parameter - used by the VM to skip over the expression
									// if a value was specified in the function call

		//HANDLE_OPTIONAL_DEBUG_INFO;	// DI_NewStack
		SerializeExpr(iCode, Ar); // Expression for this default parameter value
		//HANDLE_OPTIONAL_DEBUG_INFO;	// DI_PrevStack
		XFER(BYTE);					// EX_EndParmValue
		break;
	}
	case EX_StructCmpEq:
	case EX_StructCmpNe:
	{
		XFERPTR(UStruct*);			// Struct.
		SerializeExpr(iCode, Ar); // Left expr.
		SerializeExpr(iCode, Ar); // Right expr.
		break;
	}
	case EX_StructMember:
	{
		XFER_PROP_POINTER;			// the struct property we're accessing
		XFERPTR(UStruct*);			// the struct which contains the property
		XFER(BYTE);					// byte indicating whether a local copy of the struct must be created in order to access the member property
		XFER(BYTE);					// byte indicating whether the struct member will be modified by the expression it's being used in
		SerializeExpr(iCode, Ar); // expression corresponding to the struct member property.
		break;
	}
	case EX_DelegateProperty:
	{
		assert(false);
		//XFER_FUNC_NAME;				// Name of function we're assigning to the delegate.
		XFER_PROP_POINTER;			// delegate property corresponding to the function we're assigning (if any)
		break;
	}
	case EX_InstanceDelegate:
	{
		assert(false);
		//XFER_FUNC_NAME;				// the name of the function assigned to the delegate.
		break;
	}
	case EX_EqualEqual_DelDel:
	case EX_NotEqual_DelDel:
	case EX_EqualEqual_DelFunc:
	case EX_NotEqual_DelFunc:
	{
		while (SerializeExpr(iCode, Ar) != EX_EndFunctionParms);
		//HANDLE_OPTIONAL_DEBUG_INFO; //DEBUGGER
		break;
	}
	case EX_JumpIfFilterEditorOnly:
	{
		// Code offset.
		WORD& JumpIdx = (WORD&)Script[iCode];
		Ar << JumpIdx;
		iCode += sizeof(WORD);

		break;
	}
	default:
	{
		// This should never occur.
		assert(false);
		break;
	}
	}

	return Expr;
}

ScriptRuntimeContext* ScriptRuntimeContext::m_spContext = nullptr;
ScriptRuntimeContext* ScriptRuntimeContext::Get()
{
	return m_spContext;
}

UObject* ScriptRuntimeContext::IndexToObject(INT index)
{
	return m_reflectionInfo[index - 1];
}

size_t ScriptRuntimeContext::FindIndex(std::string& name)
{
	auto it = std::find_if(m_reflectionInfo.begin(), m_reflectionInfo.end(), [name](UObject* obj) {
		return name == obj->Name;
	});

	if (it == m_reflectionInfo.end())
		return -1;
	else
		return it - m_reflectionInfo.begin() + 1;
}

void ScriptRuntimeContext::LoadFromFile(const std::string& path)
{
	const char sep = '|';
	m_spContext = new ScriptRuntimeContext;

	std::ifstream ifile(path);
	std::string line;
	while (std::getline(ifile, line))
	{
		size_t begin = 0;
		auto str0 = StringSlice(line, sep, begin);
		auto str1 = StringSlice(line, sep, begin);

		UObject* obj = nullptr;

		if (str0 == "IntProperty")
		{
			obj = new UProperty;
			obj->Name = str1;

			auto str2 = StringSlice(line, sep, begin);
			auto str3 = StringSlice(line, sep, begin);
			auto str4 = StringSlice(line, sep, begin);

			UProperty* prop = static_cast<UProperty*>(obj);
			prop->ArrayDim = std::stoi(str2);
			prop->ElementSize = std::stoi(str3);
			prop->Offset = std::stoi(str4);

		}
		else if (str0 == "Function")
		{
			obj = new UFunction;
			obj->Name = str1;

			UFunction* func = static_cast<UFunction*>(obj);
			auto str2 = StringSlice(line, sep, begin);
			auto str3 = StringSlice(line, sep, begin);
			auto str4 = StringSlice(line, sep, begin);
			auto str5 = StringSlice(line, sep, begin);

			func->PropertiesSize = stoi(str2);
			func->BytecodeScriptSize = stoi(str3);

			const char sepSpace = ' ';
			size_t p = 0;
			while (p != str5.length())
			{
				auto s = StringSlice(str5, sepSpace, p);
				func->ScriptStorage.push_back((BYTE)strtol(s.c_str(), NULL, 16));
			}
			
		}
		else
		{
			obj = new UObject;
			obj->Name = str1;
		}

		m_spContext->m_reflectionInfo.push_back(obj);
	}

	// second pass
	ifile.close();
	ifile.open(path);
	int index = 0;
	while (std::getline(ifile, line))
	{
		size_t begin = 0;
		auto str0 = StringSlice(line, sep, begin);
		auto str1 = StringSlice(line, sep, begin);

		UObject* obj = m_spContext->m_reflectionInfo[index++];

		if (str0 == "IntProperty")
		{
			auto str2 = StringSlice(line, sep, begin);
			auto str3 = StringSlice(line, sep, begin);
			auto str4 = StringSlice(line, sep, begin);
			auto next = StringSlice(line, sep, begin);

			UObject* nextPtr = stoi(next) >= 0 ? m_spContext->m_reflectionInfo[stoi(next)] : nullptr;

			UProperty* prop = static_cast<UProperty*>(obj);
			prop->Next = (UField*)nextPtr;

		}
		else if (str0 == "Function")
		{
			UFunction* func = static_cast<UFunction*>(obj);
			auto str2 = StringSlice(line, sep, begin);
			auto str3 = StringSlice(line, sep, begin);
			auto str4 = StringSlice(line, sep, begin);
			auto str5 = StringSlice(line, sep, begin);
			auto children = StringSlice(line, sep, begin);

			UObject* nextPtr = stoi(children) >= 0 ? m_spContext->m_reflectionInfo[stoi(children)] : nullptr;

			func->Children = (UStruct*)nextPtr;

		}
	}
}

void ScriptRuntimeContext::RunFunction(int index)
{
	auto obj = IndexToObject(index);
	UFunction* func = static_cast<UFunction*>(obj);
	func->Script = ScriptSerialize(func->ScriptStorage, func->BytecodeScriptSize);

	int result = 0;
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
}