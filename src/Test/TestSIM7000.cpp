#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/SerialPort.h"
#include "Test/TestSIM7000.h"
#include "Text/StringBuilderUTF8.h"

void Test::TestSIM7000::SIM7000Info(IO::Writer *writer, IO::Device::SIM7000 *modem)
{
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[256];
	Int32 adc;
	if (modem->SIMCOMGetICCID(sbuff))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("CCID: "));
		sb.Append(sbuff);
		writer->WriteLine(sb.ToString());
	}
	else
	{
		writer->WriteLine((const UTF8Char*)"CCID: Error in getting the value");
	}

	if (modem->SIMCOMReadADC(&adc))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("ADC: "));
		sb.AppendI32(adc);
		writer->WriteLine(sb.ToString());
	}
	else
	{
		writer->WriteLine((const UTF8Char*)"ADC: Error in getting the value");
	}

	sb.ClearStr();
	sb.AppendC(UTF8STRC("Flash Device Type:\r\n"));
	if (modem->SIMCOMGetFlashDeviceType(&sb))
	{
		writer->WriteLine(sb.ToString());
	}
	else
	{
		writer->WriteLine((const UTF8Char*)"Flash Device Type: Error in getting the value");
	}

	sb.ClearStr();
	sb.AppendC(UTF8STRC("Device Product ID:\r\n"));
	if (modem->SIMCOMGetDeviceProductID(&sb))
	{
		writer->WriteLine(sb.ToString());
	}
	else
	{
		writer->WriteLine((const UTF8Char*)"Device Product ID: Error in getting the value");
	}

	if (modem->SIMCOMGetUESysInfo(sbuff))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("UE Sys Info: "));
		sb.Append(sbuff);
		writer->WriteLine(sb.ToString());
	}
	else
	{
		writer->WriteLine((const UTF8Char*)"UE Sys Info: Error in getting the value");
	}

	if (modem->SIMCOMGetNetworkAPN(sbuff))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Network APN: "));
		sb.Append(sbuff);
		writer->WriteLine(sb.ToString());
	}
	else
	{
		writer->WriteLine((const UTF8Char*)"Network APN: Error in getting the value");
	}

}
