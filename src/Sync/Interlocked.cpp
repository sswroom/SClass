#include "Stdafx.h"
#include "Sync/Interlocked.h"

Int32 Sync::Interlocked::Increment(Int32 *val)
{
	return Interlocked_IncrementI32(val);
}

Int64 Sync::Interlocked::Increment(Int64 *val)
{
	return Interlocked_IncrementI64(val);
}

UInt32 Sync::Interlocked::Increment(UInt32 *val)
{
	return Interlocked_IncrementU32(val);
}

UInt64 Sync::Interlocked::Increment(UInt64 *val)
{
	return Interlocked_DecrementU64(val);
}

Int32 Sync::Interlocked::Decrement(Int32 *val)
{
	return Interlocked_DecrementI32(val);
}

Int64 Sync::Interlocked::Decrement(Int64 *val)
{
	return Interlocked_DecrementI64(val);
}

UInt32 Sync::Interlocked::Decrement(UInt32 *val)
{
	return Interlocked_DecrementU32(val);
}

UInt64 Sync::Interlocked::Decrement(UInt64 *val)
{
	return Interlocked_DecrementU64(val);
}

Int32 Sync::Interlocked::Add(Int32 *val, Int32 aval)
{
	return Interlocked_AddI32(val, aval);
}

Int64 Sync::Interlocked::Add(Int64 *val, Int64 aval)
{
	return Interlocked_AddI64(val, aval);
}

UInt32 Sync::Interlocked::Add(UInt32 *val, UInt32 aval)
{
	return Interlocked_AddU32(val, aval);
}

UInt64 Sync::Interlocked::Add(UInt64 *val, UInt64 aval)
{
	return Interlocked_AddU64(val, aval);
}

Int32 Sync::Interlocked::Subtract(Int32 *val, Int32 sval)
{
	return Interlocked_SubtractI32(val, sval);
}

Int64 Sync::Interlocked::Subtract(Int64 *val, Int64 sval)
{
	return Interlocked_SubtractI64(val, sval);
}
