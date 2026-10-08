#ifndef _SM_NET_TETRALIP
#define _SM_NET_TETRALIP
#include "Data/DateTime.h"
#include "Map/GPSTrack.h"

namespace Net
{
	class TETRALIP
	{
	public:
		static Bool ParseProtocol(UnsafeArray<const UInt8> buff, UIntOS buffSize, NN<Data::DateTime> recvTime, NN<Map::GPSTrack::GPSRecord3> record, OutParam<Int32> reason);
		static UIntOS GenLocReq(UnsafeArray<UInt8> buff);
	};
};
#endif
