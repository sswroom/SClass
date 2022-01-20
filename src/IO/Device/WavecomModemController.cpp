#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Device/WavecomModemController.h"
#include "Text/MyString.h"

IO::Device::WavecomModemController::WavecomModemController(IO::ATCommandChannel *channel, Bool needRelease) : IO::GSMModemController(channel, needRelease)
{
}

IO::Device::WavecomModemController::~WavecomModemController()
{
}

UTF8Char *IO::Device::WavecomModemController::GetSIMCardID(UTF8Char *cardID)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr = this->SendStringCommand(sbuff, UTF8STRC("AT+CCID"), 3000);
	if (sptr == 0)
		return 0;
	if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("+CCID: \"")))
	{
		UOSInt i = Text::StrIndexOfChar(&sbuff[8], '\"');
		if (i != INVALID_INDEX)
		{
			sbuff[i + 8] = 0;
			sptr = &sbuff[i + 8];
		}
		return Text::StrConcatC(cardID, &sbuff[8], (UOSInt)(sptr - &sbuff[8]));
	}
	else
	{
		return 0;
	}
}

UTF8Char *IO::Device::WavecomModemController::GetCapabilityList(UTF8Char *capList)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr = this->SendStringCommand(sbuff, UTF8STRC("AT+GCAP"), 3000);
	if (sptr == 0)
		return 0;
	if (Text::StrStartsWith(sbuff, (const UTF8Char*)"+GCAP: "))
	{
		return Text::StrConcat(capList, &sbuff[7]);
	}
	else
	{
		return 0;
	}
}

Bool IO::Device::WavecomModemController::StopGSMStack()
{
	return this->SendBoolCommandC(UTF8STRC("AT+CPOF"));
}

Bool IO::Device::WavecomModemController::StopModule()
{
	return this->SendBoolCommandC(UTF8STRC("AT+CPOF=1"));
}

Bool IO::Device::WavecomModemController::WavecomStopGSMStack()
{
	return this->SendBoolCommandC(UTF8STRC("AT+CFUN=0"));
}

Bool IO::Device::WavecomModemController::WavecomReset()
{
	return this->SendBoolCommandC(UTF8STRC("AT+CFUN=1"));
}
