#ifndef _SM_IO_SENSORLOCATIONWIN
#define _SM_IO_SENSORLOCATIONWIN
#include "IO/SensorWin.h"
#include "IO/SensorLocation.h"

namespace IO
{
	class SensorLocationWin : public SensorWin, public SensorLocation
	{
	public:
		SensorLocationWin(void *sensor);
		virtual ~SensorLocationWin();

		virtual Bool ReadLocation(Map::GPSTrack::GPSRecord2 *rec);
		virtual SensorLocation *GetSensorLocation();
		virtual IO::Sensor::SensorType GetSensorType();
	};
}
#endif
