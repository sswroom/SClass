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
#endif

