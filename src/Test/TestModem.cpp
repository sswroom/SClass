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
	IO::SerialPort::GetAvailablePorts(ports, &portTypes);
	writer->WriteLineC(UTF8STRC("Available Serial Ports:"));
	i = 0;
	j = ports.GetCount();
	while (i < j)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Port "));
		sb.AppendUOSInt(ports.GetItem(i));
		sb.AppendC(UTF8STRC(" - "));
		sb.Append(IO::SerialPort::GetPortTypeName(portTypes.GetItem(i)));
		writer->WriteLineC(sb.ToString(), sb.GetLength());
		i++;
	}
	return ports.GetItem(0);
}

void Test::TestModem::GSMModemTest(IO::Writer *writer, IO::GSMModemController *modem, Bool quick)
{
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;

	if ((sptr = modem->GSMGetManufacturer(sbuff)) != 0)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Manufacturer: "));
		sb.AppendP(sbuff, sptr);
		writer->WriteLineC(sb.ToString(), sb.GetLength());
	}
	else
	{
		writer->WriteLineC(UTF8STRC("Manufacturer: Error in getting the value"));
	}

	if ((sptr = modem->GSMGetModelIdent(sbuff)) != 0)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Model: "));
		sb.AppendP(sbuff, sptr);
		writer->WriteLineC(sb.ToString(), sb.GetLength());
	}
	else
	{
		writer->WriteLineC(UTF8STRC("Model: Error in getting the value"));
	}

	if ((sptr = modem->GSMGetModemVer(sbuff)) != 0)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Modem Ver: "));
		sb.AppendP(sbuff, sptr);
		writer->WriteLineC(sb.ToString(), sb.GetLength());
	}
	else
	{
		writer->WriteLineC(UTF8STRC("Modem Ver: Error in getting the value"));
	}

	if ((sptr = modem->GSMGetIMEI(sbuff)) != 0)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("IMEI: "));
		sb.AppendP(sbuff, sptr);
		writer->WriteLineC(sb.ToString(), sb.GetLength());
	}
	else
	{
		writer->WriteLineC(UTF8STRC("IMEI: Error in getting the value"));
	}

	if ((sptr = modem->GSMGetTECharset(sbuff)) != 0)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("TE Charset: "));
		sb.AppendP(sbuff, sptr);
		writer->WriteLineC(sb.ToString(), sb.GetLength());
	}
	else
	{
		writer->WriteLineC(UTF8STRC("TE Charset: Error in getting the value"));
	}

	if ((sptr = modem->GSMGetIMSI(sbuff)) != 0)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("IMSI: "));
		sb.AppendP(sbuff, sptr);
		writer->WriteLineC(sb.ToString(), sb.GetLength());
	}
	else
	{
		writer->WriteLineC(UTF8STRC("IMSI: Error in getting the value"));
	}

	if ((sptr = modem->GSMGetCurrOperator(sbuff)) != 0)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Operator: "));
		sb.AppendP(sbuff, sptr);
		writer->WriteLineC(sb.ToString(), sb.GetLength());
	}
	else
	{
		writer->WriteLineC(UTF8STRC("Operator: Error in getting the value"));
	}

	if ((sptr = modem->GSMGetCurrPLMN(sbuff)) != 0)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("PLMN: "));
		sb.AppendP(sbuff, sptr);
		writer->WriteLineC(sb.ToString(), sb.GetLength());
	}
	else
	{
		writer->WriteLineC(UTF8STRC("PLMN: Error in getting the value"));
	}

	if (!quick)
	{
		Data::ArrayListNN<IO::GSMModemController::Operator> operList;
		NotNullPtr<IO::GSMModemController::Operator> oper;
		if (modem->GSMGetAllowedOperators(operList))
		{

			writer->WriteLineC(UTF8STRC("Operator List:"));
			i = 0;
			j = operList.GetCount();
			while (i < j)
			{
				oper = operList.GetItemNoCheck(i);
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
				writer->WriteLineC(sb.ToString(), sb.GetLength());
				i++;
			}
			modem->GSMFreeOperators(operList);
		}
		else
		{
			writer->WriteLineC(UTF8STRC("Operator List: Error in getting the value"));
		}
	}

	sb.ClearStr();
	sb.AppendC(UTF8STRC("SIM Status: "));
	sb.Append(IO::GSMModemController::SIMStatusGetName(modem->GSMGetSIMStatus()));
	writer->WriteLineC(sb.ToString(), sb.GetLength());

	IO::GSMModemController::RSSI rssi;
	IO::GSMModemController::BER ber;
	if (modem->GSMGetSignalQuality(rssi, ber))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("RSSI: "));
		sptr = IO::GSMModemController::RSSIGetName(sbuff, rssi);
		sb.AppendP(sbuff, sptr);
		sb.AppendC(UTF8STRC(", BER: "));
		sptr = IO::GSMModemController::BERGetName(sbuff, ber);
		sb.AppendP(sbuff, sptr);
		writer->WriteLineC(sb.ToString(), sb.GetLength());
	}
	else
	{
		writer->WriteLineC(UTF8STRC("RSSI: Error in getting the value"));
	}

	Data::DateTime dt;
	if (modem->GSMGetModemTime(dt))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Clock: "));
		sb.AppendDateTime(dt);
		writer->WriteLineC(sb.ToString(), sb.GetLength());
	}
	else
	{
		writer->WriteLineC(UTF8STRC("Clock: Error in getting the value"));
	}

	if ((sptr = modem->SMSGetSMSC(sbuff)) != 0)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("SMSC: "));
		sb.AppendP(sbuff, sptr);
		writer->WriteLineC(sb.ToString(), sb.GetLength());
	}
	else
	{
		writer->WriteLineC(UTF8STRC("SMSC: Error in getting the value"));
	}

	Data::ArrayListNN<IO::GSMModemController::SMSMessage> smsList;
	NotNullPtr<IO::GSMModemController::SMSMessage> sms;
	if (modem->SMSListMessages(smsList, IO::GSMModemController::SMSS_ALL))
	{

		writer->WriteLineC(UTF8STRC("SMS List:"));
		i = 0;
		j = smsList.GetCount();
		while (i < j)
		{
			sms = smsList.GetItemNoCheck(i);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("-"));
			sb.AppendI32(sms->index);
			sb.AppendC(UTF8STRC(", "));
			sb.AppendHexBuff(sms->pduMessage, sms->pduLeng, 0, Text::LineBreakType::None);
			sb.AppendC(UTF8STRC(", "));
			sb.AppendI32(sms->status);
			writer->WriteLineC(sb.ToString(), sb.GetLength());
			i++;
		}
		modem->SMSFreeMessages(smsList);
	}
	else
	{
		writer->WriteLineC(UTF8STRC("SMS List: Error in getting the value"));
	}
}
