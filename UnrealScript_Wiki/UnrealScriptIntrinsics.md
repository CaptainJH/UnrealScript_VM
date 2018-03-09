The UnrealScript Intrinsics are as follow:

### all of the enum value defined in enum EExprToken

    EX_LocalVariable		= 0x00,	// A local variable.
	EX_InstanceVariable		= 0x01,	// An object variable.
	EX_DefaultVariable		= 0x02,	// Default variable for a concrete object.
	EX_StateVariable		= 0x03, // State local variable.

	// Tokens.
	EX_Return				= 0x04,	// Return from function.
	EX_Switch				= 0x05,	// Switch.
	EX_Jump					= 0x06,	// Goto a local address in code.
	EX_JumpIfNot			= 0x07,	// Goto if not expression.
	EX_Stop					= 0x08,	// Stop executing state code.
	EX_Assert				= 0x09,	// Assertion.
	EX_Case					= 0x0A,	// Case.
	EX_Nothing				= 0x0B,	// No operation.
	EX_LabelTable			= 0x0C,	// Table of labels.
	EX_GotoLabel			= 0x0D,	// Goto a label.
	EX_EatReturnValue       = 0x0E, // destroy an unused return value
	EX_Let					= 0x0F,	// Assign an arbitrary size value to a variable.
	EX_DynArrayElement      = 0x10, // Dynamic array element.!!
	EX_New                  = 0x11, // New object allocation.
	EX_ClassContext         = 0x12, // Class default metaobject context.
	EX_MetaCast             = 0x13, // Metaclass cast.
	EX_LetBool				= 0x14, // Let boolean variable.
	EX_EndParmValue			= 0x15,	// end of default value for optional function parameter
	EX_EndFunctionParms		= 0x16,	// End of function call parameters.
	EX_Self					= 0x17,	// Self object.
	EX_Skip					= 0x18,	// Skippable expression.
	EX_Context				= 0x19,	// Call a function through an object context.
	EX_ArrayElement			= 0x1A,	// Array element.
	EX_VirtualFunction		= 0x1B,	// A function call with parameters.
	EX_FinalFunction		= 0x1C,	// A prebound function call with parameters.
	EX_IntConst				= 0x1D,	// Int constant.
	EX_FloatConst			= 0x1E,	// Floating point constant.
	EX_StringConst			= 0x1F,	// String constant.
	EX_ObjectConst		    = 0x20,	// An object constant.
	EX_NameConst			= 0x21,	// A name constant.
	EX_RotationConst		= 0x22,	// A rotation constant.
	EX_VectorConst			= 0x23,	// A vector constant.
	EX_ByteConst			= 0x24,	// A byte constant.
	EX_IntZero				= 0x25,	// Zero.
	EX_IntOne				= 0x26,	// One.
	EX_True					= 0x27,	// Bool True.
	EX_False				= 0x28,	// Bool False.
	EX_NativeParm           = 0x29, // Native function parameter offset.
	EX_NoObject				= 0x2A,	// NoObject.

	EX_IntConstByte			= 0x2C,	// Int constant that requires 1 byte.
	EX_BoolVariable			= 0x2D,	// A bool variable which requires a bitmask.
	EX_DynamicCast			= 0x2E,	// Safe dynamic class casting.
	EX_Iterator             = 0x2F, // Begin an iterator operation.
	EX_IteratorPop          = 0x30, // Pop an iterator level.
	EX_IteratorNext         = 0x31, // Go to next iteration.
	EX_StructCmpEq          = 0x32,	// Struct binary compare-for-equal.
	EX_StructCmpNe          = 0x33,	// Struct binary compare-for-unequal.
	EX_UnicodeStringConst   = 0x34, // Unicode string constant.
	EX_StructMember         = 0x35, // Struct member.
	EX_DynArrayLength		= 0x36,	// A dynamic array length for setting/getting
	EX_GlobalFunction		= 0x37, // Call non-state version of a function.
	EX_PrimitiveCast		= 0x38,	// A casting operator for primitives which reads the type as the subsequent byte
	EX_DynArrayInsert		= 0x39,	// Inserts into a dynamic array
	EX_ReturnNothing		= 0x3A, // failsafe for functions that return a value - returns the zero value for a property and logs that control reached the end of a non-void function
	EX_EqualEqual_DelDel	= 0x3B,	// delegate comparison for equality
	EX_NotEqual_DelDel		= 0x3C, // delegate comparison for inequality
	EX_EqualEqual_DelFunc	= 0x3D,	// delegate comparison for equality against a function
	EX_NotEqual_DelFunc		= 0x3E,	// delegate comparison for inequality against a function
	EX_EmptyDelegate		= 0x3F,	// delegate 'None'
	EX_DynArrayRemove		= 0x40,	// Removes from a dynamic array
	EX_DebugInfo			= 0x41,	//DEBUGGER Debug information
	EX_DelegateFunction		= 0x42, // Call to a delegate function
	EX_DelegateProperty		= 0x43, // Delegate expression
	EX_LetDelegate			= 0x44, // Assignment to a delegate
	EX_Conditional			= 0x45, // tertiary operator support
	EX_DynArrayFind			= 0x46, // dynarray search for item index
	EX_DynArrayFindStruct	= 0x47, // dynarray<struct> search for item index
	EX_LocalOutVariable		= 0x48, // local out (pass by reference) function parameter
	EX_DefaultParmValue		= 0x49,	// default value of optional function parameter
	EX_EmptyParmValue		= 0x4A,	// unspecified value for optional function parameter
	EX_InstanceDelegate		= 0x4B,	// const reference to a delegate or normal function object




	EX_InterfaceContext		= 0x51,	// Call a function through a native interface variable
	EX_InterfaceCast		= 0x52,	// Converting an object reference to native interface variable
	EX_EndOfScript			= 0x53, // Last byte in script code
	EX_DynArrayAdd			= 0x54, // Add to a dynamic array
	EX_DynArrayAddItem		= 0x55, // Add an item to a dynamic array
	EX_DynArrayRemoveItem	= 0x56, // Remove an item from a dynamic array
	EX_DynArrayInsertItem	= 0x57, // Insert an item into a dynamic array
	EX_DynArrayIterator		= 0x58, // Iterate through a dynamic array
	EX_DynArraySort			= 0x59,	// Sort a list in place
	EX_JumpIfFilterEditorOnly	= 0x5A,	// Skip the code block if the editor is not present.

	// Natives.
	EX_ExtendedNative		= 0x60,
	EX_FirstNative			= 0x70,
	EX_Max					= 0x1000,

### defined in UObject beyond enum EExprToken

    IMPLEMENT_FUNCTION( UObject, 129, execNot_PreBool );
    IMPLEMENT_FUNCTION( UObject, 242, execEqualEqual_BoolBool );
    IMPLEMENT_FUNCTION( UObject, 243, execNotEqual_BoolBool );
    IMPLEMENT_FUNCTION( UObject, 130, execAndAnd_BoolBool );
    IMPLEMENT_FUNCTION( UObject, 131, execXorXor_BoolBool );
    IMPLEMENT_FUNCTION( UObject, 132, execOrOr_BoolBool );
    IMPLEMENT_FUNCTION( UObject, 133, execMultiplyEqual_ByteByte );
    IMPLEMENT_FUNCTION(UObject, 198, execMultiplyEqual_ByteFloat);
    IMPLEMENT_FUNCTION( UObject, 134, execDivideEqual_ByteByte );
    IMPLEMENT_FUNCTION( UObject, 135, execAddEqual_ByteByte );
    IMPLEMENT_FUNCTION( UObject, 136, execSubtractEqual_ByteByte );
    IMPLEMENT_FUNCTION( UObject, 137, execAddAdd_PreByte );
    IMPLEMENT_FUNCTION( UObject, 138, execSubtractSubtract_PreByte );
    IMPLEMENT_FUNCTION( UObject, 139, execAddAdd_Byte );
    IMPLEMENT_FUNCTION( UObject, 140, execSubtractSubtract_Byte );
    IMPLEMENT_FUNCTION( UObject, 141, execComplement_PreInt );
    IMPLEMENT_FUNCTION( UObject, 196, execGreaterGreaterGreater_IntInt );
    IMPLEMENT_FUNCTION( UObject, 143, execSubtract_PreInt );
    IMPLEMENT_FUNCTION( UObject, 144, execMultiply_IntInt );
    IMPLEMENT_FUNCTION( UObject, 145, execDivide_IntInt );
    IMPLEMENT_FUNCTION(UObject, 253, execPercent_IntInt);
    IMPLEMENT_FUNCTION( UObject, 146, execAdd_IntInt );
    IMPLEMENT_FUNCTION( UObject, 147, execSubtract_IntInt );
    IMPLEMENT_FUNCTION( UObject, 148, execLessLess_IntInt );
    IMPLEMENT_FUNCTION( UObject, 149, execGreaterGreater_IntInt );
    IMPLEMENT_FUNCTION( UObject, 150, execLess_IntInt );
    IMPLEMENT_FUNCTION( UObject, 151, execGreater_IntInt );
    IMPLEMENT_FUNCTION( UObject, 152, execLessEqual_IntInt );
    IMPLEMENT_FUNCTION( UObject, 153, execGreaterEqual_IntInt );
    IMPLEMENT_FUNCTION( UObject, 154, execEqualEqual_IntInt );
    IMPLEMENT_FUNCTION( UObject, 155, execNotEqual_IntInt );
    IMPLEMENT_FUNCTION( UObject, 156, execAnd_IntInt );
    IMPLEMENT_FUNCTION( UObject, 157, execXor_IntInt );
    IMPLEMENT_FUNCTION( UObject, 158, execOr_IntInt );
    IMPLEMENT_FUNCTION( UObject, 159, execMultiplyEqual_IntFloat );
    IMPLEMENT_FUNCTION( UObject, 160, execDivideEqual_IntFloat );
    IMPLEMENT_FUNCTION( UObject, 161, execAddEqual_IntInt );
    IMPLEMENT_FUNCTION( UObject, 162, execSubtractEqual_IntInt );
    IMPLEMENT_FUNCTION( UObject, 163, execAddAdd_PreInt );
    IMPLEMENT_FUNCTION( UObject, 164, execSubtractSubtract_PreInt );
    IMPLEMENT_FUNCTION( UObject, 165, execAddAdd_Int );
    IMPLEMENT_FUNCTION( UObject, 166, execSubtractSubtract_Int );
    IMPLEMENT_FUNCTION( UObject, 167, execRand );
    IMPLEMENT_FUNCTION( UObject, 249, execMin );
    IMPLEMENT_FUNCTION( UObject, 250, execMax );
    IMPLEMENT_FUNCTION( UObject, 251, execClamp );
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execToHex );
    IMPLEMENT_FUNCTION( UObject, 169, execSubtract_PreFloat );
    IMPLEMENT_FUNCTION( UObject, 170, execMultiplyMultiply_FloatFloat );
    IMPLEMENT_FUNCTION( UObject, 171, execMultiply_FloatFloat );
    IMPLEMENT_FUNCTION( UObject, 172, execDivide_FloatFloat );
    IMPLEMENT_FUNCTION( UObject, 173, execPercent_FloatFloat );
    IMPLEMENT_FUNCTION( UObject, 174, execAdd_FloatFloat );
    IMPLEMENT_FUNCTION( UObject, 175, execSubtract_FloatFloat );
    IMPLEMENT_FUNCTION( UObject, 176, execLess_FloatFloat );
    IMPLEMENT_FUNCTION( UObject, 177, execGreater_FloatFloat );
    IMPLEMENT_FUNCTION( UObject, 178, execLessEqual_FloatFloat );
    IMPLEMENT_FUNCTION( UObject, 179, execGreaterEqual_FloatFloat );
    IMPLEMENT_FUNCTION( UObject, 180, execEqualEqual_FloatFloat );
    IMPLEMENT_FUNCTION( UObject, 181, execNotEqual_FloatFloat );
    IMPLEMENT_FUNCTION( UObject, 210, execComplementEqual_FloatFloat );
    IMPLEMENT_FUNCTION( UObject, 182, execMultiplyEqual_FloatFloat );
    IMPLEMENT_FUNCTION( UObject, 183, execDivideEqual_FloatFloat );
    IMPLEMENT_FUNCTION( UObject, 184, execAddEqual_FloatFloat );
    IMPLEMENT_FUNCTION( UObject, 185, execSubtractEqual_FloatFloat );
    IMPLEMENT_FUNCTION( UObject, 186, execAbs );
    IMPLEMENT_FUNCTION( UObject, 187, execSin );
    IMPLEMENT_FUNCTION( UObject, -1, execAsin );
    IMPLEMENT_FUNCTION( UObject, 188, execCos );
    IMPLEMENT_FUNCTION( UObject, -1, execAcos );
    IMPLEMENT_FUNCTION( UObject, 189, execTan );
    IMPLEMENT_FUNCTION( UObject, 190, execAtan );
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execAtan2 );
    IMPLEMENT_FUNCTION( UObject, 191, execExp );
    IMPLEMENT_FUNCTION( UObject, 192, execLoge );
    IMPLEMENT_FUNCTION( UObject, 193, execSqrt );
    IMPLEMENT_FUNCTION( UObject, 194, execSquare );
    IMPLEMENT_FUNCTION( UObject, 199, execRound );
    IMPLEMENT_FUNCTION( UObject, -1, execFFloor );
    IMPLEMENT_FUNCTION( UObject, -1, execFCeil );
    IMPLEMENT_FUNCTION( UObject, 195, execFRand );
    IMPLEMENT_FUNCTION( UObject, 244, execFMin );
    IMPLEMENT_FUNCTION( UObject, 245, execFMax );
    IMPLEMENT_FUNCTION( UObject, 246, execFClamp );
    IMPLEMENT_FUNCTION( UObject, 247, execLerp );
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execFCubicInterp );
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execFInterpEaseInOut );
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execFInterpTo );
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execFInterpConstantTo );
    IMPLEMENT_FUNCTION( UObject, EX_RotationConst, execRotationConst );
    IMPLEMENT_FUNCTION( UObject, EX_VectorConst, execVectorConst );
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execPointDistToLine);
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execPointDistToSegment);
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execPointProjectToPlane);
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execGetDotDistance);
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execGetAngularDistance);
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execGetAngularFromDotDist);
    IMPLEMENT_FUNCTION( UObject, 0x80 + 83, execSubtract_PreVector );
    IMPLEMENT_FUNCTION( UObject, 0x80 + 84, execMultiply_VectorFloat );
    IMPLEMENT_FUNCTION( UObject, 0x80 + 85, execMultiply_FloatVector );
    IMPLEMENT_FUNCTION( UObject, 296, execMultiply_VectorVector );
    IMPLEMENT_FUNCTION( UObject, 0x80 + 86, execDivide_VectorFloat );
    IMPLEMENT_FUNCTION( UObject, 0x80 + 87, execAdd_VectorVector );
    IMPLEMENT_FUNCTION( UObject, 0x80 + 88, execSubtract_VectorVector );
    IMPLEMENT_FUNCTION( UObject, 275, execLessLess_VectorRotator );
    IMPLEMENT_FUNCTION( UObject, 276, execGreaterGreater_VectorRotator );
    IMPLEMENT_FUNCTION( UObject, 0x80 + 89, execEqualEqual_VectorVector );
    IMPLEMENT_FUNCTION( UObject, 0x80 + 90, execNotEqual_VectorVector );
    IMPLEMENT_FUNCTION( UObject, 0x80 + 91, execDot_VectorVector );
    IMPLEMENT_FUNCTION( UObject, 0x80 + 92, execCross_VectorVector );
    IMPLEMENT_FUNCTION( UObject, 0x80 + 93, execMultiplyEqual_VectorFloat );
    IMPLEMENT_FUNCTION( UObject, 297, execMultiplyEqual_VectorVector );
    IMPLEMENT_FUNCTION( UObject, 0x80 + 94, execDivideEqual_VectorFloat );
    IMPLEMENT_FUNCTION( UObject, 0x80 + 95, execAddEqual_VectorVector );
    IMPLEMENT_FUNCTION( UObject, 0x80 + 96, execSubtractEqual_VectorVector );
    IMPLEMENT_FUNCTION( UObject, 0x80 + 97, execVSize );
    IMPLEMENT_FUNCTION( UObject, -1, execVSize2D );
    IMPLEMENT_FUNCTION( UObject, 228, execVSizeSq );
    IMPLEMENT_FUNCTION( UObject, -1, execVSizeSq2D );
    IMPLEMENT_FUNCTION( UObject, 0x80 + 98, execNormal );
    IMPLEMENT_FUNCTION(UObject, 227, execNormal2D);
    IMPLEMENT_FUNCTION( UObject, -1, execVLerp );
    IMPLEMENT_FUNCTION( UObject, -1, execVInterpTo );
    IMPLEMENT_FUNCTION( UObject, -1, execClampLength );
    IMPLEMENT_FUNCTION( UObject, -1, execNoZDot );
    IMPLEMENT_FUNCTION( UObject, 0x80 + 124, execVRand );
    IMPLEMENT_FUNCTION( UObject, -1, execVRandCone );
    IMPLEMENT_FUNCTION( UObject, -1, execVRandCone2 );
    IMPLEMENT_FUNCTION( UObject, 320, execRotRand );
    IMPLEMENT_FUNCTION( UObject, 300, execMirrorVectorByNormal );
    IMPLEMENT_FUNCTION(UObject,1500,execProjectOnTo);
    IMPLEMENT_FUNCTION(UObject,1501,execIsZero);
    IMPLEMENT_FUNCTION( UObject, 0x80 + 14, execEqualEqual_RotatorRotator );
    IMPLEMENT_FUNCTION( UObject, 0x80 + 75, execNotEqual_RotatorRotator );
    IMPLEMENT_FUNCTION( UObject, 287, execMultiply_RotatorFloat );
    IMPLEMENT_FUNCTION( UObject, 288, execMultiply_FloatRotator );
    IMPLEMENT_FUNCTION( UObject, 289, execDivide_RotatorFloat );
    IMPLEMENT_FUNCTION( UObject, 290, execMultiplyEqual_RotatorFloat );
    IMPLEMENT_FUNCTION( UObject, 291, execDivideEqual_RotatorFloat );
    IMPLEMENT_FUNCTION( UObject, 316, execAdd_RotatorRotator );
    IMPLEMENT_FUNCTION( UObject, 317, execSubtract_RotatorRotator );
    IMPLEMENT_FUNCTION( UObject, 318, execAddEqual_RotatorRotator );
    IMPLEMENT_FUNCTION( UObject, 319, execSubtractEqual_RotatorRotator );
    IMPLEMENT_FUNCTION( UObject, 0x80 + 101, execGetAxes );
    IMPLEMENT_FUNCTION( UObject, 0x80 + 102, execGetUnAxes );
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execGetRotatorAxis );
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execOrthoRotation );
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execNormalize );
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execRLerp );
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execRTransform );
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execRInterpTo );
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execNormalizeRotAxis );
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execRDiff );
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execRSize );
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execClockwiseFrom_IntInt );
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execQuatProduct);
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execQuatDot);
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execQuatInvert);
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execQuatRotateVector);
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execQuatFindBetween);
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execQuatFromAxisAndAngle);
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execQuatFromRotator);
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execQuatToRotator);
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execQuatSlerp);
    IMPLEMENT_FUNCTION( UObject, 270, execAdd_QuatQuat);
    IMPLEMENT_FUNCTION( UObject, 271, execSubtract_QuatQuat);
    IMPLEMENT_FUNCTION(UObject, EX_EatReturnValue, execEatReturnValue);
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execAdd_Vector2DVector2D);
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execSubtract_Vector2DVector2D);
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execMultiply_Vector2DFloat);
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execDivide_Vector2DFloat);
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execMultiplyEqual_Vector2DFloat);
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execDivideEqual_Vector2DFloat);
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execAddEqual_Vector2DVector2D);
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execSubtractEqual_Vector2DVector2D);
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execGetMappedRangeValue);
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execEvalInterpCurveFloat);
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execEvalInterpCurveVector);
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execEvalInterpCurveVector2D);
    IMPLEMENT_FUNCTION( UObject, 112, execConcat_StrStr );
    IMPLEMENT_FUNCTION( UObject, 168, execAt_StrStr );
    IMPLEMENT_FUNCTION( UObject, 115, execLess_StrStr );
    IMPLEMENT_FUNCTION( UObject, 116, execGreater_StrStr );
    IMPLEMENT_FUNCTION( UObject, 120, execLessEqual_StrStr );
    IMPLEMENT_FUNCTION( UObject, 121, execGreaterEqual_StrStr );
    IMPLEMENT_FUNCTION( UObject, 122, execEqualEqual_StrStr );
    IMPLEMENT_FUNCTION( UObject, 123, execNotEqual_StrStr );
    IMPLEMENT_FUNCTION( UObject, 124, execComplementEqual_StrStr );
    IMPLEMENT_FUNCTION( UObject, 322, execConcatEqual_StrStr );
    IMPLEMENT_FUNCTION( UObject, 323, execAtEqual_StrStr );
    IMPLEMENT_FUNCTION( UObject, 324, execSubtractEqual_StrStr );
    IMPLEMENT_FUNCTION( UObject, 125, execLen );
    IMPLEMENT_FUNCTION( UObject, 126, execInStr );
    IMPLEMENT_FUNCTION( UObject, 127, execMid );
    IMPLEMENT_FUNCTION( UObject, 128, execLeft );
    IMPLEMENT_FUNCTION( UObject, 234, execRight );
    IMPLEMENT_FUNCTION( UObject, 235, execCaps );
    IMPLEMENT_FUNCTION(UObject,238,execLocs);
    IMPLEMENT_FUNCTION( UObject, 236, execChr );
    IMPLEMENT_FUNCTION( UObject, 237, execAsc );
    IMPLEMENT_FUNCTION( UObject, 201, execRepl );
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execParseStringIntoArray );
    IMPLEMENT_FUNCTION( UObject, 254, execEqualEqual_NameName );
    IMPLEMENT_FUNCTION( UObject, 255, execNotEqual_NameName );
    IMPLEMENT_FUNCTION( UObject, 114, execEqualEqual_ObjectObject );
    IMPLEMENT_FUNCTION( UObject, 119, execNotEqual_ObjectObject );
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execEqualEqual_InterfaceInterface );
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execNotEqual_InterfaceInterface );
    IMPLEMENT_FUNCTION( UObject, 231, execLogInternal );
    IMPLEMENT_FUNCTION( UObject, 232, execWarnInternal );
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execLocalize );
    IMPLEMENT_FUNCTION( UObject, 0x60 + n, execHighNative##n );
    IMPLEMENT_FUNCTION( UObject, EX_New, execNew );
    IMPLEMENT_FUNCTION( UObject, 258, execClassIsChildOf );
    IMPLEMENT_FUNCTION( UObject, 113, execGotoState );
    IMPLEMENT_FUNCTION(UObject, -1, execPushState);
    IMPLEMENT_FUNCTION(UObject, -1, execPopState);
    IMPLEMENT_FUNCTION(UObject,-1,execDumpStateStack);
    IMPLEMENT_FUNCTION( UObject, 117, execEnable );
    IMPLEMENT_FUNCTION( UObject, 118, execDisable );
    IMPLEMENT_FUNCTION( UObject, 536, execSaveConfig);
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execStaticSaveConfig);
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execImportJSON);
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execGetPerObjectConfigSections );
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execGetEnum);
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execEnumFromString);
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execDynamicLoadObject );
    IMPLEMENT_FUNCTION( UObject, INDEX_NONE, execFindObject );
    IMPLEMENT_FUNCTION( UObject, 281, execIsInState );
    IMPLEMENT_FUNCTION( UObject, -1, execIsChildState );
    IMPLEMENT_FUNCTION( UObject, 284, execGetStateName );
    IMPLEMENT_FUNCTION(UObject,-1,execGetFuncName);
    IMPLEMENT_FUNCTION(UObject,-1,execScriptTrace);
    IMPLEMENT_FUNCTION(UObject,-1,execGetScriptTrace);
    IMPLEMENT_FUNCTION(UObject,-1,execDebugBreak);
    IMPLEMENT_FUNCTION(UObject,INDEX_NONE,execSetUTracing);
    IMPLEMENT_FUNCTION(UObject,197,execIsA);
    IMPLEMENT_FUNCTION(UObject,-1,execPathName);
    IMPLEMENT_FUNCTION(UObject,INDEX_NONE,execIsPendingKill);
    IMPLEMENT_FUNCTION(UObject,INDEX_NONE,execTransformVectorByRotation);
    IMPLEMENT_FUNCTION(UObject,INDEX_NONE,execTimeStamp);
    IMPLEMENT_FUNCTION(UObject,INDEX_NONE,execGetSystemTime);
    IMPLEMENT_FUNCTION( UObject, EX_Iterator, execIterator );
    IMPLEMENT_FUNCTION( UObject, EX_DynArrayIterator, execDynArrayIterator );
    IMPLEMENT_FUNCTION( UObject, EX_JumpIfFilterEditorOnly, execJumpIfNotEditorOnly );
    IMPLEMENT_FUNCTION(UObject,INDEX_NONE,execGetEngineVersion);
    IMPLEMENT_FUNCTION(UObject,INDEX_NONE,execGetBuildChangelistNumber);
    IMPLEMENT_FUNCTION(UObject,INDEX_NONE,execInvalidateGuid);
    IMPLEMENT_FUNCTION(UObject,INDEX_NONE,execIsGuidValid);
    IMPLEMENT_FUNCTION(UObject,INDEX_NONE,execCreateGuid);
    IMPLEMENT_FUNCTION(UObject,INDEX_NONE,execGetGuidFromString);
    IMPLEMENT_FUNCTION(UObject,INDEX_NONE,execGetStringFromGuid);
    IMPLEMENT_FUNCTION(UObject,INDEX_NONE,execProfNodeStart);
    IMPLEMENT_FUNCTION(UObject,INDEX_NONE,execProfNodeStop);
    IMPLEMENT_FUNCTION(UObject,INDEX_NONE,execProfNodeSetTimeThresholdSeconds);
    IMPLEMENT_FUNCTION(UObject,INDEX_NONE,execProfNodeSetDepthThreshold);
    IMPLEMENT_FUNCTION(UObject,INDEX_NONE,execProfNodeEvent);

### defined in other classes

    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execUpdateRBBonesFromSpaceBases);
    IMPLEMENT_FUNCTION(UPrimitiveComponent,INDEX_NONE,execWakeRigidBody);
    IMPLEMENT_FUNCTION(UPrimitiveComponent,INDEX_NONE,execPutRigidBodyToSleep);
    IMPLEMENT_FUNCTION(UPrimitiveComponent,INDEX_NONE,execSetBlockRigidBody);
    IMPLEMENT_FUNCTION(UPrimitiveComponent,INDEX_NONE,execSetNotifyRigidBodyCollision);
    IMPLEMENT_FUNCTION(UPrimitiveComponent,INDEX_NONE,execSetPhysMaterialOverride);
    IMPLEMENT_FUNCTION(UPrimitiveComponent,INDEX_NONE,execSetRBDominanceGroup);
    IMPLEMENT_FUNCTION(UPrimitiveComponent,INDEX_NONE,execGetRootBodyInstance);
    IMPLEMENT_FUNCTION(UPrimitiveComponent,INDEX_NONE,execSetRBLinearVelocity);
    IMPLEMENT_FUNCTION(UPrimitiveComponent,INDEX_NONE,execSetRBAngularVelocity);
    IMPLEMENT_FUNCTION(UPrimitiveComponent,INDEX_NONE,execRetardRBLinearVelocity);
    IMPLEMENT_FUNCTION(UPrimitiveComponent,INDEX_NONE,execSetRBPosition);
    IMPLEMENT_FUNCTION(UPrimitiveComponent,INDEX_NONE,execSetRBRotation);
    IMPLEMENT_FUNCTION(UPrimitiveComponent,INDEX_NONE,execRigidBodyIsAwake);
    IMPLEMENT_FUNCTION(UPrimitiveComponent,INDEX_NONE,execAddImpulse);
    IMPLEMENT_FUNCTION(UPrimitiveComponent,INDEX_NONE,execAddRadialImpulse);
    IMPLEMENT_FUNCTION(UPrimitiveComponent,INDEX_NONE,execAddForce);
    IMPLEMENT_FUNCTION(UPrimitiveComponent,INDEX_NONE,execAddTorque);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execSetHasPhysicsAssetInstance);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execUpdateMeshForBrokenConstraints);
    IMPLEMENT_FUNCTION(ULightComponent,INDEX_NONE,execSetEnabled);
    IMPLEMENT_FUNCTION(ULightComponent,INDEX_NONE,execSetLightProperties);
    IMPLEMENT_FUNCTION(ULightComponent,INDEX_NONE,execGetOrigin);
    IMPLEMENT_FUNCTION(ULightComponent,INDEX_NONE,execGetDirection);
    IMPLEMENT_FUNCTION(ULightComponent,INDEX_NONE,execUpdateColorAndBrightness);
    IMPLEMENT_FUNCTION(ULightComponent,INDEX_NONE,execUpdateLightShaftParameters);
    IMPLEMENT_FUNCTION(UPrimitiveComponent,INDEX_NONE,execShouldComponentAddToScene);
    IMPLEMENT_FUNCTION(UPrimitiveComponent,INDEX_NONE,execSetHidden);
    IMPLEMENT_FUNCTION(UPrimitiveComponent,INDEX_NONE,execSetTranslation);
    IMPLEMENT_FUNCTION(UPrimitiveComponent,INDEX_NONE,execSetRotation);
    IMPLEMENT_FUNCTION(UPrimitiveComponent,INDEX_NONE,execSetScale);
    IMPLEMENT_FUNCTION(UPrimitiveComponent,INDEX_NONE,execSetScale3D);
    IMPLEMENT_FUNCTION(UPrimitiveComponent,INDEX_NONE,execSetAbsolute);
    IMPLEMENT_FUNCTION(UPrimitiveComponent,INDEX_NONE,execGetPosition);
    IMPLEMENT_FUNCTION(UPrimitiveComponent,INDEX_NONE,execGetRotation);
    IMPLEMENT_FUNCTION(UMeshComponent,INDEX_NONE,execGetMaterial);
    IMPLEMENT_FUNCTION(UMeshComponent,INDEX_NONE,execSetMaterial);
    IMPLEMENT_FUNCTION(UMeshComponent,INDEX_NONE,execGetNumElements);
    IMPLEMENT_FUNCTION(UCylinderComponent,INDEX_NONE,execSetCylinderSize);
    IMPLEMENT_FUNCTION( AActor, EPOLL_FinishAnim, execPollFinishAnim );
    IMPLEMENT_FUNCTION( UAudioComponent, INDEX_NONE, execPlay );
    IMPLEMENT_FUNCTION( UAudioComponent, INDEX_NONE, execStop );
    IMPLEMENT_FUNCTION( UAudioComponent, INDEX_NONE, execIsPlaying );
    IMPLEMENT_FUNCTION( UAudioComponent, INDEX_NONE, execIsFadingIn );
    IMPLEMENT_FUNCTION( UAudioComponent, INDEX_NONE, execIsFadingOut );
    IMPLEMENT_FUNCTION( UAudioComponent, INDEX_NONE, execSetFloatParameter );
    IMPLEMENT_FUNCTION( UAudioComponent, INDEX_NONE, execSetWaveParameter );
    IMPLEMENT_FUNCTION( UAudioComponent, INDEX_NONE, execFadeIn );
    IMPLEMENT_FUNCTION( UAudioComponent, INDEX_NONE, execFadeOut );
    IMPLEMENT_FUNCTION( UAudioComponent, INDEX_NONE, execAdjustVolume );
    IMPLEMENT_FUNCTION( AController, AI_PollWaitForLanding, execPollWaitForLanding);
    IMPLEMENT_FUNCTION( AController, AI_PollMoveTo, execPollMoveTo);
    IMPLEMENT_FUNCTION( AController, AI_PollMoveToward, execPollMoveToward);
    IMPLEMENT_FUNCTION( AController, AI_PollFinishRotation, execPollFinishRotation);
    IMPLEMENT_FUNCTION( AActor, EPOLL_Sleep, execPollSleep );
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execGetPosition);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execGetRotation);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execSetMaterial);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execFindAnimSequence);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execFindMorphTarget);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execAttachComponent);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execDetachComponent);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execAttachComponentToSocket);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execGetSocketWorldLocationAndRotation);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execGetSocketByName);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execFindComponentAttachedToBone);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execIsComponentAttached);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execGetTransformMatrix);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execSetSkeletalMesh);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execSetPhysicsAsset);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execSetForceRefPose);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execSetParentAnimComponent);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execGetBoneQuaternion);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execGetBoneLocation);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent, INDEX_NONE, execGetBoneAxis);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent, INDEX_NONE, execTransformToBoneSpace);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent, INDEX_NONE, execTransformFromBoneSpace);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execSetAnimTreeTemplate);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execUpdateParentBoneMap);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execInitSkelControls);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execInitMorphTargets);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent, INDEX_NONE, execAllAnimNodes);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execFindConstraintIndex);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execFindConstraintBoneName);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent, INDEX_NONE, execFindBodyInstanceNamed);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execForceSkelUpdate);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execUpdateAnimations);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent, INDEX_NONE, execPlayFaceFXAnim);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent, INDEX_NONE, execStopFaceFXAnim);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent, INDEX_NONE, execIsPlayingFaceFXAnim);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent, INDEX_NONE, execDeclareFaceFXRegister);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent, INDEX_NONE, execGetFaceFXRegister);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent, INDEX_NONE, execSetFaceFXRegister);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent, INDEX_NONE, execSetFaceFXRegisterEx);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execGetBonesWithinRadius);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execHideBone);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execUnHideBone);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execIsBoneHidden);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execHideBoneByName);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execUnHideBoneByName);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execAddInstanceVertexWeightBoneParented);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execRemoveInstanceVertexWeightBoneParented);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execFindInstanceVertexweightBonePair);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execUpdateInstanceVertexWeightBones);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execToggleInstanceVertexWeights);
    IMPLEMENT_FUNCTION(USkeletalMeshComponent,INDEX_NONE,execShowMaterialSection);