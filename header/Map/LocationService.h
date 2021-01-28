#ifndef _SM_MAP_LOCATIONSERVICE
#define _SM_MAP_LOCATIONSERVICE

namespace Map
{
	class LocationService
	{
	private:
		Bool gpsEnable;
		Bool wifiEnable;
		const WChar *gpsStreamName;

	public:
		LocationService();
		virtual ~LocationService();
		
		void SetGPS(Bool enable, const WChar *streamName);
		void SetWifi(Bool enable);
	};
};
#endif
