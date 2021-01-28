#ifndef _SM_IO_DEVICE_AM2315
#define _SM_IO_DEVICE_AM2315
#include "IO/I2C.h"

namespace IO
{
	namespace Device
	{
		class AM2315
		{
		private:
			IO::I2CChannel *channel;
			Bool toRelease;
			IO::I2C *i2c;
		public:
			AM2315(IO::I2CChannel *channel, Bool toRelease);
			~AM2315();

			Bool IsError();
			void Wakeup();

			Bool ReadTemperature(Single *temp);
			Bool ReadRH(Single *rh);

			static IO::I2CChannel *CreateDefChannel(Int32 i2CBusNum);
		};
	}
}
#endif
