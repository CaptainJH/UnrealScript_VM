#pragma once
#include <map>
#include "Types.h"
#include "FArray.h"
#include "ReflectionInfo.h"
#include "UnScript.h"


/*-----------------------------------------------------------------------------
FPropertyBase.
-----------------------------------------------------------------------------*/

enum EPropertyReferenceType
{
	/** no reference */
	CPRT_None = 0,

	/** we're referencing this property in order to assign a value to it */
	CPRT_AssignValue = 1,

	/** we're referencing this property's value in such a way that doesn't require it be initialized (if checks, iterators, etc.) */
	CPRT_SimpleReference = 2,

	/** we're referecning this property's value in such a away that requires it to be initialized (assigning to another property, using in a function call, etc) */
	CPRT_AssignmentReference = 3,

	/** we're referencing this property in a way that both changes its value and references the value (combination of CPRT_AssignmentReference and CPRT_AssignValue) */
	CPRT_DualReference = 4,
};

enum EFunctionExportFlags
{
	FUNCEXPORT_Virtual = 0x00000001,	// function should be exported as a virtual function
	FUNCEXPORT_Final = 0x00000002,	// function declaration included "final" keyword.  Used to differentiate between functions that have FUNC_Final only because they're private
	FUNCEXPORT_NoExport = 0x00000004,	// only DECLARE_FUNCTION stub should be exported to header file
	FUNCEXPORT_Const = 0x00000008,	// export C++ declaration as const
	FUNCEXPORT_NoExportHeader = 0x00000010,	// don't export the C++ declaration, only the DECLARE_FUNCTION wrapper
};

enum EPropertyHeaderExportFlags
{
	PROPEXPORT_Public = 0x00000001,	// property should be exported as public
	PROPEXPORT_Private = 0x00000002,	// property should be exported as private
	PROPEXPORT_Protected = 0x00000004,	// property should be exported as protected
};

/**
* Basic information describing a type.
*/
class FPropertyBase
{
public:
	// Variables.
	EPropertyType Type;
	INT ArrayDim;
	QWORD PropertyFlags;

	/**
	* A mask of EPropertyHeaderExportFlags which are used for modifying how this property is exported to the native class header
	*/
	DWORD PropertyExportFlags;
	union
	{
		class UEnum* Enum;
		class UClass* PropertyClass;
		class UScriptStruct* Struct;
		class UFunction* Function;
#if _WIN64
		QWORD BitMask;
		SQWORD StringSize;
#else
		DWORD BitMask;
		INT StringSize;
#endif
	};

	UClass*	MetaClass;

	/**
	* For static array properties which use an enum value to specify the array size, corresponds to the UEnum that was used for the size.
	*/
	UEnum*	ArrayIndexEnum;

	FName	DelegateName;

	/** Raw string (not type-checked) used for specifying special text when exporting a property to the *Classes.h file */
	FString	ExportInfo;

	/** Map of key value pairs that will be added to the package's UMetaData for this property */
	std::map<FName, FString> MetaData;

	EPropertyReferenceType ReferenceType;

	/** @name Constructors */
	//@{
	FPropertyBase(EPropertyType InType, EPropertyReferenceType InRefType = CPRT_None)
		: Type(InType), ArrayDim(1), PropertyFlags(0), PropertyExportFlags(PROPEXPORT_Public)
		, BitMask(0), MetaClass(NULL), ArrayIndexEnum(NULL), DelegateName(NAME_None), ReferenceType(InRefType)
	{}
	FPropertyBase(UEnum* InEnum, EPropertyReferenceType InRefType = CPRT_None)
		: Type(CPT_Byte), ArrayDim(1), PropertyFlags(0), PropertyExportFlags(PROPEXPORT_Public)
		, Enum(InEnum), MetaClass(NULL), ArrayIndexEnum(NULL), DelegateName(NAME_None), ReferenceType(InRefType)
	{}
	FPropertyBase(UClass* InClass, UClass* InMetaClass = NULL, EPropertyReferenceType InRefType = CPRT_None)
		: Type(CPT_ObjectReference), ArrayDim(1), PropertyFlags(0), PropertyExportFlags(PROPEXPORT_Public)
		, PropertyClass(InClass), MetaClass(InMetaClass), ArrayIndexEnum(NULL), DelegateName(NAME_None), ReferenceType(InRefType)
	{
		// if this is an interface class, we use the UInterfaceProperty class instead of UObjectProperty
		//if (InClass->HasAnyClassFlags(CLASS_Interface))
		//{
		//	Type = CPT_Interface;
		//}
	}
	FPropertyBase(UScriptStruct* InStruct, EPropertyReferenceType InRefType = CPRT_None)
		: Type(CPT_Struct), ArrayDim(1), PropertyFlags(0), PropertyExportFlags(PROPEXPORT_Public)
		, Struct(InStruct), MetaClass(NULL), ArrayIndexEnum(NULL), DelegateName(NAME_None), ReferenceType(InRefType)
	{}
	FPropertyBase(UProperty* Property, EPropertyReferenceType InRefType = CPRT_None)
		: PropertyExportFlags(PROPEXPORT_Public), DelegateName(NAME_None)
	{
		assert(Property);

		UBOOL DynArray = 0;
		QWORD PropagateFlags = 0;
		if (Property->GetClass() == UArrayProperty::StaticClass())
		{
			DynArray = 1;
			// if we're an array, save up Parm flags so we can propagate them.
			// below the array will be assigned the inner property flags. This allows propagation of Parm flags (out, optional..)
			//@note: we need to explicitly specify CPF_Const instead of adding it to CPF_ParmFlags because CPF_ParmFlags is treated as exclusive;
			// i.e., flags that are in CPF_ParmFlags are not allowed in other variable types and vice versa
			PropagateFlags = Property->PropertyFlags & (CPF_ParmFlags | CPF_Const);
			Property = dynamic_cast<UArrayProperty*>(Property)->Inner;
		}
		if (Property->GetClass() == UByteProperty::StaticClass())
		{
			*this = FPropertyBase(CPT_Byte);
			Enum = dynamic_cast<UByteProperty*>(Property)->Enum;
		}
		else if (Property->GetClass() == UIntProperty::StaticClass())
		{
			*this = FPropertyBase(CPT_Int);
		}
		else if (Property->GetClass() == UBoolProperty::StaticClass())
		{
			*this = FPropertyBase(CPT_Bool);
			BitMask = dynamic_cast<UBoolProperty*>(Property)->BitMask;
		}
		else if (Property->GetClass() == UFloatProperty::StaticClass())
		{
			*this = FPropertyBase(CPT_Float);
		}
		else if (Property->GetClass() == UClassProperty::StaticClass())
		{
			*this = FPropertyBase(CPT_ObjectReference);
			PropertyClass = dynamic_cast<UClassProperty*>(Property)->PropertyClass;
			MetaClass = dynamic_cast<UClassProperty*>(Property)->MetaClass;
		}
		//else if (Property->GetClass() == UComponentProperty::StaticClass())
		//{
		//	*this = FPropertyBase(CPT_ObjectReference);
		//	PropertyClass = dynamic_cast<UComponentProperty*>(Property)->PropertyClass;
		//}
		else if (Property->GetClass() == UObjectProperty::StaticClass())
		{
			*this = FPropertyBase(CPT_ObjectReference);
			PropertyClass = dynamic_cast<UObjectProperty*>(Property)->PropertyClass;
		}
		else if (Property->GetClass() == UNameProperty::StaticClass())
		{
			*this = FPropertyBase(CPT_Name);
		}
		else if (Property->GetClass() == UStrProperty::StaticClass())
		{
			*this = FPropertyBase(CPT_String);
		}
		//else if (Property->GetClass() == UMapProperty::StaticClass())
		//{
		//	*this = FPropertyBase(CPT_Map);
		//}
		else if (Property->GetClass() == UStructProperty::StaticClass())
		{
			*this = FPropertyBase(CPT_Struct);
			Struct = dynamic_cast<UStructProperty*>(Property)->Struct;
		}
		//else if (Property->GetClass() == UDelegateProperty::StaticClass())
		//{
		//	*this = FPropertyBase(CPT_Delegate);
		//	Function = dynamic_cast<UDelegateProperty*>(Property)->Function;
		//}
		//else if (Property->GetClass() == UInterfaceProperty::StaticClass())
		//{
		//	*this = FPropertyBase(CPT_Interface);
		//	PropertyClass = dynamic_cast<UInterfaceProperty*>(Property)->InterfaceClass;
		//}
		else
		{
			//appErrorf(TEXT("Unknown property type '%s'"), *Property->GetFullName());
		}
		ArrayDim = DynArray ? 0 : Property->ArrayDim;
		PropertyFlags = Property->PropertyFlags | PropagateFlags;
		ReferenceType = InRefType;
		ArrayIndexEnum = Property->ArraySizeEnum;
	}
	//@}

	/** @name Functions */
	//@{
	INT GetSize() const //hardcoded sizes!!
	{
		static const INT ElementSizes[CPT_MAX] =
		{
			0 /*None*/,					sizeof(BYTE),		sizeof(INT),			sizeof(UBOOL),
			sizeof(FLOAT),				sizeof(UObject*),	sizeof(FName),			/*sizeof(FScriptDelegate)*/-1,
			/*sizeof(FScriptInterface)*/-1,	0 /*Array*/,		0 /*Struct*/,			0 /*Vector*/,
			0 /*Rotator*/,				0 /*Map*/,			sizeof(std::map<BYTE,BYTE>)
		};

		INT ElementSize;
		if (Type == CPT_Struct)
		{
			if (Struct->GetFName() == NAME_Pointer)
			{
				ElementSize = sizeof(void*);
			}
			else
			{
				ElementSize = Struct->GetPropertiesSize();
			}
		}
		else
		{
			ElementSize = ElementSizes[Type];
		}

		return ElementSize * ArrayDim;
	}

	/**
	* Returns whether this token represents a vector
	*
	* @return	TRUE if this token represents a vector
	*/
	UBOOL IsVector() const
	{
		return Type == CPT_Struct && Struct->GetFName() == NAME_Vector;
	}

	/**
	* Returns whether this token represents a rotator
	*
	* @return	TRUE if this token represents a rotator
	*/
	UBOOL IsRotator() const
	{
		return Type == CPT_Struct && Struct->GetFName() == NAME_Rotator;
	}

	/**
	* Returns whether this token represents a pointer
	*
	* @return	TRUE if this token represents a pointer
	*/
	UBOOL IsPointer() const
	{
		return Type == CPT_Struct && Struct->GetFName() == NAME_Pointer;
	}

	/**
	* Returns whether this token represents an object reference
	*/
	UBOOL IsObject() const
	{
		return Type == CPT_ObjectReference || Type == CPT_Interface;
	}

	/**
	* Determines whether this token represents a dynamic array.
	*/
	UBOOL IsDynamicArray() const
	{
		return ArrayDim == 0;
	}

	/**
	* Determines whether this token represents an enum property
	*/
	UBOOL IsEnumVariable() const
	{
		return Type == CPT_Byte && Enum != NULL;
	}

	/**
	* Determines whether this token's type is compatible with another token's type.
	*
	* @param	Other							the token to check against this one.
	*											Given the following example expressions, VarA is Other and VarB is 'this':
	*												VarA = VarB;
	*
	*												function func(type VarB) {}
	*												func(VarA);
	*
	*												static operator==(type VarB_1, type VarB_2) {}
	*												if ( VarA_1 == VarA_2 ) {}
	*
	* @param	bDisallowGeneralization			controls whether it should be considered a match if this token's type is a generalization
	*											of the other token's type (or vice versa, when dealing with structs
	* @param	bIgnoreImplementedInterfaces	controls whether two types can be considered a match if one type is an interface implemented
	*											by the other type.
	*/
	UBOOL MatchesType(const FPropertyBase& Other, UBOOL bDisallowGeneralization, UBOOL bIgnoreImplementedInterfaces = FALSE) const
	{
		assert(Type != CPT_None || !bDisallowGeneralization);

		UBOOL bIsObjectType = IsObject();
		UBOOL bOtherIsObjectType = Other.IsObject();
		UBOOL bIsObjectComparison = bIsObjectType && bOtherIsObjectType;
		UBOOL bReverseClassChainCheck = TRUE;

		// If converting to an l-value, we require an exact match with an l-value.
		if ((PropertyFlags&CPF_OutParm) != 0)
		{
			// if the other type is not an l-value, disallow
			if ((Other.PropertyFlags&CPF_OutParm) == 0)
			{
				return FALSE;
			}

			// if the other type is const and we are not const, disallow
			if ((Other.PropertyFlags&CPF_Const) != 0 && (PropertyFlags&CPF_Const) == 0)
			{
				return FALSE;
			}

			if (Type == CPT_Struct)
			{
				// Allow derived structs to be passed by reference, unless this is a dynamic array of structs
				bDisallowGeneralization = bDisallowGeneralization || IsDynamicArray() || Other.IsDynamicArray();
			}

			// if Type == CPT_ObjectReference, out object function parm; allow derived classes to be passed in
			// if Type == CPT_Interface, out interface function parm; allow derived classes to be passed in
			else if ((PropertyFlags & CPF_Const) == 0 || !IsObject())
			{
				// all other variable types must match exactly when passed as the value to an 'out' parameter
				bDisallowGeneralization = TRUE;
			}

			// both types are objects, but one is an interface and one is an object reference
			else if (bIsObjectComparison && Type != Other.Type)
			{
				return FALSE;
			}
		}
		else if (Type == CPT_ObjectReference && Other.Type != CPT_Interface
			&& (ReferenceType == CPRT_AssignmentReference || ReferenceType == CPRT_DualReference || (PropertyFlags & CPF_ReturnParm)))
		{
			bReverseClassChainCheck = FALSE;
		}

		// Check everything.
		if (Type == CPT_None && (Other.Type == CPT_None || !bDisallowGeneralization))
		{
			// If Other has no type, accept anything.
			return TRUE;
		}
		else if (Type != Other.Type && !bIsObjectComparison)
		{
			// Mismatched base types.
			return FALSE;
		}
		else if (ArrayDim != Other.ArrayDim)
		{
			// Mismatched array dimensions.
			return FALSE;
		}
		else if (Type == CPT_Byte)
		{
			// Make sure enums match, or we're generalizing.
			return Enum == Other.Enum || (Enum == NULL && !bDisallowGeneralization);
		}
		else if (bIsObjectType)
		{
			assert(PropertyClass != NULL);

			// Make sure object types match, or we're generalizing.
			if (bDisallowGeneralization)
			{
				// Exact match required.
				return PropertyClass == Other.PropertyClass && MetaClass == Other.MetaClass;
			}
			else if (Other.PropertyClass == NULL)
			{
				// Cannonical 'None' matches all object classes.
				return TRUE;
			}
			else
			{
				// Generalization is ok (typical example of this check would look like: VarA = VarB;, where this is VarB and Other is VarA)
				if (Other.PropertyClass->IsChildOf(PropertyClass))
				{
					if (!bIgnoreImplementedInterfaces || ((Type == CPT_Interface) == (Other.Type == CPT_Interface)))
					{
						if (PropertyClass != UClass::StaticClass() || MetaClass == NULL || Other.MetaClass->IsChildOf(MetaClass) ||
							(bReverseClassChainCheck && (Other.MetaClass == NULL || MetaClass->IsChildOf(Other.MetaClass))))
						{
							return TRUE;
						}
					}
				}
				// check the opposite class chain for object types
				else if (bReverseClassChainCheck && Type != CPT_Interface && bIsObjectComparison && PropertyClass != NULL && PropertyClass->IsChildOf(Other.PropertyClass))
				{
					if (Other.PropertyClass != UClass::StaticClass() || MetaClass == NULL || Other.MetaClass == NULL || MetaClass->IsChildOf(Other.MetaClass) || Other.MetaClass->IsChildOf(MetaClass))
					{
						return TRUE;
					}
				}

				if (PropertyClass->HasAnyClassFlags(CLASS_Interface) && !bIgnoreImplementedInterfaces)
				{
					if (Other.PropertyClass->ImplementsInterface(PropertyClass))
					{
						return TRUE;
					}
				}

				return FALSE;
			}
		}
		else if (Type == CPT_Struct)
		{
			assert(Struct != NULL);
			assert(Other.Struct != NULL);

			if (Struct == Other.Struct)
			{
				// struct types match exactly 
				return TRUE;
			}

			// returning FALSE here prevents structs related through inheritance from being used interchangeably, such as passing a derived struct as the value for a parameter
			// that expects the base struct, or vice versa.  An easier example is assignment (e.g. Vector = Plane or Plane = Vector).
			// there are two cases to consider (let's use vector and plane for the example):
			// - Vector = Plane;
			//		in this expression, 'this' is the vector, and Other is the plane.  This is an unsafe conversion, as the destination property type is used to copy the r-value to the l-value
			//		so in this case, the VM would call CopyCompleteValue on the FPlane struct, which would copy 16 bytes into the l-value's buffer;  However, the l-value buffer will only be
			//		12 bytes because that is the size of FVector
			// - Plane = Vector;
			//		in this expression, 'this' is the plane, and Other is the vector.  This is a safe conversion, since only 12 bytes would be copied from the r-value into the l-value's buffer
			//		(which would be 16 bytes).  The problem with allowing this conversion is that what to do with the extra member (e.g. Plane.W); should it be left alone? should it be zeroed?
			//		difficult to say what the correct behavior should be, so let's just ignore inheritance for the sake of determining whether two structs are identical

			// Previously, the logic for determining whether this is a generalization of Other was reversed; this is very likely the culprit behind all current issues with 
			// using derived structs interchangeably with their base versions.  The inheritance check has been fixed; for now, allow struct generalization and see if we can find any further
			// issues with allowing conversion.  If so, then we disable all struct generalization by returning FALSE here.
			// return FALSE;

			if (bDisallowGeneralization)
			{
				return FALSE;
			}
			// if we don't need an exact match, see if we're generalizing.
			else
			{
				// Generalization is ok if this is not a dynamic array
				if (!IsDynamicArray() && !Other.IsDynamicArray())
				{
					if (Other.Struct->IsChildOf(Struct))
					{
						// this is the old behavior - it would allow memory overwrites if you assigned a derived value to a base variable; e.g. Vector = Plane;
						// 						debugfSuppressed(NAME_DevCompile, TEXT("FPropertyBase::MatchesType PREVENTING match - Src:%s Destination:%s"), *Other.Struct->GetPathName(), *Struct->GetPathName());
					}
					else if (Struct->IsChildOf(Other.Struct))
					{
						// 						debugfSuppressed(NAME_DevCompile, TEXT("FPropertyBase::MatchesType ALLOWING match - Src:%s Destination:%s"), *Other.Struct->GetPathName(), *Struct->GetPathName());
						return TRUE;
					}
				}

				return FALSE;
			}
		}
		else
		{
			// General match.
			return TRUE;
		}
	}

	//FString Describe()
	//{
	//	return FString::Printf(
	//		TEXT("Type:%s  Flags:%lli  Enum:%s  PropertyClass:%s  Struct:%s  Function:%s  MetaClass:%s"),
	//		GetPropertyTypeText(Type), PropertyFlags,
	//		Enum != NULL ? *Enum->GetName() : TEXT(""),
	//		PropertyClass != NULL ? *PropertyClass->GetName() : TEXT("NULL"),
	//		Struct != NULL ? *Struct->GetName() : TEXT("NULL"),
	//		Function != NULL ? *Function->GetName() : TEXT("NULL"),
	//		MetaClass != NULL ? *MetaClass->GetName() : TEXT("NULL")
	//	);
	//}
	//@}
};

/*-----------------------------------------------------------------------------
FToken.
-----------------------------------------------------------------------------*/
//
// Information about a token that was just parsed.
//
class FToken : public FPropertyBase
{
public:
	/** @name Variables */
	//@{
	/** Type of token. */
	ETokenType				TokenType;
	/** Name of token. */
	FName					TokenName;
	/** Starting position in script where this token came from. */
	INT						StartPos;
	/** Starting line in script. */
	INT						StartLine;
	/** Always valid. */
	TCHAR					Identifier[NAME_SIZE];
	/** property that corresponds to this FToken - null if this Token doesn't correspond to a UProperty */
	UProperty*				TokenProperty;
	/** function that corresponds to this FToken - null if this Token doesn't correspond to a function */
	class FFunctionData*	TokenFunction;
	union
	{
		// TOKEN_Const values.
		BYTE	Byte;								 // If CPT_Byte.
		INT		Int;								 // If CPT_Int.
		UBOOL	Bool;								 // If CPT_Bool.
		FLOAT	Float;								 // If CPT_Float.
		UObject* Object;							 // If CPT_ObjectReference or CPT_Interface.
		BYTE	NameBytes[sizeof(FName)];			 // If CPT_Name.
		TCHAR	String[MAX_STRING_CONST_SIZE];		 // If CPT_String or IsPointer().
		BYTE	VectorBytes[sizeof(FVector)];		 // If CPT_Struct && IsVector().
		BYTE	RotationBytes[sizeof(FRotator)];	 // If CPT_Struct && IsRotator().
		UBOOL	Interface;
	};
	//@}

	/**
	* Copies the properties from this token into another.
	*
	* @param	Other	the token to copy this token's properties to.
	*/
	void Clone(const FToken& Other);

	FString GetValue()
	{
		if (TokenType == TOKEN_Const)
		{
			if (IsVector())
			{
				FVector& Vect = *(FVector*)VectorBytes;
				return FString::Printf(TEXT("FVector(%f,%f,%f)"), Vect.X, Vect.Y, Vect.Z);
			}
			else if (IsRotator())
			{
				FRotator& Rot = *(FRotator*)RotationBytes;
				return FString::Printf(TEXT("FRotator(%i,%i,%i)"), Rot.Pitch, Rot.Yaw, Rot.Roll);
			}
			else
			{
				switch (Type)
				{
				case CPT_Byte:				return FString::Printf(TEXT("%u"), Byte);
				case CPT_Int:				return FString::Printf(TEXT("%i"), Int);
					// Don't use GTrue/GFalse here because they can be localized
				case CPT_Bool:				return FString::Printf(TEXT("%s"), Bool ? *(FName::GetEntry(NAME_TRUE)->GetNameString()) : *(FName::GetEntry(NAME_FALSE)->GetNameString()));
				case CPT_Float:				return FString::Printf(TEXT("%f"), Float);
				case CPT_Name:				return FString::Printf(TEXT("%s"), *(*(FName*)NameBytes).ToString());
				case CPT_String:			return String;
				case CPT_Vector:
				{
					FVector& Vect = *(FVector*)VectorBytes;
					return FString::Printf(TEXT("FVector(%f,%f,%f)"), Vect.X, Vect.Y, Vect.Z);
				}
				case CPT_Rotation:
				{
					FRotator& Rot = *(FRotator*)RotationBytes;
					return FString::Printf(TEXT("FRotator(%i,%i,%i)"), Rot.Pitch, Rot.Yaw, Rot.Roll);
				}

				// unsupported
				case CPT_Range:
				case CPT_Struct:
				case CPT_ObjectReference:
				case CPT_Interface:
				case CPT_Delegate:
					return TEXT("");

				}

				return TEXT("???");
			}
		}
		else
		{
			return TEXT("N/A");
		}
	}

	// Constructors.
	FToken() : FPropertyBase(CPT_None, CPRT_None), TokenProperty(NULL), TokenFunction(NULL)
	{
		InitToken(CPT_None, CPRT_None);
	}
	FToken(EPropertyReferenceType InRefType)
		: FPropertyBase(CPT_None, InRefType), TokenProperty(NULL), TokenFunction(NULL)
	{
		InitToken(CPT_None, InRefType);
	}
	FToken(UProperty* Property, EPropertyReferenceType InRefType = CPRT_None)
		: FPropertyBase(Property, InRefType), TokenProperty(Property), TokenFunction(NULL)
	{
		StartPos = 0;
		StartLine = 0;

		TokenName = TokenProperty->GetFName();
		appStrncpy(Identifier, *TokenName.ToString(), NAME_SIZE);
		TokenType = TOKEN_Identifier;
		appMemzero(String, MAX_STRING_CONST_SIZE);
	}

	// copy constructors
	FToken(const FPropertyBase& InType)
		: FPropertyBase(CPT_None, InType.ReferenceType), TokenProperty(NULL), TokenFunction(NULL)
	{
		InitToken(CPT_None, InType.ReferenceType);
		(FPropertyBase&)*this = InType;
	}

	// Inlines.
	void InitToken(EPropertyType InType, EPropertyReferenceType InRefType = CPRT_None)
	{
		(FPropertyBase&)*this = FPropertyBase(InType, InRefType);
		TokenType = TOKEN_None;
		TokenName = NAME_None;
		StartPos = 0;
		StartLine = 0;
		*Identifier = 0;
		appMemzero(String, sizeof(Identifier));
	}
	UBOOL Matches(const TCHAR* Str) const
	{
		return (TokenType == TOKEN_Identifier || TokenType == TOKEN_Symbol) && appStricmp(Identifier, Str) == 0;
	}
	UBOOL Matches(const FName& Name) const
	{
		return TokenType == TOKEN_Identifier && TokenName == Name;
	}
	void AttemptToConvertConstant(const FPropertyBase& NewType)
	{
		assert(TokenType == TOKEN_Const);
		switch (NewType.Type)
		{
		case CPT_Int: {INT        V(0);           if (GetConstInt(V)) SetConstInt(V); break; }
		case CPT_Bool: {UBOOL      V(0);           if (GetConstBool(V)) SetConstBool(V); break; }
		case CPT_Float: {FLOAT      V(0.f);         if (GetConstFloat(V)) SetConstFloat(V); break; }
		case CPT_Name: {FName      V(NAME_None);   if (GetConstName(V)) SetConstName(V); break; }
		case CPT_Struct:
		{
			if (NewType.IsVector())
			{
				FVector V(0.f, 0.f, 0.f);
				if (GetConstVector(V))
				{
					SetConstVector(V);
				}
			}
			else if (NewType.IsRotator())
			{
				FRotator V(0, 0, 0);
				if (GetConstRotation(V))
				{
					SetConstRotation(V);
				}
			}
			//!!struct conversion support would be nice
			break;
		}
		case CPT_String:
		{
			break;
		}
		case CPT_Byte:
		{
			BYTE V = 0;
			if (NewType.Enum == NULL && GetConstByte(V))
			{
				SetConstByte(NULL, V);
			}
			break;
		}
		case CPT_ObjectReference:
		{
			UObject* Ob = NULL;
			if (GetConstObject(NewType.PropertyClass, Ob))
			{
				SetConstObject(Ob);
			}
			break;
		}
		}
	}

	// Setters.

	void SetTokenProperty(UProperty* Property)
	{
		TokenProperty = Property;
		TokenName = Property->GetFName();
		appStrcpy(Identifier, *TokenName.ToString());
		TokenType = TOKEN_Identifier;
	}

	void SetTokenFunction(FFunctionData* Function)
	{
		TokenFunction = Function;
	}
	void SetConstByte(UEnum* InEnum, BYTE InByte)
	{
		(FPropertyBase&)*this = FPropertyBase(CPT_Byte);
		Enum = InEnum;
		Byte = InByte;
		TokenType = TOKEN_Const;
	}
	void SetConstInt(INT InInt)
	{
		(FPropertyBase&)*this = FPropertyBase(CPT_Int);
		Int = InInt;
		TokenType = TOKEN_Const;
	}
	void SetConstBool(UBOOL InBool)
	{
		(FPropertyBase&)*this = FPropertyBase(CPT_Bool);
		Bool = InBool;
		TokenType = TOKEN_Const;
	}
	void SetConstFloat(FLOAT InFloat)
	{
		(FPropertyBase&)*this = FPropertyBase(CPT_Float);
		Float = InFloat;
		TokenType = TOKEN_Const;
	}
	void SetConstObject(UObject* InObject)
	{
		(FPropertyBase&)*this = FPropertyBase(CPT_ObjectReference);
		PropertyClass = InObject ? InObject->GetClass() : NULL;
		Object = InObject;
		TokenType = TOKEN_Const;
		if (PropertyClass == UClass::StaticClass())
		{
			MetaClass = CastChecked<UClass>(InObject);
		}
	}
	void SetConstDelegate()
	{
		(FPropertyBase&)*this = FPropertyBase(CPT_Delegate);
		TokenType = TOKEN_Const;
	}
	void SetConstName(FName InName)
	{
		(FPropertyBase&)*this = FPropertyBase(CPT_Name);
		*(FName *)NameBytes = InName;
		TokenType = TOKEN_Const;
	}
	void SetConstString(TCHAR* InString, INT MaxLength = MAX_STRING_CONST_SIZE)
	{
		check(MaxLength>0);
		(FPropertyBase&)*this = FPropertyBase(CPT_String);
		if (InString != String)
		{
			appStrncpy(String, InString, MaxLength);
		}
		TokenType = TOKEN_Const;
	}
	void SetConstVector(FVector &InVector)
	{
		(FPropertyBase&)*this = FPropertyBase(CPT_Struct);
		static UScriptStruct* VectorStruct = FindObjectChecked<UScriptStruct>(UObject::StaticClass(), TEXT("Vector"));
		Struct = VectorStruct;
		*(FVector *)VectorBytes = InVector;
		TokenType = TOKEN_Const;
	}
	void SetConstRotation(FRotator &InRotation)
	{
		(FPropertyBase&)*this = FPropertyBase(CPT_Struct);
		static UScriptStruct* RotatorStruct = FindObjectChecked<UScriptStruct>(UObject::StaticClass(), TEXT("Rotator"));
		Struct = RotatorStruct;
		*(FRotator *)RotationBytes = InRotation;
		TokenType = TOKEN_Const;
	}
	//!!struct constants

	// Getters.
	UBOOL GetConstByte(BYTE& B) const
	{
		if (TokenType == TOKEN_Const && Type == CPT_Byte)
		{
			B = Byte;
			return 1;
		}
		else if (TokenType == TOKEN_Const && Type == CPT_Int && Int >= 0 && Int<256)
		{
			B = (BYTE)Int;
			return 1;
		}
		else if (TokenType == TOKEN_Const && Type == CPT_Float && Float >= 0 && Float<255 && Float == appTrunc(Float))
		{
			B = (BYTE)Float;
			return 1;
		}
		else return 0;
	}
	UBOOL GetConstInt(INT& I) const
	{
		if (TokenType == TOKEN_Const && Type == CPT_Int)
		{
			I = Int;
			return 1;
		}
		else if (TokenType == TOKEN_Const && Type == CPT_Byte)
		{
			I = Byte;
			return 1;
		}
		else if (TokenType == TOKEN_Const && Type == CPT_Float && Float == appTrunc(Float))
		{
			I = (INT)Float;
			return 1;
		}
		else return 0;
	}
	UBOOL GetConstBool(UBOOL& B) const
	{
		if (TokenType == TOKEN_Const && Type == CPT_Bool)
		{
			B = Bool;
			return 1;
		}
		else return 0;
	}
	UBOOL GetConstFloat(FLOAT& R) const
	{
		if (TokenType == TOKEN_Const && Type == CPT_Float)
		{
			R = Float;
			return 1;
		}
		else if (TokenType == TOKEN_Const && Type == CPT_Int)
		{
			R = Int;
			return 1;
		}
		else if (TokenType == TOKEN_Const && Type == CPT_Byte)
		{
			R = Byte;
			return 1;
		}
		else return 0;
	}
	UBOOL GetConstObject(UClass* DesiredClass, UObject*& Ob) const
	{
		if (TokenType == TOKEN_Const && Type == CPT_ObjectReference && (DesiredClass == NULL || PropertyClass->IsChildOf(DesiredClass)))
		{
			Ob = Object;
			return 1;
		}
		return 0;
	}
	UBOOL GetConstName(FName& n) const
	{
		if (TokenType == TOKEN_Const && Type == CPT_Name)
		{
			n = *(FName *)NameBytes;
			return 1;
		}
		return 0;
	}
	UBOOL GetConstVector(FVector& v) const
	{
		if (TokenType == TOKEN_Const && IsVector())
		{
			v = *(FVector *)VectorBytes;
			return 1;
		}
		return 0;
	}
	//!!struct constants
	UBOOL GetConstRotation(FRotator& r) const
	{
		if (TokenType == TOKEN_Const && IsRotator())
		{
			r = *(FRotator *)RotationBytes;
			return 1;
		}
		return 0;
	}

	FString Describe()
	{
		return FString::Printf(
			TEXT("Property:%s  Type:%s  TokenName:%s  Value:%s  Struct:%s  Flags:%lli  RefType:%s"),
			TokenProperty != NULL ? *TokenProperty->GetName() : TEXT("NULL"),
			GetPropertyTypeText(Type), *TokenName.ToString(), *GetValue(),
			Struct != NULL ? *Struct->GetName() : TEXT("NULL"),
			PropertyFlags, GetPropertyRefText(ReferenceType)
		);
	}
};