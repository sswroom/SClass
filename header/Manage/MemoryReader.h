#ifndef _SM_MANAGE_MEMORYREADER
#define _SM_MANAGE_MEMORYREADER

namespace Manage
{
	class MemoryReader
	{
	public:
		virtual ~MemoryReader(){};

		virtual UInt8 ReadMemUInt8(UInt64 addr) = 0;
		virtual UInt16 ReadMemUInt16(UInt64 addr) = 0;
		virtual UInt32 ReadMemUInt32(UInt64 addr) = 0;
		virtual UInt64 ReadMemUInt64(UInt64 addr) = 0;
		virtual UOSInt ReadMemory(UInt64 addr, UnsafeArray<UInt8> buff, UOSInt reqSize) = 0;
	};
}
#endif
