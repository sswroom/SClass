#if defined(__CYGWIN__)
#define _vsnprintf(a, b, c, d) vsprintf(a, c, d)
#define _vsnwprintf(a, b, c, d) vswprintf(a, b, c, d)
#include <wchar.h>
#endif
#include <tchar.h>
#if defined(__CYGWIN__) || defined(__MINGW32__)
//#include <initguid.h>
//#include <cguid.h>

inline unsigned long min(unsigned long a, unsigned long b) {return (a > b)?b:a;}

#define __deref_in
#define __deref_out
#define __deref_out_opt
#define __deref_inout_opt
#define __field_ecount_opt(a)
#define __in_bcount(a)
#define __in_bcount_opt(a)
#define __out_bcount(a)
#define __out_bcount_part(a, b)
#define __in_ecount(a)
#define __in_ecount_opt(a)
#define __out_ecount(a)
#define __out_ecount_part(a, b)
#define __inout_ecount_full(a)
#define __in
#define __in_opt
#define __out
#define __out_opt
#define __inout
#define __inout_opt
#define __range(a, b)
#define AM_NOVTABLE
#elif _MSC_VER <= 1600
#define PTCHAR (TCHAR *)
#endif
#if defined(__MINGW32__) || defined(__CYGWIN__)
constexpr long long int operator "" I64 (unsigned long long int v)
{
	return (long long int)v;
}
#endif