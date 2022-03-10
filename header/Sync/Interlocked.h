#ifndef _SM_SYNC_INTERLOCKED
#define _SM_SYNC_INTERLOCKED

namespace Sync
{
	class Interlocked
	{
	public:
		static Int32 Increment(Int32 *val);
		static Int64 Increment(Int64 *val);
		static UInt32 Increment(UInt32 *val);
		static UInt64 Increment(UInt64 *val);
		static Int32 Decrement(Int32 *val);
		static Int64 Decrement(Int64 *val);
		static UInt32 Decrement(UInt32 *val);
		static UInt64 Decrement(UInt64 *val);
		static Int32 Add(Int32 *val, Int32 aval);
		static Int64 Add(Int64 *val, Int64 aval);
		static UInt32 Add(UInt32 *val, UInt32 aval);
		static UInt64 Add(UInt64 *val, UInt64 aval);
		static Int32 Subtract(Int32 *val, Int32 sval);
		static Int64 Subtract(Int64 *val, Int64 sval);
	};
}

#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__)
#include <windows.h>
#undef CreateDirectory
#undef DeleteFile
#undef FindNextFile
#undef LoadImage
#undef RemoveDirectory
#undef PARITY_NONE
#undef PARITY_EVEN
#undef PARITY_ODD

#define Interlocked_IncrementI32(val) InterlockedIncrement((LONG*)val)
#define Interlocked_IncrementU32(val) InterlockedIncrement((ULONG*)val)
#define Interlocked_DecrementI32(val) InterlockedDecrement((LONG*)val)
#define Interlocked_DecrementU32(val) InterlockedDecrement((ULONG*)val)
#define Interlocked_AddI32(val, aval) (InterlockedExchangeAdd((LONG*)val, (LONG)aval) + aval)
#define Interlocked_AddU32(val, aval) (InterlockedExchangeAdd((ULONG*)val, (ULONG)aval) + aval)
#define Interlocked_SubtractI32(val, sval) (InterlockedExchangeAdd((LONG*)val, -sval) - sval)

#if 1 || defined(_WIN64)
#define Interlocked_IncrementI64(val) InterlockedIncrement64(val)
#define Interlocked_IncrementU64(val) ((UInt64)InterlockedIncrement64((Int64*)val))
#define Interlocked_DecrementI64(val) InterlockedDecrement64(val)
#define Interlocked_DecrementU64(val) ((UInt64)InterlockedDecrement64((Int64*)val))
#define Interlocked_AddI64(val, aval) (InterlockedExchangeAdd64(val, aval) + aval)
#define Interlocked_AddU64(val, aval) ((UInt64)Interlocked_AddI64((Int64*)val, (Int64)aval))
#define Interlocked_SubtractI64(val, sval) (InterlockedExchangeAdd64(val, -sval) - sval)
#else
#define Interlocked_IncrementI64(val) (++*(val))
#define Interlocked_IncrementU64(val) (++*(val))
#define Interlocked_DecrementI64(val) (--*(val))
#define Interlocked_DecrementU64(val) (--*(val))
#define Interlocked_AddI64(val, aval) (*val = *val + aval)
#define Interlocked_AddU64(val, aval) (*val = *val + aval)
#define Interlocked_SubtractI64(val, sval) (*val = *val - sval)
#endif

#elif defined(__GNUC__) && !(defined(_MIPSEL) && defined(__ANDROID_API__)) && !defined(_R3000) && !defined(AVR) && !defined(__ARM_ARCH_5T__) && !(defined(__ARM_ARCH_5TEJ__) && (__STDC_VERSION__ == 201112L))

#define Interlocked_IncrementI32(val) __sync_add_and_fetch(val, 1)
#define Interlocked_IncrementI64(val) __sync_add_and_fetch(val, 1)
#define Interlocked_IncrementU32(val) __sync_add_and_fetch(val, 1)
#define Interlocked_IncrementU64(val) __sync_add_and_fetch(val, 1)
#define Interlocked_DecrementI32(val) __sync_sub_and_fetch(val, 1)
#define Interlocked_DecrementI64(val) __sync_sub_and_fetch(val, 1)
#define Interlocked_DecrementU32(val) __sync_sub_and_fetch(val, 1)
#define Interlocked_DecrementU64(val) __sync_sub_and_fetch(val, 1)
#define Interlocked_AddI32(val, aval) __sync_add_and_fetch(val, aval)
#define Interlocked_AddI64(val, aval) __sync_add_and_fetch(val, aval)
#define Interlocked_AddU32(val, aval) __sync_add_and_fetch(val, aval)
#define Interlocked_AddU64(val, aval) __sync_add_and_fetch(val, aval)
#define Interlocked_SubtractI32(val, sval) __sync_sub_and_fetch(val, sval)
#define Interlocked_SubtractI64(val, sval) (*val = *val - sval)

#elif defined(_MIPSEL) && (defined(__ANDROID_API__) || defined(_R3000))

#define Interlocked_IncrementI32(val) __sync_add_and_fetch(val, 1)
#define Interlocked_IncrementI64(val) (++*(val))
#define Interlocked_IncrementU32(val) __sync_add_and_fetch(val, 1)
#define Interlocked_IncrementU64(val) (++*(val))
#define Interlocked_DecrementI32(val) __sync_sub_and_fetch(val, 1)
#define Interlocked_DecrementI64(val) (--*(val))
#define Interlocked_DecrementU32(val) __sync_sub_and_fetch(val, 1)
#define Interlocked_DecrementU64(val) (--*(val))
#define Interlocked_AddI32(val, aval) __sync_add_and_fetch(val, aval)
#define Interlocked_AddI64(val, aval) (*val = *val + aval)
#define Interlocked_AddU32(val, aval) __sync_add_and_fetch(val, aval)
#define Interlocked_AddU64(val, aval) (*val = *val + aval)
#define Interlocked_SubtractI32(val, sval) __sync_sub_and_fetch(val, sval)
#define Interlocked_SubtractI64(val, sval) (*val = *val - sval)

#else

#define Interlocked_IncrementI32(val) (++*(val))
#define Interlocked_IncrementI64(val) (++*(val))
#define Interlocked_IncrementU32(val) (++*(val))
#define Interlocked_IncrementU64(val) (++*(val))
#define Interlocked_DecrementI32(val) (--*(val))
#define Interlocked_DecrementI64(val) (--*(val))
#define Interlocked_DecrementU32(val) (--*(val))
#define Interlocked_DecrementU64(val) (--*(val))
#define Interlocked_AddI32(val, aval) (*val = *val + aval)
#define Interlocked_AddI64(val, aval) (*val = *val + aval)
#define Interlocked_AddU32(val, aval) (*val = *val + aval)
#define Interlocked_AddU64(val, aval) (*val = *val + aval)
#define Interlocked_SubtractI32(val, sval) (*val = *val - sval)
#define Interlocked_SubtractI64(val, sval) (*val = *val - sval)

#endif

#endif
