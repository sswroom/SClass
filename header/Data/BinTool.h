#ifndef _SM_DATA_BINTOOL
#define _SM_DATA_BINTOOL
namespace Data
{
	class BinTool
	{
	public:
		static Bool Equals(UnsafeArray<const UInt8> buff1, UnsafeArray<const UInt8> buff2, UOSInt len);
	};
}
#endif
