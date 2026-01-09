#ifndef _SM_MAP_MAPSFORGEUTIL
#define _SM_MAP_MAPSFORGEUTIL
namespace Map
{
	class MapsforgeUtil
	{
	public:
		static UOSInt ReadVBEU(UnsafeArray<UInt8> buff, UOSInt ofst, OutParam<UInt64> v);
		static UOSInt ReadVBES(UnsafeArray<UInt8> buff, UOSInt ofst, OutParam<Int64> v);
	};
}
#endif
