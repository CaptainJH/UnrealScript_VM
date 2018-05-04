#pragma once

typedef unsigned char		BYTE;
typedef wchar_t				TCHAR;
typedef wchar_t				UNICHAR;
typedef int	INT;
typedef unsigned int        UINT;
typedef float FLOAT;

typedef unsigned long       DWORD;
typedef signed __int64		SQWORD;		// 64-bit signed.
typedef int                 BOOL;
typedef unsigned short      WORD;
typedef unsigned __int64	QWORD;		// 64-bit unsigned.
typedef float               FLOAT;
typedef UINT				UBOOL;		// Boolean 0 (false) or 1 (true).
typedef unsigned long       BITFIELD;	// For bitfields.
typedef SQWORD				PTRINT;		// Integer large enough to hold a pointer.

typedef WORD				CodeSkipSizeType;
typedef	QWORD				ScriptPointerType;
typedef WORD				VariableSizeType;

#define TRUE 1
#define FALSE 0

//
// UnrealScript intrinsic return value declaration.
//
#define RESULT_DECL void*const Result

enum { INDEX_NONE = -1 };

#define MINCHAR     0x80        
#define MAXCHAR     0x7f        
#define MINSHORT    0x8000      
#define MAXSHORT    0x7fff      
#define MINLONG     0x80000000  
#define MAXLONG     0x7fffffff  
#define MAXBYTE     0xff        
#define MAXWORD     0xffff      
#define MAXDWORD    0xffffffff 
