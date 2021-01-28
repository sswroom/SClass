#ifndef _SM_MAP_ILOCATIONSERVICE
#define _SM_MAP_ILOCATIONSERVICE
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

		typedef void (__stdcall *LocationHandler)(void *userObj, Map::GPSTrack::GPSRecord *record);

		virtual ~ILocationService() {};

		virtual void RegisterLocationHandler(LocationHandler hdlr, void *userObj) = 0;
		virtual void UnregisterLocationHandler(LocationHandler hdlr, void *userObj) = 0;
		virtual void ErrorRecover() = 0;
		virtual ServiceType GetServiceType() = 0;
	};
};
#endif
