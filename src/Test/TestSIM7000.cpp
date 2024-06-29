#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/SerialPort.h"
#include "Test/TestSIM7000.h"
#include "Text/StringBuilderUTF8.h"

void Test::TestSIM7000::SIM7000Info(NN<IO::Writer> writer, NN<IO::Device::SIM7000> modem)
{
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	Int32 adc;
	if (modem->SIMCOMGetICCID(sbuff).SetTo(sptr))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("CCID: "));
		sb.AppendP(sbuff, sptr);
		writer->WriteLine(sb.ToCString());
	}
	else
	{
		writer->WriteLine(CSTR("CCID: Error in getting the value"));
	}

	if (modem->SIMCOMReadADC(adc))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("ADC: "));
		sb.AppendI32(adc);
		writer->WriteLine(sb.ToCString());
	}
	else
	{
		writer->WriteLine(CSTR("ADC: Error in getting the value"));
	}

	sb.ClearStr();
	sb.AppendC(UTF8STRC("Flash Device Type:\r\n"));
	if (modem->SIMCOMGetFlashDeviceType(sb))
	{
		writer->WriteLine(sb.ToCString());
	}
	else
	{
		writer->WriteLine(CSTR("Flash Device Type: Error in getting the value"));
	}

	sb.ClearStr();
	sb.AppendC(UTF8STRC("Device Product ID:\r\n"));
	if (modem->SIMCOMGetDeviceProductID(sb))
	{
		writer->WriteLine(sb.ToCString());
	}
	else
	{
		writer->WriteLine(CSTR("Device Product ID: Error in getting the value"));
	}

	if (modem->SIMCOMGetUESysInfo(sbuff).SetTo(sptr))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("UE Sys Info: "));
		sb.AppendP(sbuff, sptr);
		writer->WriteLine(sb.ToCString());
	}
	else
	{
		writer->WriteLine(CSTR("UE Sys Info: Error in getting the value"));
	}

	if (modem->SIMCOMGetNetworkAPN(sbuff).SetTo(sptr))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Network APN: "));
		sb.AppendP(sbuff, sptr);
		writer->WriteLine(sb.ToCString());
	}
	else
	{
		writer->WriteLine(CSTR("Network APN: Error in getting the value"));
	}

}
