/* Defines
REGCALL
FORCEINLINE
REGVAR
_WCHAR_SIZE
_OSINT_SIZE
CPU_X86_32
CPU_X86_64
CPU_ARM
CPU_ARM64
CPU_MIPS
CPU_AVR
HAS_ASM32
HAS_GCCASM32
HAS_GCCASM64
HAS_INT64
HAS_DOUBLE
HAS_INTRIN
IS_BYTEORDER_LE
*/
#if !defined(SDEFS_INCLUDED)
#define SDEFS_INCLUDED

#define REGCALL __fastcall

#ifndef FORCEINLINE
#if (_MSC_VER >= 1200)
#define FORCEINLINE __forceinline
#else
#define FORCEINLINE __inline
#endif
#endif


#if defined(__WCHAR_MAX__) && (__WCHAR_MAX__ >= 0x10000)
#define _WCHAR_SIZE 4
#elif !defined(__WCHAR_MAX__) || (__WCHAR_MAX__ >= 0x100)
#define _WCHAR_SIZE 2
#else
#define _WCHAR_SIZE 1
#endif

#if defined(__UINT32_TYPE__)
typedef __INT32_TYPE__ Int32;
typedef __UINT32_TYPE__ UInt32;
#else
typedef int Int32;
typedef unsigned int UInt32;
#endif

#if defined(_MSC_VER)
typedef __int64 Int64;
typedef unsigned __int64 UInt64;
#define HAS_INT64
#elif !defined(__UINTMAX_MAX__) || __UINTMAX_MAX__ > 0xffffffffULL || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L)
__extension__
typedef long long int Int64;
__extension__
typedef unsigned long long int UInt64;
#ifndef __stdcall
#define __stdcall
#endif
#ifndef __cdecl
#define __cdecl
#endif
#define HAS_INT64
#else
#define __stdcall
#endif

#if defined(__UINT16_TYPE__)
typedef __INT16_TYPE__ Int16;
typedef __UINT16_TYPE__ UInt16;
#else
typedef signed short Int16;
typedef unsigned short UInt16;
#endif
typedef char Char;
typedef unsigned char UInt8;
typedef signed char Int8;
typedef bool Bool;

typedef float Single;
#if !defined(__SIZEOF_DOUBLE__) || __SIZEOF_DOUBLE == 8
#define HAS_DOUBLE
typedef long double LDouble; //80-bit floating point
typedef double Double;
#else
typedef long double LDouble; //32-bit floating point
typedef double Double;
#endif

#if defined(_WIN64) && (defined(_M_X64) || defined(_M_AMD64))
typedef __int64 OSInt;
typedef unsigned __int64 UOSInt;
#define _OSINT_SIZE 64
#define CPU_X86_64
#define IS_BYTEORDER_LE 1
#elif defined(__x86_64__)
typedef long long OSInt;
typedef unsigned long long UOSInt;
#define _OSINT_SIZE 64
#define CPU_X86_64
#define IS_BYTEORDER_LE 1
#elif defined(_M_ARM64)
typedef __int64 OSInt;
typedef unsigned __int64 UOSInt;
#define _OSINT_SIZE 64
#define CPU_ARM64
#define IS_BYTEORDER_LE 1
#elif defined(__aarch64__) 
typedef long long OSInt;
typedef unsigned long long UOSInt;
#define _OSINT_SIZE 64
#define CPU_ARM64
#define IS_BYTEORDER_LE 1
#elif defined(_MSC_VER)
#if defined(_WIN32_WCE)
#define __w64
#endif
#if defined(ARM) || defined(_M_ARM)
typedef Int32 OSInt;
typedef UInt32 UOSInt;
#define _OSINT_SIZE 32
#define CPU_ARM
#define IS_BYTEORDER_LE 1
#else
typedef __w64 int OSInt;
typedef __w64 unsigned int UOSInt;
#define _OSINT_SIZE 32
#define CPU_X86_32
#define IS_BYTEORDER_LE 1
#endif
#elif defined(__i386__)
typedef Int32 OSInt;
typedef UInt32 UOSInt;
#define _OSINT_SIZE 32
#define CPU_X86_32
#define IS_BYTEORDER_LE 1
#elif defined(__arm__) || defined(__arm__) || defined(ARM)
typedef Int32 OSInt;
typedef UInt32 UOSInt;
#define _OSINT_SIZE 32
#define CPU_ARM
#define IS_BYTEORDER_LE 1
#elif defined(__mips__)
typedef Int32 OSInt;
typedef UInt32 UOSInt;
#define _OSINT_SIZE 32
#define CPU_MIPS
#if defined(__MIPSEL__)
#define IS_BYTEORDER_LE 1
#else
#define IS_BYTEORDER_LE 0
#endif
#elif defined(AVR)
typedef Int16 OSInt;
typedef UInt16 UOSInt;
#define _OSINT_SIZE 16
#define CPU_AVR
#define IS_BYTEORDER_LE 1
#else
#error "Unknown CPU"
#endif

#if defined(_WIN64)
#define __OSINT_64
#endif

#if defined(__GNUC__)
typedef wchar_t WChar;
#elif defined(_WCHAR_T_DEFINED)
typedef wchar_t WChar;
#else
typedef unsigned short WChar;
#endif

#if defined(_MSC_VER) && !defined(_WIN32_WCE)
#if defined(CPU_X86_32)
#define HAS_ASM32
#elif _MSC_VER >= 1400
#define HAS_INTRIN
#endif
#elif defined(__GNUC__)
#if defined(__x86_64__)
#define HAS_GCCASM64
#elif defined(__i386__)
#define HAS_GCCASM32
#endif
#define HAS_INTRIN
#endif

#if defined(_WIN32_WCE)
#if (_WIN32_WCE <= 0x500)
#define MulDiv32(x, y, z) ((x) * (y) / (z))
#define MulDivOS(x, y, z) ((x) * (y) / (z))
#endif
#endif
#define _CRT_SECURE_NO_WARNINGS 1

#ifdef HAS_ASM32
Int32 __inline BSWAP32(Int32 v)
{
	_asm
	{
		mov eax,v
		bswap eax
	}
}

UInt32 __inline BSWAPU32(UInt32 v)
{
	_asm
	{
		mov eax, v
		bswap eax
	}
}

Int64 __inline BSWAP64(Int64 v)
{
	_asm
	{
		mov eax,dword ptr [v + 4]
		mov edx,dword ptr [v]
		bswap eax
		bswap edx
	}
}

Int32 __inline MulDiv32(Int32 x, Int32 y, Int32 z)
{
	_asm
	{
		mov eax,x
		imul y
		idiv z
	}
}

UInt32 __inline MulDivU32(UInt32 x, UInt32 y, UInt32 z)
{
	_asm
	{
		mov eax,x
		mul y
		div z
	}
}

OSInt __inline MulDivOS(OSInt x, OSInt y, OSInt z)
{
	_asm
	{
		mov eax,x
		imul y
		idiv z
	}
}

UOSInt __inline MulDivUOS(UOSInt x, UOSInt y, UOSInt z)
{
	_asm
	{
		mov eax,x
		mul y
		div z
	}
}
#elif defined(HAS_GCCASM64)
/*
Int32 __inline BSWAP32(Int32 v)
{
	asm("bswapl %0" : "=r" (v) : "0" (v));
    return v;
}

UInt32 __inline BSWAPU32(UInt32 v)
{
	asm("bswapl %0" : "=r" (v) : "0" (v));
    return v;
}

Int64 __inline BSWAP64(Int64 v)
{
	asm("bswapq %0" : "=r" (v) : "0" (v));
    return v;
}*/

#define BSWAP32(v) (Int32)__builtin_bswap32((UInt32)(v))
#define BSWAPU32(v) __builtin_bswap32(v)
#define BSWAP64(v) (Int64)__builtin_bswap64((UInt64)(v))

Int32 __inline MulDiv32(Int32 x, Int32 y, Int32 z)
{
	 return (Int32)(((Int64)x * (Int64)y) / z);
}

UInt32 __inline MulDivU32(UInt32 x, UInt32 y, UInt32 z)
{
	 return (UInt32)(((UInt64)x * (UInt64)y) / z);
}

OSInt __inline MulDivOS(OSInt x, OSInt y, OSInt z)
{
	 return (OSInt)(((__int128)x * (__int128)y) / z);
}

UOSInt __inline MulDivUOS(UOSInt x, UOSInt y, UOSInt z)
{
	 return (UOSInt)(((unsigned __int128)x * (unsigned __int128)y) / z);
}
#elif defined(HAS_GCCASM32)
#define BSWAP32(v) (Int32)__builtin_bswap32((UInt32)(v))
#define BSWAPU32(v) __builtin_bswap32(v)
#define BSWAP64(v) (Int64)__builtin_bswap64((UInt64)(v))

Int32 __inline MulDiv32(Int32 x, Int32 y, Int32 z)
{
	return (Int32)(((Int64)x * (Int64)y) / z);
}

UInt32 __inline MulDivU32(UInt32 x, UInt32 y, UInt32 z)
{
	return (UInt32)(((UInt64)x * (UInt64)y) / z);
}

OSInt __inline MulDivOS(OSInt x, OSInt y, OSInt z)
{
	return (Int32)(((Int64)x * (Int64)y) / z);
}

UOSInt __inline MulDivUOS(UOSInt x, UOSInt y, UOSInt z)
{
	 return (UInt32)(((UInt64)x * (UInt64)y) / z);
}
#else
#if defined(_MSC_VER)
#include <stdlib.h>

#define BSWAP32(v) (Int32)_byteswap_ulong((UInt32)(v))
#define BSWAPU32(v) _byteswap_ulong(v)
#define BSWAP64(v) (Int64)_byteswap_uint64((UInt64)(v))
#elif defined(__APPLE__)
#include <libkern/OSByteOrder.h>
#define BSWAP32(x) OSSwapInt32(x)
#define BSWAPU32(x) OSSwapInt32(x)
#define BSWAP64(x) OSSwapInt64(x)
#elif defined(__sun) || defined(sun)
#include <sys/byteorder.h>
#define BSWAP32(x) BSWAP_32(x)
#define BSWAPU32(x) BSWAP_32(x)
#define BSWAP64(x) BSWAP_64(x)
#elif defined(__FreeBSD__)
#include <sys/endian.h>
#define BSWAP32(x) bswap32(x)
#define BSWAPU32(x) bswap32(x)
#define BSWAP64(x) bswap64(x)
#elif defined(__OpenBSD__)
#include <sys/types.h>
#define BSWAP32(x) swap32(x)
#define BSWAPU32(x) swap32(x)
#define BSWAP64(x) swap64(x)
#else
#define BSWAP32(x) \
    ((Int32)( (( (UInt32)x          ) << 24) | \
      ((((UInt32)x)&0xff00  ) << 8 ) | \
      ((((UInt32)x)&0xff0000) >> 8 ) | \
      (( (UInt32)x          ) >> 24)  ))
#define BSWAPU32(x) \
    (( (( (UInt32)x          ) << 24) | \
      ((((UInt32)x)&0xff00  ) << 8 ) | \
      ((((UInt32)x)&0xff0000) >> 8 ) | \
      (( (UInt32)x          ) >> 24)  ))
#endif
#define MulDiv32(x, y, z) (Int32)((Int64)(x) * (Int64)(y) / (z))
#define MulDivU32(x, y, z) (UInt32)((UInt64)(x) * (UInt64)(y) / (z))
#define MulDivOS(x, y, z) ((x) * (y) / (z))
#define MulDivUOS(x, y, z) ((x) * (y) / (z))
#endif

typedef UInt8 UTF8Char;
#if _WCHAR_SIZE == 4
typedef UInt16 UTF16Char;
typedef WChar UTF32Char;
#else
typedef WChar UTF16Char;
typedef UInt32 UTF32Char;
#endif

#define Double2Int32(val) (((val) < 0)?(Int32)(val - 0.5):(Int32)(val + 0.5))
#define Double2Int64(val) (((val) < 0)?(Int64)(val - 0.5):(Int64)(val + 0.5))
#if _OSINT_SIZE == 64
#define Double2OSInt(val) (OSInt)Double2Int64(val)
#else
#define Double2OSInt(val) (OSInt)Double2Int32(val)
#endif
#define OSInt2Double(val) ((Double)(val))
#define UOSInt2Double(val) ((Double)(val))
#define Int64_Double(val) ((Double)(val))
#define UInt64_Double(val) ((Double)(val))

#define ROR32(x, n) ((x >> n) | (x << (32 - n)))
#define ROR64(x, n) ((x >> n) | (x << (64 - n)))
#define INVALID_INDEX ((UOSInt)-1)
#define UTF8STRC(s) (const UTF8Char*)s, (sizeof(s) - 1)

#if defined(__GNUC__)
#define REGVAR register
#else
#define REGVAR
#endif
#endif
