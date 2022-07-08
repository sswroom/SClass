#include "Stdafx.h"
#include "IO/Device/TelitLE910.h"

IO::Device::TelitLE910::TelitLE910(IO::ATCommandChannel *channel, Bool needRelease) : IO::GSMModemController(channel, needRelease)
{
}

IO::Device::TelitLE910::~TelitLE910()
{
}

Bool IO::Device::TelitLE910::GPSIsPowerUp(Bool *result)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr = this->SendStringCommand(sbuff, UTF8STRC("AT$GPSP?"), 3000);
	if (sptr && Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("$GPSP: ")))
	{
		*result = (sbuff[7] == '1');
		return true;
	}
	return false;
}

Bool IO::Device::TelitLE910::GPSSetPower(Bool isUp)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr = Text::StrConcatC(sbuff, UTF8STRC("AT$GPSP="));
	sptr = Text::StrUOSInt(sptr, isUp?1:0);
	return this->SendBoolCommandC(sbuff, (UOSInt)(sptr - sbuff));
}

Bool IO::Device::TelitLE910::GPSStartNMEAData(Bool ggaData, Bool gllData, Bool gsaData, Bool gsvData, Bool rmcData, Bool vtgData)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr = Text::StrConcatC(sbuff, UTF8STRC("AT$GPSNMUN=2,"));
	sptr = Text::StrUOSInt(sptr, ggaData?1:0);
	*sptr++ = ',';
	sptr = Text::StrUOSInt(sptr, gllData?1:0);
	*sptr++ = ',';
	sptr = Text::StrUOSInt(sptr, gsaData?1:0);
	*sptr++ = ',';
	sptr = Text::StrUOSInt(sptr, gsvData?1:0);
	*sptr++ = ',';
	sptr = Text::StrUOSInt(sptr, rmcData?1:0);
	*sptr++ = ',';
	sptr = Text::StrUOSInt(sptr, vtgData?1:0);
	return this->SendBoolCommandC(sbuff, (UOSInt)(sptr - sbuff));
}

Bool IO::Device::TelitLE910::GPSEndNMEAData()
{
	return this->SendBoolCommandC(UTF8STRC("AT$GPSNMUN=0"));
}
