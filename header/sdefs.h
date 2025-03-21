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
MEMORY_UNALIGNED

ASTRUCT
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

#if defined(_WIN64) && (defined(_M_X64) || defined(_M_AMD64)) && !defined(_M_ARM64EC)
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
#elif defined(_M_ARM64) || defined(_M_ARM64EC)
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

typedef UInt8 UTF8Ch;
#if defined(__cplusplus) && __cplusplus >= 201103L
#if _WCHAR_SIZE == 4
typedef char16_t UTF16Ch;
typedef WChar UTF32Ch;
#else
typedef WChar UTF16Ch;
typedef char32_t UTF32Ch;
#endif
#else
#if _WCHAR_SIZE == 4
typedef UInt16 UTF16Ch;
typedef WChar UTF32Ch;
#else
typedef WChar UTF16Ch;
typedef UInt32 UTF32Ch;
#endif
#endif

#define UTF8Char UTF8Ch
#define UTF16Char UTF16Ch
#define UTF32Char UTF32Ch

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
#elif defined(__aarch64__)
#define HAS_GCCASMA64
#elif defined(__arm__)
#define HAS_GCCASMA32
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

UInt64 __inline BSWAP64(UInt64 v)
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
#define BSWAPU64(v) __builtin_bswap64(v)
#define MyADC_UOS(v1, v2, outPtr) __builtin_add_overflow(v1, v2, outPtr)

__inline UOSInt MyMUL_UOS(UOSInt x, UOSInt y, UOSInt* hi)
{
    __asm__(
        "mulq %3	\n\t"
        : "=&a"(x), "=&d"(y)
        : "0"(x), "1"(y)
        : "cc"
    );

    *hi = y;
    return x;
}

__inline UOSInt MyDIV_UOS(UOSInt lo, UOSInt hi, UOSInt divider, UOSInt *reminder)
{
	unsigned __int128 v = (lo | (((unsigned __int128)hi) << 64));
	*reminder = (UOSInt)(v % divider);
    return (UOSInt)(v / divider);
}

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
#define BSWAPU64(v) __builtin_bswap64(v)
#define MyADC_UOS(v1, v2, outPtr) __builtin_add_overflow(v1, v2, outPtr)

UOSInt __inline MyMUL_UOS(UOSInt x, UOSInt y, UOSInt* hi)
{
	UInt64 v = ((UInt64)x * (UInt64)y);
	*hi = (UOSInt)(v >> 32);
	return (UInt32)v;
}

__inline UOSInt MyDIV_UOS(UOSInt lo, UOSInt hi, UOSInt divider, UOSInt *reminder)
{
	UInt64 v = (lo | (((UInt64)hi) << 32));
	*reminder = (UOSInt)(v % divider);
    return (UOSInt)(v / divider);
}

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
#define BSWAPU64(v) _byteswap_uint64(v)
#if _OSINT_SIZE == 64
#if defined(_M_ARM64) || defined(_M_ARM64EC)
#include <intrin.h>
Bool __inline MyADC_UOS(UOSInt v1, UOSInt v2, UOSInt* outPtr)
{
	v1 += v2;
	*outPtr = v1;
	return v1 < v2;
}

UOSInt __inline MyMUL_UOS(UOSInt x, UOSInt y, UOSInt* hi)
{
	*hi = __umulh(x, y);
	return (x * y);
}

__inline UOSInt MyDIV_UOS(UOSInt lo, UOSInt hi, UOSInt divider, UOSInt* reminder)
{
	hi = hi % divider;
	UOSInt i = 64;
	UOSInt ret = 0;
	while (i--)
	{
		ret <<= 1;
		hi = (hi << 1) | (lo >> 63);
		if (hi >= divider)
		{
			hi -= divider;
		}
		lo <<= 1;
	}
	*reminder = hi;
	return ret;
}
#else
#define MyADC_UOS(v1, v2, out) _addcarry_u64(0, v1, v2, out);
#define MyMUL_UOS(x, y, hi) _umul128(x, y, hi)
#define MyDIV_UOS(lo, hi, divider, reminder) _udiv128(hi, lo, divider, reminder)
#endif
#else
#define MyADC_UOS(v1, v2, out) _addcarry_u32(0, v1, v2, out);
#endif
#elif defined(__APPLE__)
#include <libkern/OSByteOrder.h>
#define BSWAP32(x) OSSwapInt32(x)
#define BSWAPU32(x) OSSwapInt32(x)
#define BSWAP64(x) OSSwapInt64(x)
#define BSWAPU64(x) OSSwapInt64(x)
#define MyADC_UOS(v1, v2, outPtr) __builtin_add_overflow(v1, v2, outPtr)

UOSInt __inline MyMUL_UOS(UOSInt x, UOSInt y, UOSInt* hi)
{
	UInt64 v = ((UInt64)x * (UInt64)y);
	*hi = (UOSInt)(v >> 32);
	return (UInt32)v;
}

__inline UOSInt MyDIV_UOS(UOSInt lo, UOSInt hi, UOSInt divider, UOSInt *reminder)
{
	UInt64 v = (lo | (((UInt64)hi) << 32));
	*reminder = (UOSInt)(v % divider);
    return (UOSInt)(v / divider);
}

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
#elif defined(__sun) || defined(sun)
#include <sys/byteorder.h>
#define BSWAP32(x) BSWAP_32(x)
#define BSWAPU32(x) BSWAP_32(x)
#define BSWAP64(x) BSWAP_64(x)
#define BSWAPU64(x) BSWAP_64(x)
#elif defined(__FreeBSD__)
#include <sys/endian.h>
#define BSWAP32(x) bswap32(x)
#define BSWAPU32(x) bswap32(x)
#define BSWAP64(x) bswap64(x)
#define BSWAPU64(x) bswap64(x)
#elif defined(__OpenBSD__)
#include <sys/types.h>
#define BSWAP32(x) swap32(x)
#define BSWAPU32(x) swap32(x)
#define BSWAP64(x) swap64(x)
#define BSWAPU64(x) swap64(x)
#elif defined(__GNUC__)
#define BSWAP32(v) (Int32)__builtin_bswap32((UInt32)(v))
#define BSWAPU32(v) __builtin_bswap32(v)
#define BSWAP64(v) (Int64)__builtin_bswap64((UInt64)(v))
#define BSWAPU64(v) __builtin_bswap64(v)
#if __GNUC__ >= 5
#define MyADC_UOS(v1, v2, outPtr) __builtin_add_overflow(v1, v2, outPtr)
#else
Bool __inline MyADC_UOS(UOSInt v1, UOSInt v2, UOSInt* outPtr)
{
	v1 += v2;
	*outPtr = v1;
	return v1 < v2;
}
#endif

#if _OSINT_SIZE == 64
__inline UOSInt MyMUL_UOS(UOSInt x, UOSInt y, UOSInt* hi)
{
	unsigned __int128 v = (x * (unsigned __int128)y);
    *hi = (UOSInt)(v >> 64);
    return (UOSInt)v;
}

__inline UOSInt MyDIV_UOS(UOSInt lo, UOSInt hi, UOSInt divider, UOSInt *reminder)
{
	unsigned __int128 v = (lo | (((unsigned __int128)hi) << 64));
	*reminder = (UOSInt)(v % divider);
    return (UOSInt)(v / divider);
}
#else
UOSInt __inline MyMUL_UOS(UOSInt x, UOSInt y, UOSInt* hi)
{
	UInt64 v = ((UInt64)x * (UInt64)y);
	*hi = (UOSInt)(v >> 32);
	return (UInt32)v;
}

__inline UOSInt MyDIV_UOS(UOSInt lo, UOSInt hi, UOSInt divider, UOSInt *reminder)
{
	UInt64 v = (lo | (((UInt64)hi) << 32));
	*reminder = (UOSInt)(v % divider);
    return (UOSInt)(v / divider);
}
#endif

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
#define UTF8STRC(s) U8STR(s), U8STRLEN(s)
#define UTF8STRCPTR(s) U8STRPTR(s), U8STRLEN(s)
#define UTF8STR_NULL (const UTF8Char*)0, 0

#if defined(__GNUC__) && (__cplusplus < 201703L) && (__STDC_VERSION__ < 201703L) && !defined(__clang__)
#define REGVAR register
#else
#define REGVAR
#endif

#if defined(_MSC_VER)
#define ASTRUCT __declspec(align(32)) struct
#else
#define ASTRUCT struct
#endif

#define CHSTR(s) UnsafeArray<const Char>::FromPtrNoCheck(s)
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
#define U8STR(s) UnsafeArray<const UTF8Char>::FromPtrNoCheck((const UTF8Char*)(u8 ## s))
#define U8STRPTR(s) ((const UTF8Char*)(u8 ## s))
#define U8STRLEN(s) (sizeof(u8 ## s) - 1)
#define U16STR(s) ((const UTF16Char*)(u ## s))
#define U32STR(s) ((const UTF32Char*)(U ## s))
#else
#define U8STR(s) UnsafeArray<const UTF8Char>::FromPtrNoCheck((const UTF8Char*)s)
#define U8STRPTR(s) ((const UTF8Char*)s)
#define U8STRLEN(s) (sizeof(s) - 1)
#if _WCHAR_SIZE == 4
#define U32STR(s) ((const UTF32Char*)(L ## s))
#else
#define U16STR(s) ((const UTF16Char*)(L ## s))
#endif
#endif

#if defined(CPU_X86_32) || defined(CPU_X86_64) || defined(CPU_ARM64) || (defined(CPU_ARM) && defined(__ARM_FEATURE_UNALIGNED)) || (defined(_MSC_VER) && !defined(WIN32_WCE))
#define MEMORY_UNALIGNED 1
#else
#define MEMORY_UNALIGNED 0
#endif

#define CALLBACKFUNC __stdcall *
#endif
