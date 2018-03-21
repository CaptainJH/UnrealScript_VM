#pragma once
#include "ScriptCompilerHelper.h"


enum ECompilerPassType
{
	/** None */
	PASS_None,

	/** Parsing */
	PASS_Parse,

	/** post-parse cleanup */
	PASS_PostParse,

	/** compiling */
	PASS_Compile,

	/** importing defaults */
	PASS_Defaults,
};

// Max nesting.
enum { MAX_NEST_LEVELS = 16 };

enum EScriptCompilerWarningLevel
{
	SCWL_Level1,
	SCWL_Level2,
	SCWL_Level3,
	SCWL_Level4,
};

//
// Script compiler class.
//
class FScriptCompiler
{
public:
	// Variables.
	FString				ContextFormat;			// Printf formatting string.
	FFeedbackContext*	Warn;					// For compiling messages and errors.
	UClass*				Class;					// Actor class info while compiling is happening.
	UTextBuffer*		ErrorText;				// Error text buffer.
	FMemStack*			Mem;					// Pool for temporary allocations.
	const TCHAR*		Input;					// Input text.
	INT					InputLen;				// Length of input text.
	INT					InputPos;				// Current position in text.
	INT					InputLine;				// Current line in text.
	INT					PrevPos;				// Position previous to last GetChar() call.
	INT					PrevLine;				// Line previous to last GetChar() call.
	FString				PrevComment;			// Previous comment parsed by GetChar() call.
	UBOOL				bPrevCommentFormatted;	// TRUE once PrevComment has been formatted.
	INT					StatementsCompiled;		// Number of statements compiled.
	INT					LinesCompiled;			// Total number of lines compiled.
	UBOOL				GotAffector;			// Got an expression that has a side effect?
	UProperty*			AffectorReturnProperty; // property returned by affector
	UBOOL				GotIterator;			// Got an iterator.
	INT					Booting;				// Bootstrap compiling classes.
	ECompilerPassType	Pass;					// Compilation pass.
	INT					NestLevel;				// Current nest level, starts at 0.
	FNestInfo*			TopNest;				// Top nesting level.
	UStruct*			TopNode;				// Top stack node.
	FNestInfo			Nest[MAX_NEST_LEVELS];	// Information about all nesting levels.
	INT					OriginalPropertiesSize;	// Original native properties size before compile.
	UBOOL				InDefaultPropContext;	// GetContext() is in default properties mode
	UBOOL				bEmitDebugInfo;			//DEBUGGER Emit debug information to the stream.
	UBOOL				bReparsingClass;		// Reparsing a class that has already been compiled
	INT					SupressDebugInfo;
	EScriptCompilerWarningLevel	RequiredWarningLevel;	// Any calls to ScriptWarnf with a WarningLevel less than this value will be ignored
	FClassMetaData*		ClassData;
	INT					OriginalVariableIndex;	//Global index that indicates the order of variable initialization
	TMap<FString, INT>	PreviousNames;


	/** default values for optional parameters that need to be resolved after all classes have been parsed */
	struct FUnresolvedExpression*	UnresolvedParameterValues;


	/** Script writer. */
	FScriptWriter			Writer;

	// Constructor.
	FScriptCompiler(FFeedbackContext* InWarn, UBOOL bParseOnly)
		: Writer(*this)
		, Warn(InWarn)
		, bReparsingClass(bParseOnly)
		, TopNode(NULL)
		, TopNest(NULL)
		, LocalProperties(NULL)
		, Pass(PASS_None)
		, UnresolvedParameterValues(NULL)
		, RequiredWarningLevel(SCWL_Level1)
		, OriginalVariableIndex(0)
	{
		FScriptLocation::Compiler = this;
	}

	~FScriptCompiler()
	{
		if (FScriptLocation::Compiler == this)
		{
			FScriptLocation::Compiler = NULL;
		}
	}

	// Precomputation.
	void PostParse(UStruct* Node);

	// FContextSupplier interface.
	FString GetContext();

	// High-level compiling functions.
	UBOOL			CompileScript(FClassTree& AllClasses, UClass* Class, FMemStack* Mem, UBOOL Booting, ECompilerPassType Pass);

	void			CompileDirective();
	void			CompileSecondPass(UStruct* Node);
	UEnum*			CompileEnum(UStruct* Owner, UBOOL& NeedSemicolon);
	UScriptStruct*	CompileStruct(UStruct* Owner, UBOOL& NeedSemicolon);
	void			CompileConst(UStruct* Owner, UBOOL& NeedSemicolon);
	UBOOL			CompileDeclaration(FToken& Token, UBOOL& NeedSemicolon);
	void			CompileClassDeclaration(FToken& Token, UBOOL& NeedSemicolon);
	void			CompileFunctionDeclaration(FToken& Token, UBOOL& NeedSemiColon);
	void			CompileVariableDeclaration(FToken& Token, UBOOL& NeedSemiColon);
	void			CompileStateDeclaration(FToken& Token, UBOOL& NeedSemiColon);
	void			CompileIgnoreDeclaration(FToken& Token, UBOOL& NeedSemiColon);

	/** compile Java style interface declaration  **/
	void			CompileInterfaceDeclaration(FToken& Token, UBOOL& NeedSemicolon);

	void			CompileCommand(FToken& Token, UBOOL& NeedSemicolon);
	UBOOL			CompileStatement();
	void			CompileStatements();

	/**
	* Compiles an arbitrary general expression.
	*
	* @param	RequiredType	contains information for type-checking the result of the expression. If RequiredType.Type == CPT_None,
	*							type checking isn't necessary for this expression
	* @param	ErrorTag		arbitrary text which will be used in any messages that are logged while compiling this expression
	* @param	ResultToken		Will be filled in with information for the result of the evaluation of the expression.
	* @param	MaxPrecedence	@todo
	* @param	HintType		Used for communication of additional information to use when compiling complex expressions.
	* @param	bInParentheses	If true, allow non-matching types as the type will be checked again after the parentheses are resolved
	* @param	TokenList		Tracks the tokens that were encountered while compiling this expression.
	* @param	bIsEnteringNest	TRUE if we are evaluating an expression that controls a change in nesting level (such as an if-check). Used
	*							for controlling null context response (what to do if runtime evaluates an expression through a null context)
	* @param	StructModificationByte	If a struct expression is compiled, an entry will be added to the nest's StructModificationByteList to
	*										keep track of where the struct member code must be updated if it is found that that the struct will be modified
	*										by the code compiled following the struct expression. If this parameter is specified and non NULL, that byte location
	*										will be placed in the passed-in data and the calling function will be responsible for popping the entry
	*										instead of CompileExpr() managing that task itself
	*										(used for e.g. CompileAffector() because it may need to modify the byte itself if it detects an assignment operation)
	*
	* @return	0: if no expression was parsed.
	*			1: if an expression matching RequiredType (or any type if CPT_None) was parsed.
	*			-1:	if there was a type mismatch.
	*/
	UBOOL			CompileExpr(const FPropertyBase RequiredType, const TCHAR* ErrorTag = NULL, FToken* ResultToken = NULL, INT MaxPrecedence = MAXINT, FToken* HintType = NULL, UBOOL bInParentheses = false, FTokenChain* TokenList = NULL, UBOOL bIsEnteringNest = FALSE, FScopedStructModificationByte* StructModificationByte = NULL);
	UBOOL			CompileFieldExpr(UStruct* Scope, FPropertyBase& RequiredType, FToken Token, FToken& ResultToken, UBOOL IsSelf, UBOOL IsConcrete, FScriptLocation* StartOfExpression);
	UBOOL			CompileDynamicCast(const FToken& Token, FToken& ResultType);
	void			CompileAffector(EPropertyReferenceType LeftRefType = CPRT_AssignValue, EPropertyReferenceType RightRefType = CPRT_AssignmentReference);
	void			CompileParameterValue(UProperty* Prop, FPropertyBase& RequiredType);

	/**
	* Gets the next token from the input stream, advancing the variables which keep track of the current input position and line.
	*
	* @param	Token			receives the value of the parsed text; if Token is pre-initialized, special logic is performed
	*							to attempt to evaluated Token in the context of that type.  Useful for distinguishing between ambigous symbols
	*							like enum tags.
	* @param	Hint			optional tag for providing additional information about the type of token that is expected.
	* @param	NoConsts		specify TRUE to indicate that tokens representing literal const values are not allowed.
	*
	* @return	TRUE if a token was successfully processed, FALSE otherwise.
	*/
	UBOOL			GetToken(FToken& Token, const FPropertyBase* Hint = NULL, UBOOL NoConsts = FALSE);

	/**
	* Put all text from the current position up to either EOL or the StopToken
	* into Token.  Advances the compiler's current position.
	*
	* @param	Token	[out] will contain the text that was parsed
	* @param	StopChar	stop processing when this character is reached
	*
	* @return	TRUE if a token was parsed
	*/
	UBOOL			GetRawToken(FToken& Token, TCHAR StopChar = TCHAR('\n'));
	void			UngetToken(FToken& Token);
	UBOOL			GetIdentifier(FToken& Token, INT NoConsts = 0);
	UClass*			GetQualifiedClass(const TCHAR* Thing);
	UBOOL			GetSymbol(FToken& Token);
	void			CheckAllow(const TCHAR* Thing, DWORD AllowFlags);
	void			CheckInScope(UObject* Obj);
	UStruct*		GetSuperScope(UStruct* CurrentScope, const FName& SearchName);

	/**
	* Find a field in the specified context.  Starts with the specified scope, then iterates
	* through the Outer chain (including ClassWithin-type outers) until the field is found.
	*
	* @param	InScope				scope to start searching for the field in
	* @param	InIdentifier		name of the field we're searching for
	* @param	bIncludeParents		whether to allow searching in the scope of a parent struct
	* @param	FieldClass			class of the field to search for.  used to e.g. search for functions only
	* @param	Thing				hint text that will be used in the error message if an error is encountered
	* @param	OuterContextCount	[out] if non-NULL, set to the number of 'Outer' contexts which must be emitted if the field is found in an outer class (ClassWithin),
	*								which will require calling EmitOuterContext() before emitting the property reference bytecodes
	*
	* @return	a pointer to a UField with a name matching InIdentifier, or NULL if it wasn't found
	*/
	UField*			FindField(UStruct* InScope, const TCHAR* InIdentifier, UBOOL bIncludeParents = TRUE, UClass* FieldClass = UField::StaticClass(), const TCHAR* Thing = NULL, INT* OuterContextCount = NULL);
	INT				ConversionCost(const FPropertyBase& Dest, const FPropertyBase& Source);
	void			SkipStatements(int SubCount, const TCHAR* ErrorTag);
	/**
	* Parses a variable or return value declaration and determines the variable type and property flags.
	*
	* @param		Scope				struct to create the property in
	* @param		VarProperty			will be filled in with type and property flag data for the property declaration that was parsed
	* @param		ObjectFlags			will contain the object flags that should be assigned to all UProperties which are part of this declaration (will not be determined
	*									until GetVarNameAndDim is called for this declaration).
	* @param		Disallow			contains a mask of variable modifiers that are disallowed in this context
	* @param		Thing				used for compiler errors to provide more information about the type of parsing that was occurring
	* @param		OuterPropertyType	only specified when compiling the inner properties for arrays or maps.  corresponds to the FToken for the outer property declaration.
	*
	* @return
	*/
	UBOOL			GetVarType(UStruct* Scope, FPropertyBase& VarProperty, EObjectFlags& ObjectFlags, QWORD Disallow, const TCHAR* Thing, FToken* OuterPropertyType = NULL);
	/**
	* Parses a variable name declaration and creates a new UProperty object.
	*
	* @param	Scope			struct to create the property in
	* @param	VarProperty		type and propertyflag info for the new property
	* @param	ObjectFlags		flags to pass on to the new property
	* @param	NoArrays		TRUE if static arrays are disallowed
	* @param	IsFunction		TRUE if the property is a function parameter or return value
	* @param	HardcodedName	name to assign to the new UProperty, if specified. primarily used for function return values,
	*							which are automatically called "ReturnValue"
	* @param	HintText		text to use in error message if error is encountered
	* @param	Category		editor category to place this property in
	* @param	Skip			TRUE if we're not supposed to actually create the UProperty object
	*
	* @return	a pointer to the new UProperty if successful, or NULL if there was no property to parse
	*/
	UProperty*      GetVarNameAndDim(UStruct* Struct, FPropertyBase& VarProperty, EObjectFlags ObjectFlags, UBOOL NoArrays, UBOOL IsFunction, const TCHAR* HardcodedName, const TCHAR* Thing, FName Category, UBOOL Skip);
	void			CheckObscures(UStruct* Scope, FToken& Token);
	UBOOL			AllowReferenceToClass(UClass* CheckClass) const;
	UBOOL			IsValidFunctionSpecifier(const FToken& Token);

	/**
	* @return	TRUE if Scope has UProperty objects in its list of fields
	*/
	static UBOOL	HasMemberProperties(const UStruct* Scope);

	/**
	* Extract only valid unrealscript from the input stream
	*
	* @param	StartPos	position into the Input array to start
	* @param	StartLine	line number to start on
	* @param	Count		how far to go before stopping
	*
	* @return	compact unrealscript text stripped of all extra stuff (whitespace, comments, new lines, etc.)
	*/
	FString			StripExpressionText(INT StartPos, INT StartLine, INT Count);

	/**
	* Parses optional alternate text specified for various variable modifiers that will modify the way
	* the variable is exported to the header file.
	*
	* @param	ExportFlags			will be set with EPropertyHeaderExportFlags that are parsed
	* @param	ParseErrorHint		the text to use in parsing error messages
	*
	* @return	TRUE if additional export text was specified
	*/
	UBOOL			ParsePropertyExportText(DWORD& ExportFlags, const TCHAR* ParseErrorHint);

	/**
	* Parses optional alternate text to be used for exporting a variable declaration.
	*
	* @param	VarProperty			the token corresponding to the variable declaration
	* @param	PropertyTypeName	the type of property being parsed (used for logging)
	*
	* @return	TRUE if additional export text was specified
	*/
	UBOOL			ParsePropertyExportText(FPropertyBase& VarProperty, const TCHAR* PropertyTypeName);

	/**
	* Ensures at script compile time that the metadata formatting is correct
	* @param	InKey			the metadata key being added
	* @param	InValue			the value string that will be associated with the InKey
	*/
	void ValidateMetaDataFormat(const FString& InKey, const FString& InValue);

	/**
	* Parses optional metadata text.
	*
	* @param	VarProperty			the token corresponding to the variable declaration
	* @param	PropertyTypeName	the type of property being parsed (used for logging)
	*
	* @return	TRUE if metadata was specified
	*/
	UBOOL			ParsePropertyMetaData(FPropertyBase& VarProperty, const TCHAR* PropertyTypeName);

	/**
	* Reads a comment block from the input stream and if found, applies the comment as the tooltip for any of the
	* specified properties which do not already have a tooltip.
	*
	* @param	VarProperty			the token corresponding to the variable declaration
	* @param	Properties			the list of properties which were declared by this variable declaration
	*/
	void			ConvertEOLCommentToTooltip(FPropertyBase& VarProperty, const TArray<UProperty*>& Properties);

	// Low-level parsing functions.
	const TCHAR*	NestTypeName(ENestType NestType);
	TCHAR			GetChar(UBOOL Literal = 0);
	TCHAR			PeekChar();
	TCHAR			GetLeadingChar();
	void			UngetChar();
	UBOOL			IsEOL(TCHAR c);
	void VARARGS	AddResultText(const TCHAR* Fmt, ...);
	UBOOL			GetConstInt(int& Result, const TCHAR* Tag = NULL, UStruct* Scope = NULL); //NEW: LanguageEnhancements -- from Paul DuBois, InfiniteMachine
	UBOOL			GetConstFloat(FLOAT& Result, const TCHAR* Tag = NULL);
	const TCHAR*	FunctionNameCpp(UFunction* Function);
	UProperty*		GetUObjectOuterProperty() const;

	// Nest management functions.
	void			PushNest(ENestType NestType, FName ThisName, UStruct* InNode);
	void			PopNest(ENestType NestType, const TCHAR* Descr);
	INT				FindNest(ENestType NestType);
	/**
	* Remove all placeholders of the specified type from the specified nest's JumpPlaceholderList linked list.
	*
	* @param	Nest	the nest to remove the placeholders from
	* @param	Type	the placeholder type to remove
	*
	* @return	the number of placeholders that were removed
	*/
	INT				RemoveJumpAddressPlaceholders(FNestInfo* Nest, EFixupType Type);
	/**
	* Replace all skip/jump address placeholders in the specified nest with the correct values.
	*
	* @param	Nest	the nest that contains the placeholders that are ready to be fixed up
	*/
	void			ReplacePlaceholderValues(FNestInfo* Nest);
	ENestType		GetNestType(const UStruct* InNode) const;

	/**
	* Binds all delegate properties declared in ValidationScope the delegate functions specified in the variable declaration, verifying that the function is a valid delegate
	* within the current scope.  This must be done once the entire class has been parsed because instance delegate properties must be declared before the delegate declaration itself.
	*
	* @todo: this function will no longer be required once the post-parse fixup phase is added (TTPRO #13256)
	*
	* @param	ValidationScope		the scope to validate delegate properties for
	* @param	OwnerClass			the class currently being compiled.
	* @param	DelegateCache		cached map of delegates that have already been found; used for faster lookup.
	*/
	void FixupDelegateProperties(UStruct* ValidationScope, UClass* OwnerClass, TMap<FName, UFunction*>& DelegateCache);

	// Matching predefined text.
	UBOOL			MatchIdentifier(FName Match);
	UBOOL			PeekIdentifier(FName Match);
	UBOOL			MatchSymbol(const TCHAR* Match);
	UBOOL			PeekSymbol(const TCHAR* Match);

	// Requiring predefined text.
	void			RequireIdentifier(FName Match, const TCHAR* Tag);
	void			RequireSymbol(const TCHAR* Match, const TCHAR* Tag);
	void			RequireSizeOfParm(FToken& TypeToken, const TCHAR* Tag);

	// Retry functions.
	void			InitScriptLocation(FScriptLocation& Retry);
	void			ReturnToLocation(const FScriptLocation& Retry, UBOOL Binary = 1, UBOOL Text = 1);
	void			MoveCompiledCode(FScriptLocation& Dest, FScriptLocation& Source);

	// Emitters.
	void			EmitConstant(FToken& ConstToken);
	void			EmitStackNodeLinkFunction(UFunction* Node, UBOOL ForceFinal, UBOOL Global, UProperty *DelegateProp = NULL);

	/**
	* Emit a placeholder for a jump target and add this placeholder to the specified nest's list of addresses to fixup
	* - this placeholder value will be filled in with the correct location when the nest is popped.
	*
	* @param	Nest		the nest that will fill in the placeholder value with the actual value
	* @param	Type		indicates which of the nest's fix-up linked lists to add this placeholder to
	* @param	Name		the name of the label (only used for label fix-up types)
	* @param	Location	the location in the bytecode to emit the placeholder value.  If no value
	*						is specified, the placeholder is emitted at the top of the current node's
	*						script.
	* @param	Value		the placeholder value to emit
	*/
	void			EmitPlaceholderForJumpAddress(FNestInfo* Nest, EFixupType Type, FName Name, INT Location = INDEX_NONE, CodeSkipSizeType Value = 0);
	void			EmitPlaceholderForChainAddress(FNestInfo* Nest);
	void			EmitChainAddressValue(FNestInfo* Nest);
private:
	void			EmitFieldPlaceholderFixup(TArray<BYTE>& NodeScript, INT FixupLocation, UField** Property);
public:
	void			EmitProperty(FToken& ExpressionToken, const TCHAR* Tag);
	void			EmitLet(const FPropertyBase& Type, const TCHAR* Tag);

	/**
	* Emit bytecodes corresponding to an Outer context reference.
	*
	* @return	the index into the current node's bytecode (Script array) for
	*			the context skip-over placeholder value (once the size of the
	*			field being referenced through the context expression is known,
	*			the placeholder at this index should be replaced with that size)
	*/
	INT				EmitOuterContext(FScriptLocation& StartOfExpression, UBOOL bHasExistingContext);

	/**
	* Emits bytecodes for performing a type conversion.
	*
	* @param	ConversionType		the type of conversion to perform
	* @param	InsertionPoint		the location to insert the conversion bytecodes.
	* @param	DestinationToken	the token containing the data for the target of the conversion
	*/
	void			EmitConversion(ECastToken ConversionType, FScriptLocation& InsertionPoint, const FPropertyBase& DestinationToken);
	void			EmitDebugInfo(BYTE OpCode); //DEBUGGER

												/** Clears out the stored comment. */
	void ClearComment();

	VARARG_DECL(void, void, {}, ScriptErrorf, VARARG_NONE, const TCHAR*, VARARG_EXTRA(EScriptCompilerErrorLevel ErrorLevel), VARARG_EXTRA(ErrorLevel));
	VARARG_DECL(void, void, {}, ScriptWarnf, VARARG_NONE, const TCHAR*, VARARG_EXTRA(EScriptCompilerWarningLevel WarningLevel), VARARG_EXTRA(WarningLevel));

	/**
	* Helper function to determine whether the class hierarchy rooted at Suspect is
	* dependent on the hierarchy rooted at Source.
	*
	* @param	Suspect		Root of hierarchy for suspect class
	* @param	Source		Root of hierarchy for source class
	* @param	AllClasses	Array of parsed classes
	* @return	TRUE if the hierarchy rooted at Suspect is dependent on the one rooted at Source, FALSE otherwise
	*/
	UBOOL IsDependentOn(UClass* Suspect, UClass* Source, const FClassTree& AllClasses);

	/**
	* Hierarchically import defaultproperties for all classes (Step 2).  Ensures that the class's defaults have been initialized
	* (all defaults are propagated from its parent class), propagates defaults for all struct properties, and that the class's component map is valid.
	* Then calls ImportDefaultProperties to actually import the default property text into the Defaults address space for the current class.  Finally
	* recursively calls CompileDefaultProperties for the child classes of the current class.
	*
	* @param	AllClasses		the classtree node associated with the current class
	* @param	MakeSubclasses	TRUE if we should compile child classes
	*
	* @return	TRUE if the class defaults were successfully parsed
	*/
	UBOOL CompileDefaultProperties(const FClassTree& AllClasses, UBOOL MakeSubclasses);

	/**
	* Hierarchically import defaultproperties for all classes (Step 1).  Determines if any classes
	* failed to import their defaultproperties (by checking whether any classes still need defaults imported)
	*
	* @param	AllClasses		the classtree node associated with Class
	* @param	Compiler		the script compiler
	* @param	MakeSubclasses	TRUE if we should compile child classes
	*
	* @return	TRUE if the class defaults were successfully parsed
	*/
	UBOOL CompileClassDefaults(const FClassTree& AllClasses, UBOOL MakeSubclasses);

	/**
	* If the property has already been seen during compilation, then return add. If not,
	* then return replace so that INI files don't mess with header exporting
	*
	* @param PropertyName the string token for the property
	*
	* @return FNAME_Replace or FNAME_Add
	*/
	EFindName GetFindFlagForPropertyName(const TCHAR* PropertyName);

	TMap<UProperty*, INT>	LocalPropertyLineNumbers;
	FLocalProperty*			LocalProperties;
};