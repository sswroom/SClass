#include "Stdafx.h"
#include "IO/Device/TelitLE910.h"

IO::Device::TelitLE910::TelitLE910(NN<IO::ATCommandChannel> channel, Bool needRelease) : IO::GSMModemController(channel, needRelease)
{
}

IO::Device::TelitLE910::~TelitLE910()
{
}

Bool IO::Device::TelitLE910::GPSIsPowerUp(OutParam<Bool> result)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	if (this->SendStringCommand(sbuff, UTF8STRC("AT$GPSP?"), 3000).SetTo(sptr) && Text::StrStartsWithC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("$GPSP: ")))
	{
		result.Set(sbuff[7] == '1');
		return true;
	}
	return false;
}

Bool IO::Device::TelitLE910::GPSSetPower(Bool isUp)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr = Text::StrConcatC(sbuff, UTF8STRC("AT$GPSP="));
	sptr = Text::StrUIntOS(sptr, isUp?1:0);
	return this->SendBoolCommandC(sbuff, (UIntOS)(sptr - sbuff));
}

Bool IO::Device::TelitLE910::GPSStartNMEAData(Bool ggaData, Bool gllData, Bool gsaData, Bool gsvData, Bool rmcData, Bool vtgData)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr = Text::StrConcatC(sbuff, UTF8STRC("AT$GPSNMUN=2,"));
	sptr = Text::StrUIntOS(sptr, ggaData?1:0);
	*sptr++ = ',';
	sptr = Text::StrUIntOS(sptr, gllData?1:0);
	*sptr++ = ',';
	sptr = Text::StrUIntOS(sptr, gsaData?1:0);
	*sptr++ = ',';
	sptr = Text::StrUIntOS(sptr, gsvData?1:0);
	*sptr++ = ',';
	sptr = Text::StrUIntOS(sptr, rmcData?1:0);
	*sptr++ = ',';
	sptr = Text::StrUIntOS(sptr, vtgData?1:0);
	return this->SendBoolCommandC(sbuff, (UIntOS)(sptr - sbuff));
}

Bool IO::Device::TelitLE910::GPSEndNMEAData()
{
	return this->SendBoolCommandC(UTF8STRC("AT$GPSNMUN=0"));
}
