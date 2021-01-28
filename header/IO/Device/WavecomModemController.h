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
			WavecomModemController(IO::ATCommandChannel *channel, Bool needRelease);
			virtual ~WavecomModemController();

			UTF8Char *GetSIMCardID(UTF8Char *cardID);
			UTF8Char *GetCapabilityList(UTF8Char *capList);
			Bool StopGSMStack();
			Bool StopModule(); //Cannot response to command until hard reset
			Bool WavecomStopGSMStack();
			Bool WavecomReset();
		};
	}
}
#endif
