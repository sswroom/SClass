#ifndef _SM_SYNC_INTERLOCKED
#define _SM_SYNC_INTERLOCKED
namespace Sync
{
	class Interlocked
	{
	public:
		static Int32 Increment(Int32 *val);
		static Int64 Increment(Int64 *val);
		static Int32 Decrement(Int32 *val);
		static Int64 Decrement(Int64 *val);
		static Int32 Add(Int32 *val, Int32 aval);
		static Int64 Add(Int64 *val, Int64 aval);
		static Int32 Subtract(Int32 *val, Int32 sval);
		static Int64 Subtract(Int64 *val, Int64 sval);
	};
}
#endif

