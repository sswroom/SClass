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
			NN<IO::OneWireGPIO> oneWire;

			UInt8 CalcCRC(UnsafeArray<const UInt8> buff, IntOS size);
		public:
			DS18B20(NN<IO::OneWireGPIO> oneWire);
			~DS18B20();

			Bool ReadSensorID(UnsafeArray<UInt8> buff); //7 bytes
			Bool ConvTemp();
			Bool ReadTemp(OutParam<Double> temp);
		};
	}
}
#endif
