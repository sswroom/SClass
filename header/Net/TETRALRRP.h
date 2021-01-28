#ifndef _SM_NET_TETRALRRP
#define _SM_NET_TETRALRRP
#include "Data/DateTime.h"
#include "Map/GPSTrack.h"

namespace Net
{
	class TETRALRRP
	{
	public:
		static Bool ParseProtocol(UInt8 *buff, OSInt buffSize, Data::DateTime *recvTime, Map::GPSTrack::GPSRecord *record, Int32 *requestId, Int32 *resultCode);
		static Double ReadLat(UInt8 **buff);
		static Double ReadLong(UInt8 **buff);
		static Int32 ReadSIntVar(UInt8 **buff);
		static UInt32 ReadUIntVar(UInt8 **buff);
		static Double ReadSFloatVar(UInt8 **buff);
		static Double ReadUFloatVar(UInt8 **buff);

		static UInt8 *WriteUIntVar(UInt8 *buff, UInt32 val);
		static UInt8 *WriteLat(UInt8 *buff, Double lat);
		static UInt8 *WriteLong(UInt8 *buff, Double lon);
		static UInt8 *WriteDateTime(UInt8 *buff, Data::DateTime *dt);

		static OSInt GenLocReq(UInt8 *buff);

		static OSInt BuildPacket(UInt8 *buff, Map::GPSTrack::GPSRecord *record, Int32 requestId, Int32 resultCode);
	};
};
#endif
