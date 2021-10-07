#include "Stdafx.h"
#include "Sync/Interlocked.h"

#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__)
#include <windows.h>

Int32 Sync::Interlocked::Increment(Int32 *val)
{
	return InterlockedIncrement((LONG*)val);
}

UInt32 Sync::Interlocked::Increment(UInt32 *val)
{
	return InterlockedIncrement((ULONG*)val);
}

Int32 Sync::Interlocked::Decrement(Int32 *val)
{
	return InterlockedDecrement((LONG*)val);
}

UInt32 Sync::Interlocked::Decrement(UInt32 *val)
{
	return InterlockedDecrement((ULONG*)val);
}

Int32 Sync::Interlocked::Add(Int32 *val, Int32 aval)
{
	return InterlockedExchangeAdd((LONG*)val, (LONG)aval) + aval;
}

Int32 Sync::Interlocked::Subtract(Int32 *val, Int32 sval)
{
	return InterlockedExchangeAdd((LONG*)val, -sval) - sval;
}

#if defined(_WIN64)
Int64 Sync::Interlocked::Increment(Int64 *val)
{
	return InterlockedIncrement64(val);
}

UInt64 Sync::Interlocked::Increment(UInt64 *val)
{
	return (UInt64)InterlockedIncrement64((Int64*)val);
}

Int64 Sync::Interlocked::Decrement(Int64 *val)
{
	return InterlockedDecrement64(val);
}

UInt64 Sync::Interlocked::Decrement(UInt64 *val)
{
	return (UInt64)InterlockedDecrement64((Int64*)val);
}

Int64 Sync::Interlocked::Add(Int64 *val, Int64 aval)
{
	return InterlockedExchangeAdd64(val, aval) + aval;
}

UInt64 Sync::Interlocked::Add(UInt64 *val, UInt64 aval)
{
	return (UInt64)Add((Int64*)val, (Int64)aval);
}

Int64 Sync::Interlocked::Subtract(Int64 *val, Int64 sval)
{
	return InterlockedExchangeAdd64(val, -sval) - sval;
}

#else
Int64 Sync::Interlocked::Increment(Int64 *val)
{
	return ++*val;
}

UInt64 Sync::Interlocked::Increment(UInt64 *val)
{
	return ++*val;
}

Int64 Sync::Interlocked::Decrement(Int64 *val)
{
	return --*val;
}

UInt64 Sync::Interlocked::Decrement(UInt64 *val)
{
	return --*val;
}

Int64 Sync::Interlocked::Add(Int64 *val, Int64 aval)
{
	*val += aval;
	return *val;
}

UInt64 Sync::Interlocked::Add(UInt64 *val, UInt64 aval)
{
	return (UInt64)Add((Int64*)val, (Int64)aval);
}

Int64 Sync::Interlocked::Subtract(Int64 *val, Int64 sval)
{
	*val -= sval;
	return *val;
}
#endif

#elif defined(__GNUC__) && !(defined(_MIPSEL) && defined(__ANDROID_API__)) && !defined(_R3000) && !defined(AVR) && !defined(__ARM_ARCH_5T__) && !(defined(__ARM_ARCH_5TEJ__) && (__STDC_VERSION__ == 201112L))

Int32 Sync::Interlocked::Increment(Int32 *val)
{
	return __sync_add_and_fetch(val, 1);
}

Int64 Sync::Interlocked::Increment(Int64 *val)
{
	return __sync_add_and_fetch(val, 1);
}

UInt32 Sync::Interlocked::Increment(UInt32 *val)
{
	return __sync_add_and_fetch(val, 1);
}

UInt64 Sync::Interlocked::Increment(UInt64 *val)
{
	return __sync_add_and_fetch(val, 1);
}

Int32 Sync::Interlocked::Decrement(Int32 *val)
{
	return __sync_sub_and_fetch(val, 1);
}

Int64 Sync::Interlocked::Decrement(Int64 *val)
{
	return __sync_sub_and_fetch(val, 1);
}

UInt32 Sync::Interlocked::Decrement(UInt32 *val)
{
	return __sync_sub_and_fetch(val, 1);
}

UInt64 Sync::Interlocked::Decrement(UInt64 *val)
{
	return __sync_sub_and_fetch(val, 1);
}

Int32 Sync::Interlocked::Add(Int32 *val, Int32 aval)
{
	return __sync_add_and_fetch(val, aval);
}

Int64 Sync::Interlocked::Add(Int64 *val, Int64 aval)
{
	return __sync_add_and_fetch(val, aval);
}

UInt32 Sync::Interlocked::Add(UInt32 *val, UInt32 aval)
{
	return __sync_add_and_fetch(val, aval);
}

UInt64 Sync::Interlocked::Add(UInt64 *val, UInt64 aval)
{
	return __sync_add_and_fetch(val, aval);
}

Int32 Sync::Interlocked::Subtract(Int32 *val, Int32 sval)
{
	return __sync_sub_and_fetch(val, sval);
}

Int64 Sync::Interlocked::Subtract(Int64 *val, Int64 sval)
{
	*val -= sval;
	return *val;
}
#elif defined(_MIPSEL) && (defined(__ANDROID_API__) || defined(_R3000))
Int32 Sync::Interlocked::Increment(Int32 *val)
{
	return __sync_add_and_fetch(val, 1);
}

Int64 Sync::Interlocked::Increment(Int64 *val)
{
	return ++*val;
}

Int32 Sync::Interlocked::Decrement(Int32 *val)
{
	return __sync_sub_and_fetch(val, 1);
}

Int64 Sync::Interlocked::Decrement(Int64 *val)
{
	return --*val;
}

Int32 Sync::Interlocked::Add(Int32 *val, Int32 aval)
{
	return __sync_add_and_fetch(val, aval);
}

Int64 Sync::Interlocked::Add(Int64 *val, Int64 aval)
{
	*val += aval;
	return *val;
}

Int32 Sync::Interlocked::Subtract(Int32 *val, Int32 sval)
{
	return __sync_sub_and_fetch(val, sval);
}

Int64 Sync::Interlocked::Subtract(Int64 *val, Int64 sval)
{
	*val -= sval;
	return *val;
}
#else
Int32 Sync::Interlocked::Increment(Int32 *val)
{
	return ++*val;
}

Int64 Sync::Interlocked::Increment(Int64 *val)
{
	return ++*val;
}

Int32 Sync::Interlocked::Decrement(Int32 *val)
{
	return --*val;
}

Int64 Sync::Interlocked::Decrement(Int64 *val)
{
	return --*val;
}

Int32 Sync::Interlocked::Add(Int32 *val, Int32 aval)
{
	*val += aval;
	return *val;
}

Int64 Sync::Interlocked::Add(Int64 *val, Int64 aval)
{
	*val += aval;
	return *val;
}

Int32 Sync::Interlocked::Subtract(Int32 *val, Int32 sval)
{
	*val -= sval;
	return *val;
}

Int64 Sync::Interlocked::Subtract(Int64 *val, Int64 sval)
{
	*val -= sval;
	return *val;
}
#endif
