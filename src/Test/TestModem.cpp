#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/SerialPort.h"
#include "Test/TestModem.h"
#include "Text/StringBuilderUTF8.h"

UOSInt Test::TestModem::ListPorts(NN<IO::Writer> writer)
{
	Text::StringBuilderUTF8 sb;
	Data::ArrayList<UOSInt> ports;
	UOSInt i;
	UOSInt j;
	Data::ArrayList<IO::SerialPort::SerialPortType> portTypes;
	IO::SerialPort::GetAvailablePorts(ports, &portTypes);
	writer->WriteLine(CSTR("Available Serial Ports:"));
	i = 0;
	j = ports.GetCount();
	while (i < j)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Port "));
		sb.AppendUOSInt(ports.GetItem(i));
		sb.AppendC(UTF8STRC(" - "));
		sb.Append(IO::SerialPort::GetPortTypeName(portTypes.GetItem(i)));
		writer->WriteLine(sb.ToCString());
		i++;
	}
	return ports.GetItem(0);
}

void Test::TestModem::GSMModemTest(NN<IO::Writer> writer, NN<IO::GSMModemController> modem, Bool quick)
{
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;

	if (modem->GSMGetManufacturer(sbuff).SetTo(sptr))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Manufacturer: "));
		sb.AppendP(sbuff, sptr);
		writer->WriteLine(sb.ToCString());
	}
	else
	{
		writer->WriteLine(CSTR("Manufacturer: Error in getting the value"));
	}

	if (modem->GSMGetModelIdent(sbuff).SetTo(sptr))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Model: "));
		sb.AppendP(sbuff, sptr);
		writer->WriteLine(sb.ToCString());
	}
	else
	{
		writer->WriteLine(CSTR("Model: Error in getting the value"));
	}

	if (modem->GSMGetModemVer(sbuff).SetTo(sptr))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Modem Ver: "));
		sb.AppendP(sbuff, sptr);
		writer->WriteLine(sb.ToCString());
	}
	else
	{
		writer->WriteLine(CSTR("Modem Ver: Error in getting the value"));
	}

	if (modem->GSMGetIMEI(sbuff).SetTo(sptr))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("IMEI: "));
		sb.AppendP(sbuff, sptr);
		writer->WriteLine(sb.ToCString());
	}
	else
	{
		writer->WriteLine(CSTR("IMEI: Error in getting the value"));
	}

	if (modem->GSMGetTECharset(sbuff).SetTo(sptr))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("TE Charset: "));
		sb.AppendP(sbuff, sptr);
		writer->WriteLine(sb.ToCString());
	}
	else
	{
		writer->WriteLine(CSTR("TE Charset: Error in getting the value"));
	}

	if (modem->GSMGetIMSI(sbuff).SetTo(sptr))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("IMSI: "));
		sb.AppendP(sbuff, sptr);
		writer->WriteLine(sb.ToCString());
	}
	else
	{
		writer->WriteLine(CSTR("IMSI: Error in getting the value"));
	}

	if (modem->GSMGetCurrOperator(sbuff).SetTo(sptr))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Operator: "));
		sb.AppendP(sbuff, sptr);
		writer->WriteLine(sb.ToCString());
	}
	else
	{
		writer->WriteLine(CSTR("Operator: Error in getting the value"));
	}

	if (modem->GSMGetCurrPLMN(sbuff).SetTo(sptr))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("PLMN: "));
		sb.AppendP(sbuff, sptr);
		writer->WriteLine(sb.ToCString());
	}
	else
	{
		writer->WriteLine(CSTR("PLMN: Error in getting the value"));
	}

	if (!quick)
	{
		Data::ArrayListNN<IO::GSMModemController::Operator> operList;
		NN<IO::GSMModemController::Operator> oper;
		if (modem->GSMGetAllowedOperators(operList))
		{

			writer->WriteLine(CSTR("Operator List:"));
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
				writer->WriteLine(sb.ToCString());
				i++;
			}
			modem->GSMFreeOperators(operList);
		}
		else
		{
			writer->WriteLine(CSTR("Operator List: Error in getting the value"));
		}
	}

	sb.ClearStr();
	sb.AppendC(UTF8STRC("SIM Status: "));
	sb.Append(IO::GSMModemController::SIMStatusGetName(modem->GSMGetSIMStatus()));
	writer->WriteLine(sb.ToCString());

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
		writer->WriteLine(sb.ToCString());
	}
	else
	{
		writer->WriteLine(CSTR("RSSI: Error in getting the value"));
	}

	Data::DateTime dt;
	if (modem->GSMGetModemTime(dt))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Clock: "));
		sb.AppendDateTime(dt);
		writer->WriteLine(sb.ToCString());
	}
	else
	{
		writer->WriteLine(CSTR("Clock: Error in getting the value"));
	}

	if (modem->SMSGetSMSC(sbuff).SetTo(sptr))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("SMSC: "));
		sb.AppendP(sbuff, sptr);
		writer->WriteLine(sb.ToCString());
	}
	else
	{
		writer->WriteLine(CSTR("SMSC: Error in getting the value"));
	}

	Data::ArrayListNN<IO::GSMModemController::SMSMessage> smsList;
	NN<IO::GSMModemController::SMSMessage> sms;
	if (modem->SMSListMessages(smsList, IO::GSMModemController::SMSS_ALL))
	{

		writer->WriteLine(CSTR("SMS List:"));
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
			writer->WriteLine(sb.ToCString());
			i++;
		}
		modem->SMSFreeMessages(smsList);
	}
	else
	{
		writer->WriteLine(CSTR("SMS List: Error in getting the value"));
	}
}
