#ifndef _SM_IO_DEVICE_BME280
#define _SM_IO_DEVICE_BME280
#include "IO/I2C.h"

namespace IO
{
	namespace Device
	{
		class BME280
		{
		private:
			IO::I2CChannel *channel;
			Bool toRelease;
			IO::I2C *i2c;
			Bool valid;
			UInt16 t1;
			Int16 t2;
			Int16 t3;
			UInt16 p1;
			Int16 p2;
			Int16 p3;
			Int16 p4;
			Int16 p5;
			Int16 p6;
			Int16 p7;
			Int16 p8;
			Int16 p9;
			UInt8 h1;
			UInt16 h2;
			UInt8 h3;
			UInt16 h4;
			UInt16 h5;
			UInt8 h6;

			Double CalcTempRAW(Int32 tRAW);
			Double CalcRHRAW(Int32 tRAW, Int32 rhRAW);
			Double CalcPressureRAW(Int32 tRAW, Int32 pressureRAW);
		public:
			BME280(IO::I2CChannel *channel, Bool toRelease);
			~BME280();

			Bool IsError();
			Bool Reset();

			Bool ReadTemperature(Double *temp);
			Bool ReadRH(Double *rh);
			Bool ReadPressure(Double *pressure);
			Bool ReadAll(Double *temp, Double *rh, Double *pressure);

			static IO::I2CChannel *CreateDefChannel(Int32 i2cBusNo);
		};
	}
}
#endif
