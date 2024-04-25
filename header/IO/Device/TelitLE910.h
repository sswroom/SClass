#ifndef _SM_IO_DEVICE_TELITLE910
#define _SM_IO_DEVICE_TELITLE910
#include "IO/GSMModemController.h"

namespace IO
{
	namespace Device
	{
		class TelitLE910 : public IO::GSMModemController
		{
		private:
			static Bool __stdcall CheckATCommand(void *userObj, const UTF8Char *cmd, UOSInt cmdLen);
		public:
			TelitLE910(NN<IO::ATCommandChannel> channel, Bool needRelease);
			virtual ~TelitLE910();

			Bool GPSIsPowerUp(OutParam<Bool> result); //AT$GPSP?
			Bool GPSSetPower(Bool isUp); //AT$GPSP=
			Bool GPSStartNMEAData(Bool ggaData, Bool gllData, Bool gsaData, Bool gsvData, Bool rmcData, Bool vtgData); //AT$GPSNMUN=2,?,?,?,?,?,?
			Bool GPSEndNMEAData(); //AT$GPSNMUN=0
		};		
	}
}
#endif
