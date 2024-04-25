#ifndef _SM_IO_MOTIONDETECTORACCELEROMETER
#define _SM_IO_MOTIONDETECTORACCELEROMETER
#include "IO/SensorAccelerometer.h"

namespace IO
{
	class MotionDetectorAccelerometer
	{
	private:
		NN<IO::SensorAccelerometer> acc;
		Bool toRelease;
		Math::Vector3 currAcc;
		Bool currMoving;
	public:
		MotionDetectorAccelerometer(NN<IO::SensorAccelerometer> acc, Bool toRelease);
		~MotionDetectorAccelerometer();

		Bool UpdateStatus();
		Math::Vector3 GetValues() const;
		Bool IsMovving();
	};
}

#endif
