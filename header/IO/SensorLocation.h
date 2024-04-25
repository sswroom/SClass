#ifndef _SM_IO_SENSORLOCATION
#define _SM_IO_SENSORLOCATION
#include "Map/GPSTrack.h"

namespace IO
{
	class SensorLocation
	{
	public:
		virtual ~SensorLocation() {};

		virtual Bool ReadLocation(NN<Map::GPSTrack::GPSRecord3> rec) = 0;
	};
}
#endif
