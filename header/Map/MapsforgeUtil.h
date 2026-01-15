#ifndef _SM_MAP_MAPSFORGEUTIL
#define _SM_MAP_MAPSFORGEUTIL
namespace Map
{
	class MapsforgeUtil
	{
	public:
		static UIntOS ReadVBEU(UnsafeArray<UInt8> buff, UIntOS ofst, OutParam<UInt64> v);
		static UIntOS ReadVBES(UnsafeArray<UInt8> buff, UIntOS ofst, OutParam<Int64> v);
	};
}
#endif
