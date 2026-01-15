#ifndef _SM_IO_DEVICE_AM2315GPIO
#define _SM_IO_DEVICE_AM2315GPIO
#include "Crypto/Hash/CRC16R.h"
#include "IO/IOPin.h"

namespace IO
{
	namespace Device
	{
		class AM2315GPIO
		{
		private:
			NN<IO::IOPin> sdaPin;
			NN<IO::IOPin> sclPin;
			Crypto::Hash::CRC16R crc;
			Bool error;

			Bool I2CStart();
			Bool I2CWriteByte(UInt8 b); //return isLast
			Bool I2CReadByte(UInt8 *b, Bool isLast); //return isLast
			Bool I2CEnd();

			IntOS DirectRead(UInt8 *buff, IntOS readSize);
			IntOS DirectWrite(const UInt8 *buff, IntOS writeSize);
			void Wait();
			Bool ReadWord(UInt8 regAddr, UInt8 *data);
		public:
			AM2315GPIO(NN<IO::IOPin> sdaPin, NN<IO::IOPin> sclPin);
			~AM2315GPIO();

			Bool IsError();
			void Wakeup();

			Bool ReadTemperature(OutParam<Double> temp);
			Bool ReadRH(OutParam<Double> rh);
		};
	}
}
#endif
