#pragma once
#include <vector>
#include "Types.h"
#include "UObject.h"


#define DECLARE_UINT64(x)	x

// For compilers that don't support 64 bit enums.
#define	CPF_Edit					DECLARE_UINT64(0x0000000000000001)		// Property is user-settable in the editor.
#define	CPF_Const				DECLARE_UINT64(0x0000000000000002)		// Actor's property always matches class's default actor property.
#define CPF_Input					DECLARE_UINT64(0x0000000000000004)		// Variable is writable by the input system.
#define CPF_ExportObject			DECLARE_UINT64(0x0000000000000008)		// Object can be exported with actor.
#define CPF_OptionalParm			DECLARE_UINT64(0x0000000000000010)		// Optional parameter (if CPF_Param is set).
#define CPF_Net					DECLARE_UINT64(0x0000000000000020)		// Property is relevant to network replication.
#define CPF_EditFixedSize			DECLARE_UINT64(0x0000000000000040)		// Indicates that elements of an array can be modified, but its size cannot be changed.
#define CPF_Parm					DECLARE_UINT64(0x0000000000000080)		// Function/When call parameter.
#define CPF_OutParm				DECLARE_UINT64(0x0000000000000100)		// Value is copied out after function call.
#define CPF_SkipParm				DECLARE_UINT64(0x0000000000000200)		// Property is a short-circuitable evaluation function parm.
#define CPF_ReturnParm				DECLARE_UINT64(0x0000000000000400)		// Return value.
#define CPF_CoerceParm				DECLARE_UINT64(0x0000000000000800)		// Coerce args into this function parameter.
#define CPF_Native      			DECLARE_UINT64(0x0000000000001000)		// Property is native: C++ code is responsible for serializing it.
#define CPF_Transient   			DECLARE_UINT64(0x0000000000002000)		// Property is transient: shouldn't be saved, zero-filled at load time.
#define CPF_Config      			DECLARE_UINT64(0x0000000000004000)		// Property should be loaded/saved as permanent profile.
#define CPF_Localized   			DECLARE_UINT64(0x0000000000008000)		// Property should be loaded as localizable text.

#define CPF_EditConst   			DECLARE_UINT64(0x0000000000020000)		// Property is uneditable in the editor.
#define CPF_GlobalConfig			DECLARE_UINT64(0x0000000000040000)		// Load config from base class, not subclass.
#define CPF_Component				DECLARE_UINT64(0x0000000000080000)		// Property containts component references.
#define CPF_AlwaysInit				DECLARE_UINT64(0x0000000000100000)		// Property should never be exported as NoInit	(@todo - this doesn't need to be a property flag...only used during make)
#define CPF_DuplicateTransient		DECLARE_UINT64(0x0000000000200000)		// Property should always be reset to the default value during any type of duplication (copy/paste, binary duplication, etc.)
#define CPF_NeedCtorLink			DECLARE_UINT64(0x0000000000400000)		// Fields need construction/destruction.
#define CPF_NoExport    			DECLARE_UINT64(0x0000000000800000)		// Property should not be exported to the native class header file.
#define	CPF_NoImport				DECLARE_UINT64(0x0000000001000000)		// Property should not be imported when creating an object from text (copy/paste)
#define CPF_NoClear				DECLARE_UINT64(0x0000000002000000)		// Hide clear (and browse) button.
#define CPF_EditInline				DECLARE_UINT64(0x0000000004000000)		// Edit this object reference inline.

#define CPF_EditInlineUse			DECLARE_UINT64(0x0000000010000000)		// EditInline with Use button.
#define CPF_Deprecated  			DECLARE_UINT64(0x0000000020000000)		// Property is deprecated.  Read it from an archive, but don't save it.
#define CPF_DataBinding			DECLARE_UINT64(0x0000000040000000)		// Indicates that this property should be exposed to data stores
#define CPF_SerializeText			DECLARE_UINT64(0x0000000080000000)		// Native property should be serialized as text (ImportText, ExportText)

#define CPF_RepNotify				DECLARE_UINT64(0x0000000100000000)		// Notify actors when a property is replicated
#define CPF_Interp				DECLARE_UINT64(0x0000000200000000)		// interpolatable property for use with matinee
#define CPF_NonTransactional		DECLARE_UINT64(0x0000000400000000)		// Property isn't transacted

#define CPF_EditorOnly				DECLARE_UINT64(0x0000000800000000)		// Property should only be loaded in the editor
#define CPF_NotForConsole			DECLARE_UINT64(0x0000001000000000)		// Property should not be loaded on console (or be a console cooker commandlet)
#define CPF_RepRetry				DECLARE_UINT64(0x0000002000000000)		// retry replication of this property if it fails to be fully sent (e.g. object references not yet available to serialize over the network)
#define CPF_PrivateWrite			DECLARE_UINT64(0x0000004000000000)		// property is const outside of the class it was declared in
#define CPF_ProtectedWrite			DECLARE_UINT64(0x0000008000000000)		// property is const outside of the class it was declared in and subclasses

#define CPF_ArchetypeProperty		DECLARE_UINT64(0x0000010000000000)		// property should be ignored by archives which have ArIgnoreArchetypeRef set

#define CPF_EditHide				DECLARE_UINT64(0x0000020000000000)		// property should never be shown in a properties window.
#define CPF_EditTextBox			DECLARE_UINT64(0x0000040000000000)		// property can be edited using a text dialog box.

#define CPF_CrossLevelPassive		DECLARE_UINT64(0x0000100000000000)		// property can point across levels, and will be serialized properly, but assumes it's target exists in-game (non-editor)
#define CPF_CrossLevelActive		DECLARE_UINT64(0x0000200000000000)		// property can point across levels, and will be serialized properly, and will be updated when the target is streamed in/out


/** @name Combinations flags */
//@{
#define	CPF_ParmFlags				(CPF_OptionalParm | CPF_Parm | CPF_OutParm | CPF_SkipParm | CPF_ReturnParm | CPF_CoerceParm)
#define	CPF_PropagateFromStruct		(CPF_Const | CPF_Native | CPF_Transient)
#define	CPF_PropagateToArrayInner	(CPF_ExportObject | CPF_EditInline | CPF_EditInlineUse | CPF_Localized | CPF_Component | CPF_Config | CPF_AlwaysInit | CPF_EditConst | CPF_Deprecated | CPF_SerializeText | CPF_CrossLevel | CPF_EditorOnly )

/** the flags that should never be set on interface properties */
#define CPF_InterfaceClearMask		(CPF_NeedCtorLink|CPF_ExportObject)

/** a combination of both cross level types */
#define CPF_CrossLevel				(CPF_CrossLevelPassive | CPF_CrossLevelActive)	

/** all the properties that can be stripped for final release console builds */
#define CPF_DevelopmentAssets		(CPF_EditorOnly | CPF_NotForConsole)

#define CPF_AllFlags				DECLARE_UINT64(0xFFFFFFFFFFFFFFFF)

class UProperty;

class UField : public UObject
{
public:

	// Variables.
	UField*			Next = nullptr;
};

class UStruct : public UField
{
public:

	INT GetPropertiesSize() const;
	INT GetMinAlignment() const;

	int PropertiesSize;
	int MinAlignment;
	std::vector<BYTE> ScriptStorage;
	std::vector<BYTE> Script;

	UStruct*			SuperStruct;

	UField*				Children = nullptr;

	UProperty*			ConstructorLink;

	UStruct* GetSuperStruct() const
	{
		return SuperStruct;
	}
};

class UScriptStruct : public UStruct
{
public:
	
};

class UClass : public UStruct
{
public:
	UClass(size_t InSize, size_t flag, size_t castFlag, const char* name);

	INT GetDefaultsCount();

	UBOOL IsChildOf(const UStruct* SomeBase) const
	{
		for (const UStruct* Struct = this; Struct; Struct = Struct->GetSuperStruct())
			if (Struct == SomeBase)
				return 1;
		return 0;
	}

};

class UFunction : public UStruct
{
public:
	int NumParams;
	int ParamSize;
	int ReturnValueOffset;
	int BytecodeScriptSize;

	UProperty* GetReturnProperty();
};

class UEnum : public UField
{
public:
};

class UProperty : public UField
{
public:

	int Offset;
	INT ArrayDim;
	INT ElementSize;

	int PropertySize;

	QWORD PropertyFlags;
	UProperty*	ConstructorLinkNext;

	virtual void CopySingleValue(void* Dest, void* Src);
	virtual void CopyCompleteValue(void* Dest, void* Src); 
	virtual void CopySingleValue(void* Dest, void* Src, UObject* SubobjectRoot=NULL, UObject* DestOwnerObject=NULL/*, FObjectInstancingGraph* InstanceGraph=NULL*/) const;
	virtual void DestroyValue(void* Dest) const;

	INT GetSize() const;

	virtual bool Identical(const void* A, const void* B, DWORD PortFlags = 0) const { return false; }

};

class UIntProperty : public UProperty
{
public:
	bool Identical(const void* A, const void* B, DWORD PortFlags = 0) const override;
};

class UStrProperty : public UProperty
{
public:
};

class UBoolProperty : public UProperty
{
public:
	bool Identical(const void* A, const void* B, DWORD PortFlags = 0) const override;

	BITFIELD BitMask;
};

class UFloatProperty : public UProperty
{
public:
	bool Identical(const void* A, const void* B, DWORD PortFlags = 0) const override;
};

class UByteProperty : public UProperty
{
public:
	bool Identical(const void* A, const void* B, DWORD PortFlags = 0) const override;
};

class UArrayProperty : public UProperty
{
public:
	UProperty* Inner;
};

class UStructProperty : public UProperty
{
public:

};

class UDelegateProperty : public UProperty
{
public:
	/** Function this delegate is mapped to */
	UFunction* Function;

	/**
	* If this DelegateProperty corresponds to an actual delegate variable (as opposed to the hidden property the script compiler creates when you declare a delegate function)
	* points to the source delegate function (the function declared with the delegate keyword) used in the declaration of this delegate property.
	*/
	UFunction* SourceDelegate;
};