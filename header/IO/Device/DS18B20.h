#ifndef _SM_IO_DEVICE_DS18B20
#define _SM_IO_DEVICE_DS18B20
#include "IO/OneWireGPIO.h"

namespace IO
{
	namespace Device
	{
		class DS18B20
		{
		private:
			IO::OneWireGPIO *oneWire;

			UInt8 CalcCRC(const UInt8 *buff, OSInt size);
		public:
			DS18B20(IO::OneWireGPIO *oneWire);
			~DS18B20();

			Bool ReadSensorID(UInt8 *buff); //7 bytes
			Bool ConvTemp();
			Bool ReadTemp(Double *temp);
		};
	}
}
#endif
