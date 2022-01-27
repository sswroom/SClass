#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/SerialPort.h"
#include "Test/TestSIM7000.h"
#include "Text/StringBuilderUTF8.h"

void Test::TestSIM7000::SIM7000Info(IO::Writer *writer, IO::Device::SIM7000 *modem)
{
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Int32 adc;
	if ((sptr = modem->SIMCOMGetICCID(sbuff)) != 0)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("CCID: "));
		sb.AppendP(sbuff, sptr);
		writer->WriteLineC(sb.ToString(), sb.GetLength());
	}
	else
	{
		writer->WriteLineC(UTF8STRC("CCID: Error in getting the value"));
	}

	if (modem->SIMCOMReadADC(&adc))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("ADC: "));
		sb.AppendI32(adc);
		writer->WriteLineC(sb.ToString(), sb.GetLength());
	}
	else
	{
		writer->WriteLineC(UTF8STRC("ADC: Error in getting the value"));
	}

	sb.ClearStr();
	sb.AppendC(UTF8STRC("Flash Device Type:\r\n"));
	if (modem->SIMCOMGetFlashDeviceType(&sb))
	{
		writer->WriteLineC(sb.ToString(), sb.GetLength());
	}
	else
	{
		writer->WriteLineC(UTF8STRC("Flash Device Type: Error in getting the value"));
	}

	sb.ClearStr();
	sb.AppendC(UTF8STRC("Device Product ID:\r\n"));
	if (modem->SIMCOMGetDeviceProductID(&sb))
	{
		writer->WriteLineC(sb.ToString(), sb.GetLength());
	}
	else
	{
		writer->WriteLineC(UTF8STRC("Device Product ID: Error in getting the value"));
	}

	if ((sptr = modem->SIMCOMGetUESysInfo(sbuff)) != 0)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("UE Sys Info: "));
		sb.AppendP(sbuff, sptr);
		writer->WriteLineC(sb.ToString(), sb.GetLength());
	}
	else
	{
		writer->WriteLineC(UTF8STRC("UE Sys Info: Error in getting the value"));
	}

	if ((sptr = modem->SIMCOMGetNetworkAPN(sbuff)) != 0)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Network APN: "));
		sb.AppendP(sbuff, sptr);
		writer->WriteLineC(sb.ToString(), sb.GetLength());
	}
	else
	{
		writer->WriteLineC(UTF8STRC("Network APN: Error in getting the value"));
	}

}
