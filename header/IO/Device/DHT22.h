#ifndef _SM_IO_DEVICE_DHT22
#define _SM_IO_DEVICE_DHT22
#include "IO/IOPin.h"

namespace IO
{
	namespace Device
	{
		class DHT22
		{
		private:
			IO::IOPin *pin;
		public:
			DHT22(IO::IOPin *pin);
			~DHT22();

			Bool ReadData(Double *temp, Double *rh);
		};
	}
}
#endif
