#ifndef _SM_NET_TETRALRRP
#define _SM_NET_TETRALRRP
#include "Data/DateTime.h"
#include "Map/GPSTrack.h"

namespace Net
{
	class TETRALRRP
	{
	public:
		static Bool ParseProtocol(UnsafeArray<UInt8> buff, UIntOS buffSize, NN<Data::DateTime> recvTime, NN<Map::GPSTrack::GPSRecord3> record, OutParam<Int32> requestId, OutParam<Int32> resultCode);
		static Double ReadLat(InOutParam<UnsafeArray<UInt8>> buff);
		static Double ReadLong(InOutParam<UnsafeArray<UInt8>> buff);
		static Int32 ReadSIntVar(InOutParam<UnsafeArray<UInt8>> buff);
		static UInt32 ReadUIntVar(InOutParam<UnsafeArray<UInt8>> buff);
		static Double ReadSFloatVar(InOutParam<UnsafeArray<UInt8>> buff);
		static Double ReadUFloatVar(InOutParam<UnsafeArray<UInt8>> buff);

		static UnsafeArray<UInt8> WriteUIntVar(UnsafeArray<UInt8> buff, UInt32 val);
		static UnsafeArray<UInt8> WriteLat(UnsafeArray<UInt8> buff, Double lat);
		static UnsafeArray<UInt8> WriteLong(UnsafeArray<UInt8> buff, Double lon);
		static UnsafeArray<UInt8> WriteDateTime(UnsafeArray<UInt8> buff, NN<Data::DateTime> dt);

		static UIntOS GenLocReq(UnsafeArray<UInt8> buff);

		static UIntOS BuildPacket(UnsafeArray<UInt8> buff, NN<Map::GPSTrack::GPSRecord3> record, Int32 requestId, Int32 resultCode);
	};
};
#endif
