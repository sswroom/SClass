#ifndef _SM_IO_DEVICE_HCSR04
#define _SM_IO_DEVICE_HCSR04
#include "IO/GPIOControl.h"

namespace IO
{
	namespace Device
	{
		class HCSR04
		{
		private:
			IO::GPIOControl *gpio;
			OSInt trigPin;
			OSInt echoPin;

		public:
			HCSR04(IO::GPIOControl *gpio, OSInt trigPin, OSInt echoPin);
			~HCSR04();

			Bool ReadTime(Int32 *t);
			Int32 ConvDistanceMM_RoomTemp(Int32 t);
		};
	}
}
#endif
