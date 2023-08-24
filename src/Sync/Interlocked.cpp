#include "Stdafx.h"
#include "Sync/Interlocked.h"

Int32 Sync::Interlocked::IncrementI32(NotNullPtr<Int32> val)
{
	return Interlocked_IncrementI32(val.Ptr());
}

Int64 Sync::Interlocked::IncrementI64(NotNullPtr<Int64> val)
{
	return Interlocked_IncrementI64(val.Ptr());
}

UInt32 Sync::Interlocked::IncrementU32(NotNullPtr<UInt32> val)
{
	return Interlocked_IncrementU32(val.Ptr());
}

UInt64 Sync::Interlocked::IncrementU64(NotNullPtr<UInt64> val)
{
	return Interlocked_IncrementU64(val.Ptr());
}

OSInt Sync::Interlocked::IncrementOS(NotNullPtr<OSInt> val)
{
#if _OSINT_SIZE == 64
	return Interlocked_IncrementI64(val.Ptr());
#else
	return Interlocked_IncrementI32(val.Ptr());
#endif
}

UOSInt Sync::Interlocked::IncrementUOS(NotNullPtr<UOSInt> val)
{
#if _OSINT_SIZE == 64
	return Interlocked_IncrementU64(val.Ptr());
#else
	return Interlocked_IncrementU32(val.Ptr());
#endif
}

Int32 Sync::Interlocked::DecrementI32(NotNullPtr<Int32> val)
{
	return Interlocked_DecrementI32(val.Ptr());
}

Int64 Sync::Interlocked::DecrementI64(NotNullPtr<Int64> val)
{
	return Interlocked_DecrementI64(val.Ptr());
}

UInt32 Sync::Interlocked::DecrementU32(NotNullPtr<UInt32> val)
{
	return Interlocked_DecrementU32(val.Ptr());
}

UInt64 Sync::Interlocked::DecrementU64(NotNullPtr<UInt64> val)
{
	return Interlocked_DecrementU64(val.Ptr());
}

OSInt Sync::Interlocked::DecrementOS(NotNullPtr<OSInt> val)
{
#if _OSINT_SIZE == 64
	return Interlocked_DecrementI64(val.Ptr());
#else
	return Interlocked_DecrementI32(val.Ptr());
#endif
}

UOSInt Sync::Interlocked::DecrementUOS(NotNullPtr<UOSInt> val)
{
#if _OSINT_SIZE == 64
	return Interlocked_DecrementU64(val.Ptr());
#else
	return Interlocked_DecrementU32(val.Ptr());
#endif
}

Int32 Sync::Interlocked::AddI32(NotNullPtr<Int32> val, Int32 aval)
{
	return Interlocked_AddI32(val.Ptr(), aval);
}

Int64 Sync::Interlocked::AddI64(NotNullPtr<Int64> val, Int64 aval)
{
	return Interlocked_AddI64(val.Ptr(), aval);
}

UInt32 Sync::Interlocked::AddU32(NotNullPtr<UInt32> val, UInt32 aval)
{
	return Interlocked_AddU32(val.Ptr(), aval);
}

UInt64 Sync::Interlocked::AddU64(NotNullPtr<UInt64> val, UInt64 aval)
{
	return Interlocked_AddU64(val.Ptr(), aval);
}

Int32 Sync::Interlocked::SubtractI32(NotNullPtr<Int32> val, Int32 sval)
{
	return Interlocked_SubtractI32(val.Ptr(), sval);
}

Int64 Sync::Interlocked::SubtractI64(NotNullPtr<Int64> val, Int64 sval)
{
	return Interlocked_SubtractI64(val.Ptr(), sval);
}
