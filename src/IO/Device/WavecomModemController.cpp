#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Device/WavecomModemController.h"
#include "Text/MyString.h"

IO::Device::WavecomModemController::WavecomModemController(NN<IO::ATCommandChannel> channel, Bool needRelease) : IO::GSMModemController(channel, needRelease)
{
}

IO::Device::WavecomModemController::~WavecomModemController()
{
}

UnsafeArrayOpt<UTF8Char> IO::Device::WavecomModemController::GetSIMCardID(UnsafeArray<UTF8Char> cardID)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	if (!this->SendStringCommand(sbuff, UTF8STRC("AT+CCID"), 3000).SetTo(sptr))
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

UnsafeArrayOpt<UTF8Char> IO::Device::WavecomModemController::GetCapabilityList(UnsafeArray<UTF8Char> capList)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	if (!this->SendStringCommand(sbuff, UTF8STRC("AT+GCAP"), 3000).SetTo(sptr))
		return 0;
	if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("+GCAP: ")))
	{
		return Text::StrConcatC(capList, &sbuff[7], (UOSInt)(sptr - &sbuff[7]));
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
