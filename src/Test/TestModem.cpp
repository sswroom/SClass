#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/SerialPort.h"
#include "Test/TestModem.h"
#include "Text/StringBuilderUTF8.h"

UOSInt Test::TestModem::ListPorts(IO::Writer *writer)
{
	Text::StringBuilderUTF8 sb;
	Data::ArrayList<UOSInt> ports;
	UOSInt i;
	UOSInt j;
	Data::ArrayList<IO::SerialPort::SerialPortType> portTypes;
	IO::SerialPort::GetAvailablePorts(&ports, &portTypes);
	writer->WriteLine((const UTF8Char*)"Available Serial Ports:");
	i = 0;
	j = ports.GetCount();
	while (i < j)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Port "));
		sb.AppendUOSInt(ports.GetItem(i));
		sb.AppendC(UTF8STRC(" - "));
		sb.Append(IO::SerialPort::GetPortTypeName(portTypes.GetItem(i)));
		writer->WriteLine(sb.ToString());
		i++;
	}
	return ports.GetItem(0);
}

void Test::TestModem::GSMModemTest(IO::Writer *writer, IO::GSMModemController *modem, Bool quick)
{
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[256];
	UOSInt i;
	UOSInt j;

	if (modem->GSMGetManufacturer(sbuff))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Manufacturer: "));
		sb.Append(sbuff);
		writer->WriteLine(sb.ToString());
	}
	else
	{
		writer->WriteLine((const UTF8Char*)"Manufacturer: Error in getting the value");
	}

	if (modem->GSMGetModelIdent(sbuff))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Model: "));
		sb.Append(sbuff);
		writer->WriteLine(sb.ToString());
	}
	else
	{
		writer->WriteLine((const UTF8Char*)"Model: Error in getting the value");
	}

	if (modem->GSMGetModemVer(sbuff))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Modem Ver: "));
		sb.Append(sbuff);
		writer->WriteLine(sb.ToString());
	}
	else
	{
		writer->WriteLine((const UTF8Char*)"Modem Ver: Error in getting the value");
	}

	if (modem->GSMGetIMEI(sbuff))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("IMEI: "));
		sb.Append(sbuff);
		writer->WriteLine(sb.ToString());
	}
	else
	{
		writer->WriteLine((const UTF8Char*)"IMEI: Error in getting the value");
	}

	if (modem->GSMGetTECharset(sbuff))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("TE Charset: "));
		sb.Append(sbuff);
		writer->WriteLine(sb.ToString());
	}
	else
	{
		writer->WriteLine((const UTF8Char*)"TE Charset: Error in getting the value");
	}

	if (modem->GSMGetIMSI(sbuff))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("IMSI: "));
		sb.Append(sbuff);
		writer->WriteLine(sb.ToString());
	}
	else
	{
		writer->WriteLine((const UTF8Char*)"IMSI: Error in getting the value");
	}

	if (modem->GSMGetCurrOperator(sbuff))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Operator: "));
		sb.Append(sbuff);
		writer->WriteLine(sb.ToString());
	}
	else
	{
		writer->WriteLine((const UTF8Char*)"Operator: Error in getting the value");
	}

	if (modem->GSMGetCurrPLMN(sbuff))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("PLMN: "));
		sb.Append(sbuff);
		writer->WriteLine(sb.ToString());
	}
	else
	{
		writer->WriteLine((const UTF8Char*)"PLMN: Error in getting the value");
	}

	if (!quick)
	{
		Data::ArrayList<IO::GSMModemController::Operator *> operList;
		IO::GSMModemController::Operator *oper;
		if (modem->GSMGetAllowedOperators(&operList))
		{

			writer->WriteLine((const UTF8Char*)"Operator List:");
			i = 0;
			j = operList.GetCount();
			while (i < j)
			{
				oper = operList.GetItem(i);
				sb.ClearStr();
				sb.AppendC(UTF8STRC("-"));
				sb.Append(oper->shortName);
				sb.AppendC(UTF8STRC(", "));
				sb.Append(oper->longName);
				sb.AppendC(UTF8STRC(", "));
				sb.AppendI32(oper->plmn);
				sb.AppendC(UTF8STRC(", "));
				sb.Append(IO::GSMModemController::OperStatusGetName(oper->status));
				sb.AppendC(UTF8STRC(", "));
				sb.AppendI32(oper->netact);
				writer->WriteLine(sb.ToString());
				i++;
			}
			modem->GSMFreeOperators(&operList);
		}
		else
		{
			writer->WriteLine((const UTF8Char*)"Operator List: Error in getting the value");
		}
	}

	sb.ClearStr();
	sb.AppendC(UTF8STRC("SIM Status: "));
	sb.Append(IO::GSMModemController::SIMStatusGetName(modem->GSMGetSIMStatus()));
	writer->WriteLine(sb.ToString());

	IO::GSMModemController::RSSI rssi;
	IO::GSMModemController::BER ber;
	if (modem->GSMGetSignalQuality(&rssi, &ber))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("RSSI: "));
		IO::GSMModemController::GetRSSIString(sbuff, rssi);
		sb.Append(sbuff);
		sb.AppendC(UTF8STRC(", BER: "));
		IO::GSMModemController::GetBERString(sbuff, ber);
		sb.Append(sbuff);
		writer->WriteLine(sb.ToString());
	}
	else
	{
		writer->WriteLine((const UTF8Char*)"RSSI: Error in getting the value");
	}

	Data::DateTime dt;
	if (modem->GSMGetModemTime(&dt))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Clock: "));
		sb.AppendDate(&dt);
		writer->WriteLine(sb.ToString());
	}
	else
	{
		writer->WriteLine((const UTF8Char*)"Clock: Error in getting the value");
	}

	if (modem->SMSGetSMSC(sbuff))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("SMSC: "));
		sb.Append(sbuff);
		writer->WriteLine(sb.ToString());
	}
	else
	{
		writer->WriteLine((const UTF8Char*)"SMSC: Error in getting the value");
	}

	Data::ArrayList<IO::GSMModemController::SMSMessage *> smsList;
	IO::GSMModemController::SMSMessage *sms;
	if (modem->SMSListMessages(&smsList, IO::GSMModemController::SMSS_ALL))
	{

		writer->WriteLine((const UTF8Char*)"SMS List:");
		i = 0;
		j = smsList.GetCount();
		while (i < j)
		{
			sms = smsList.GetItem(i);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("-"));
			sb.AppendI32(sms->index);
			sb.AppendC(UTF8STRC(", "));
			sb.AppendHexBuff(sms->pduMessage, sms->pduLeng, 0, Text::LineBreakType::None);
			sb.AppendC(UTF8STRC(", "));
			sb.AppendI32(sms->status);
			writer->WriteLine(sb.ToString());
			i++;
		}
		modem->SMSFreeMessages(&smsList);
	}
	else
	{
		writer->WriteLine((const UTF8Char*)"SMS List: Error in getting the value");
	}
}
