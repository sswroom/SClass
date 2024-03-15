#ifndef _SM_MAP_ILOCATIONSERVICE
#define _SM_MAP_ILOCATIONSERVICE
#include "Data/DataArray.h"
#include "Map/GPSTrack.h"

namespace Map
{
	class ILocationService
	{
	public:
		typedef enum
		{
			ST_NMEA,
			ST_MTK
		} ServiceType;

		enum class SateType
		{
			GPS,
			GLONASS,
			Galileo,
			QZSS,
			BeiDou
		};

		struct SateStatus
		{
			SateType sateType;
			UInt8 prn;
			UInt8 elev;
			UInt16 azimuth;
			Int8 snr;
		};

		typedef void (__stdcall *LocationHandler)(AnyType userObj, NotNullPtr<Map::GPSTrack::GPSRecord3> record, Data::DataArray<SateStatus> sates);

		virtual ~ILocationService() {};

		virtual Bool IsDown() = 0;
		virtual void RegisterLocationHandler(LocationHandler hdlr, AnyType userObj) = 0;
		virtual void UnregisterLocationHandler(LocationHandler hdlr, AnyType userObj) = 0;
		virtual void ErrorRecover() = 0;
		virtual ServiceType GetServiceType() = 0;

		static Text::CStringNN SateTypeGetName(SateType sateType);
	};
}
#endif
