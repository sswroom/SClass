#ifndef _SM_MANAGE_IMEMORYREADER
#define _SM_MANAGE_IMEMORYREADER

namespace Manage
{
	class IMemoryReader
	{
	public:
		virtual ~IMemoryReader(){};

		virtual UInt8 ReadMemUInt8(UInt64 addr) = 0;
		virtual UInt16 ReadMemUInt16(UInt64 addr) = 0;
		virtual UInt32 ReadMemUInt32(UInt64 addr) = 0;
		virtual UInt64 ReadMemUInt64(UInt64 addr) = 0;
		virtual OSInt ReadMemory(UInt64 addr, UInt8 *buff, OSInt reqSize) = 0;
	};
};
#endif
