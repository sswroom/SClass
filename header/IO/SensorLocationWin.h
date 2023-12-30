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

		virtual Bool ReadLocation(NotNullPtr<Map::GPSTrack::GPSRecord3> rec);
		virtual Optional<SensorLocation> GetSensorLocation();
		virtual IO::Sensor::SensorType GetSensorType();
	};
}
#endif
