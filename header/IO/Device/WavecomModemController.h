#ifndef _SM_IO_DEVICE_WAVECOMMODEMCONTROLLER
#define _SM_IO_DEVICE_WAVECOMMODEMCONTROLLER
#include "IO/GSMModemController.h"

namespace IO
{
	namespace Device
	{
		class WavecomModemController : public IO::GSMModemController
		{
		public:
			WavecomModemController(NN<IO::ATCommandChannel> channel, Bool needRelease);
			virtual ~WavecomModemController();

			UnsafeArrayOpt<UTF8Char> GetSIMCardID(UnsafeArray<UTF8Char> cardID);
			UnsafeArrayOpt<UTF8Char> GetCapabilityList(UnsafeArray<UTF8Char> capList);
			Bool StopGSMStack();
			Bool StopModule(); //Cannot response to command until hard reset
			Bool WavecomStopGSMStack();
			Bool WavecomReset();
		};
	}
}
#endif
