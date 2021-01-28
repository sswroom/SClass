#ifndef _SM_IO_MOTIONDETECTORACCELEROMETER
#define _SM_IO_MOTIONDETECTORACCELEROMETER
#include "IO/SensorAccelerometer.h"

namespace IO
{
	class MotionDetectorAccelerometer
	{
	private:
		IO::SensorAccelerometer *acc;
		Bool toRelease;
		Double currX;
		Double currY;
		Double currZ;
		Bool currMoving;
	public:
		MotionDetectorAccelerometer(IO::SensorAccelerometer *acc, Bool toRelease);
		~MotionDetectorAccelerometer();

		Bool UpdateStatus();
		void GetValues(Double *x, Double *y, Double *z);
		Bool IsMovving();
	};
};

#endif
