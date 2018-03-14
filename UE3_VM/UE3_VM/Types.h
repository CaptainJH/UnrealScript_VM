#pragma once

typedef unsigned char BYTE;
typedef int	INT;
typedef float FLOAT;

typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned short      WORD;
typedef unsigned __int64	QWORD;		// 64-bit unsigned.
typedef float               FLOAT;

typedef WORD				CodeSkipSizeType;
typedef	QWORD				ScriptPointerType;

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
