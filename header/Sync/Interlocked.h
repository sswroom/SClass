#ifndef _SM_SYNC_INTERLOCKED
#define _SM_SYNC_INTERLOCKED

namespace Sync
{
	class Interlocked
	{
	public:
		static Int32 IncrementI32(NN<Int32> val);
		static Int64 IncrementI64(NN<Int64> val);
		static UInt32 IncrementU32(NN<UInt32> val);
		static UInt64 IncrementU64(NN<UInt64> val);
		static IntOS IncrementOS(NN<IntOS> val);
		static UIntOS IncrementUOS(NN<UIntOS> val);
		static Int32 DecrementI32(NN<Int32> val);
		static Int64 DecrementI64(NN<Int64> val);
		static UInt32 DecrementU32(NN<UInt32> val);
		static UInt64 DecrementU64(NN<UInt64> val);
		static IntOS DecrementOS(NN<IntOS> val);
		static UIntOS DecrementUOS(NN<UIntOS> val);
		static Int32 AddI32(NN<Int32> val, Int32 aval);
		static Int64 AddI64(NN<Int64> val, Int64 aval);
		static UInt32 AddU32(NN<UInt32> val, UInt32 aval);
		static UInt64 AddU64(NN<UInt64> val, UInt64 aval);
		static Int32 SubtractI32(NN<Int32> val, Int32 sval);
		static Int64 SubtractI64(NN<Int64> val, Int64 sval);
	};
}
#endif
