#ifndef _SM_IO_DEVICE_ADAM4050
#define _SM_IO_DEVICE_ADAM4050
#include "IO/AdvantechASCIIChannel.h"

namespace IO
{
	namespace Device
	{
		class ADAM4050 : public IO::AdvantechASCIIChannel
		{
		public:
			Bool GetIOStatus(UInt8 addr, UInt16 *outputs, UInt16 *inputs);
		};
	}
}
#endif
