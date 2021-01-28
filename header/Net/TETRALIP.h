#ifndef _SM_NET_TETRALIP
#define _SM_NET_TETRALIP
#include "Data/DateTime.h"
#include "Map/GPSTrack.h"

namespace Net
{
	class TETRALIP
	{
	public:
		static Bool ParseProtocol(UInt8 *buff, OSInt buffSize, Data::DateTime *recvTime, Map::GPSTrack::GPSRecord *record, Int32 *reason);
		static OSInt GenLocReq(UInt8 *buff);
	};
};
#endif
