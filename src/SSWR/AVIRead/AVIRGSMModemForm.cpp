#include "Stdafx.h"
#include "Data/FastMap.hpp"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/SerialPort.h"
#include "IO/StreamWriter.h"
#include "SSWR/AVIRead/AVIRGSMModemForm.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/UTF8Writer.h"
#include "UI/GUIFileDialog.h"
#include "UI/GUIFolderDialog.h"

UInt32 __stdcall SSWR::AVIRead::AVIRGSMModemForm::ModemThread(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGSMModemForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGSMModemForm>();
	Data::Timestamp currTime;
	Data::Timestamp nextSignalTime;
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	Bool init = false;
	IO::GSMModemController::BER ber;
	NN<IO::ATCommandChannel> channel;
	NN<Text::String> s;
	NN<IO::GSMModemController> modem;
	NN<IO::Device::HuaweiGSMModemController> huawei;

	nextSignalTime = Data::Timestamp::UtcNow();
	me->running = true;

	if (me->modem.SetTo(modem))
	{
		while (!me->toStop)
		{
			if (!init)
			{
				init = true;
				OPTSTR_DEL(me->initModemManu);
				OPTSTR_DEL(me->initModemModel);
				OPTSTR_DEL(me->initModemVer);
				OPTSTR_DEL(me->initIMEI);
				OPTSTR_DEL(me->huaweiICCID);
				if (modem->GSMGetManufacturer(sbuff).SetTo(sptr))
				{
					s = Text::String::NewP(sbuff, sptr);
					me->initModemManu = s;
					if (s->StartsWith(UTF8STRC("Huawei")) && me->channel.SetTo(channel))
					{
						NN<IO::Device::HuaweiGSMModemController> huawei;
						NN<IO::GSMModemController> oldModem;
						NEW_CLASSNN(huawei, IO::Device::HuaweiGSMModemController(channel, false));
						me->huawei = huawei;
						oldModem = modem;
						me->modem = huawei;
						oldModem.Delete();
						modem = huawei;
						huawei->HuaweiGetCardMode(me->huaweiSIMType);
					}
				}
				if (modem->GSMGetModelIdent(sbuff).SetTo(sptr))
					me->initModemModel = Text::String::NewP(sbuff, sptr).Ptr();
				if (modem->GSMGetModemVer(sbuff).SetTo(sptr))
					me->initModemVer = Text::String::NewP(sbuff, sptr).Ptr();
				if (modem->GSMGetIMEI(sbuff).SetTo(sptr))
					me->initIMEI = Text::String::NewP(sbuff, sptr).Ptr();
				if (me->huawei.SetTo(huawei) && huawei->HuaweiGetICCID(sbuff).SetTo(sptr))
					me->huaweiICCID = Text::String::NewP(sbuff, sptr).Ptr();
				me->initStrs = true;

				if (modem->GSMGetTECharset(sbuff).SetTo(sptr))
				{
					OPTSTR_DEL(me->cfgTECharset);
					me->cfgTECharset = Text::String::NewP(sbuff, sptr);
					me->cfgTECharsetUpd = true;
				}
			}
			if (me->simChanged)
			{
				me->simChanged = false;
				if (modem->GSMGetIMSI(sbuff).SetTo(sptr))
				{
					OPTSTR_DEL(me->simIMSI);
					me->simIMSI = Text::String::NewP(sbuff, sptr);
				}
				me->simInfoUpdated = true;
			}

			currTime = Data::Timestamp::UtcNow();
			if (currTime >= me->operNextTime)
			{
				me->operNextTime = me->operNextTime.AddSecond(30);
				if (modem->GSMGetCurrPLMN(sbuff).SetTo(sptr))
				{
					OPTSTR_DEL(me->operName);
					me->operName = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
					me->operUpdated = true;
				}
				if (modem->GSMGetRegisterNetwork(me->regNetN, me->regNetStat, me->regNetLAC, me->regNetCI, me->regNetACT))
				{
					me->regNetUpdated = true;
				}
				if (me->huawei.SetTo(huawei))
				{
					me->huaweiSysInfoUpdated = huawei->HuaweiGetSysInfoEx(me->huaweiSysInfoSrvStatus,
						me->huaweiSysInfoSrvDomain,
						me->huaweiSysInfoRoamStatus,
						me->huaweiSysInfoSIMState,
						me->huaweiSysInfoLockState,
						me->huaweiSysInfoSysMode,
						me->huaweiSysInfoSubMode);
					IO::Device::HuaweiGSMModemController::FreeVersionInfo(me->huaweiVersion);
					me->huaweiVersionUpdated = huawei->HuaweiGetVersion(me->huaweiVersion);
				}
			}
			if (currTime >= nextSignalTime)
			{
				nextSignalTime = nextSignalTime.AddSecond(10);
				modem->GSMGetSignalQuality(me->signalQuality, ber);
				if (me->huawei.SetTo(huawei))
				{
					me->huaweiCSQUpdated = huawei->HuaweiGetSignalStrength(me->huaweiCSQ);
				}
				me->signalUpdated = true;
			}
			me->modemEvt.Wait(1000);
		}
	}
	me->running = false;
	return 0;
}

void __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnTimerTick(AnyType userObj)
{
	Double values[5];
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	NN<Text::String> s;
	NN<SSWR::AVIRead::AVIRGSMModemForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGSMModemForm>();

	if (me->initStrs)
	{
		me->initStrs = false;
		if (me->initModemManu.SetTo(s))
		{
			me->txtModemManu->SetText(s->ToCString());
		}
		if (me->initModemModel.SetTo(s))
		{
			me->txtModemModel->SetText(s->ToCString());
		}
		if (me->initModemVer.SetTo(s))
		{
			me->txtModemVer->SetText(s->ToCString());
		}
		if (me->initIMEI.SetTo(s))
		{
			me->txtModemIMEI->SetText(s->ToCString());
		}
		if (me->huawei.NotNull())
		{
			if (me->huaweiICCID.SetTo(s))
			{
				me->txtHuaweiICCID->SetText(s->ToCString());
			}
			me->txtHuaweiSIMType->SetText(IO::Device::HuaweiGSMModemController::SIMCardTypeGetName(me->huaweiSIMType));
		}
	}
	if (me->simInfoUpdated)
	{
		me->simInfoUpdated = false;
		me->txtIMSI->SetText(Text::String::OrEmpty(me->simIMSI)->ToCString());
	}
	if (me->cfgTECharsetUpd && me->cfgTECharset.SetTo(s))
	{
		me->cfgTECharsetUpd = false;
		me->txtTECharset->SetText(s->ToCString());
	}

	if (me->operUpdated && me->operName.SetTo(s))
	{
		me->operUpdated = false;
		me->txtOperator->SetText(s->ToCString());
	}

	if (me->regNetUpdated)
	{
		me->regNetUpdated = false;
		me->txtRegStatus->SetText(IO::GSMModemController::RegisterStatusGetName(me->regNetStat));
		if (me->regNetN == IO::GSMModemController::NetworkResult::Enable_w_Location)
		{
			sptr = Text::StrUInt16(sbuff, me->regNetLAC);
			me->txtLAC->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrUInt32(sbuff, me->regNetCI);
			me->txtCI->SetText(CSTRP(sbuff, sptr));
			me->txtACT->SetText(IO::GSMModemController::AccessTechGetName(me->regNetACT));
		}
	}
	if (me->huaweiSysInfoUpdated)
	{
		me->huaweiSysInfoUpdated = false;
		me->txtHuaweiSrvStatus->SetText(IO::Device::HuaweiGSMModemController::ServiceStatusGetName(me->huaweiSysInfoSrvStatus));
		me->txtHuaweiSrvDomain->SetText(IO::Device::HuaweiGSMModemController::ServiceDomainGetName(me->huaweiSysInfoSrvDomain));
		me->txtHuaweiRoamStatus->SetText(me->huaweiSysInfoRoamStatus?CSTR("Roaming"):CSTR("Not Roaming"));
		me->txtHuaweiSIMState->SetText(IO::Device::HuaweiGSMModemController::SIMStateGetName(me->huaweiSysInfoSIMState));
		me->txtHuaweiLockState->SetText(me->huaweiSysInfoLockState?CSTR("Locked"):CSTR("Not locked"));
		me->txtHuaweiSysMode->SetText(IO::Device::HuaweiGSMModemController::SysModeGetName(me->huaweiSysInfoSysMode));
		me->txtHuaweiSubMode->SetText(IO::Device::HuaweiGSMModemController::SubModeGetName(me->huaweiSysInfoSubMode));
	}
	if (me->signalUpdated)
	{
		me->signalUpdated = false;
		sptr = IO::GSMModemController::RSSIGetName(sbuff, me->signalQuality);
		me->txtSignalQuality->SetText(CSTRP(sbuff, sptr));
		values[0] = (Double)IO::GSMModemController::RSSIGetdBm(me->signalQuality);
		values[1] = 0;
		values[2] = 0;
		values[3] = 0;
		values[4] = 0;
		if (me->huawei.NotNull())
		{
			if (me->huaweiCSQ.sysmode == IO::Device::HuaweiGSMModemController::SysMode::LTE)
			{
				values[1] = IO::Device::HuaweiGSMModemController::RSSIGetdBm(me->huaweiCSQ.lteRSSI);
				values[2] = IO::Device::HuaweiGSMModemController::RSRPGetdBm(me->huaweiCSQ.lteRSRP);
				values[3] = IO::Device::HuaweiGSMModemController::SINRGetdBm(me->huaweiCSQ.lteSINR);
				values[4] = IO::Device::HuaweiGSMModemController::RSRQGetdBm(me->huaweiCSQ.lteRSRQ);
			}
			else if (me->huaweiCSQ.sysmode == IO::Device::HuaweiGSMModemController::SysMode::TD_SCDMA)
			{
				values[1] = IO::Device::HuaweiGSMModemController::RSSIGetdBm(me->huaweiCSQ.tdscdmaRSSI);
				values[2] = IO::Device::HuaweiGSMModemController::RSCPGetdBm(me->huaweiCSQ.tdscdmaRSCP);
				values[3] = IO::Device::HuaweiGSMModemController::ECIOGetdBm(me->huaweiCSQ.tdscdmaECIO);
			}
			else if (me->huaweiCSQ.sysmode == IO::Device::HuaweiGSMModemController::SysMode::WCDMA)
			{
				values[1] = IO::Device::HuaweiGSMModemController::RSSIGetdBm(me->huaweiCSQ.wcdmaRSSI);
				values[2] = IO::Device::HuaweiGSMModemController::RSCPGetdBm(me->huaweiCSQ.wcdmaRSCP);
				values[3] = IO::Device::HuaweiGSMModemController::ECIOGetdBm(me->huaweiCSQ.wcdmaECIO);
			}
			else if (me->huaweiCSQ.sysmode == IO::Device::HuaweiGSMModemController::SysMode::GSM)
			{
				values[1] = IO::Device::HuaweiGSMModemController::RSSIGetdBm(me->huaweiCSQ.gsmRSSI);
			}
		}
		me->rlcRSSI->AddSample(values);
	}

	if (me->huaweiCSQUpdated)
	{
		me->huaweiCSQUpdated = false;
		if (me->huaweiCSQ.sysmode == IO::Device::HuaweiGSMModemController::SysMode::LTE)
		{
			me->lblHuaweiCSQ1->SetText(CSTR("LTE RSSI"));
			sptr = IO::Device::HuaweiGSMModemController::RSSIGetName(sbuff, me->huaweiCSQ.lteRSSI);
			me->txtHuaweiCSQ1->SetText(CSTRP(sbuff, sptr));
			me->lblHuaweiCSQ2->SetText(CSTR("LTE RSRP"));
			sptr = IO::Device::HuaweiGSMModemController::RSRPGetName(sbuff, me->huaweiCSQ.lteRSRP);
			me->txtHuaweiCSQ2->SetText(CSTRP(sbuff, sptr));
			me->lblHuaweiCSQ3->SetText(CSTR("LTE SINR"));
			sptr = IO::Device::HuaweiGSMModemController::SINRGetName(sbuff, me->huaweiCSQ.lteSINR);
			me->txtHuaweiCSQ3->SetText(CSTRP(sbuff, sptr));
			me->lblHuaweiCSQ4->SetText(CSTR("LTE RSRQ"));
			sptr = IO::Device::HuaweiGSMModemController::RSRQGetName(sbuff, me->huaweiCSQ.lteRSRQ);
			me->txtHuaweiCSQ4->SetText(CSTRP(sbuff, sptr));
		}
		else if (me->huaweiCSQ.sysmode == IO::Device::HuaweiGSMModemController::SysMode::TD_SCDMA)
		{
			me->lblHuaweiCSQ1->SetText(CSTR("TD-SCDMA RSSI"));
			sptr = IO::Device::HuaweiGSMModemController::RSSIGetName(sbuff, me->huaweiCSQ.tdscdmaRSSI);
			me->txtHuaweiCSQ1->SetText(CSTRP(sbuff, sptr));
			me->lblHuaweiCSQ2->SetText(CSTR("TD-SCDMA RSCP"));
			sptr = IO::Device::HuaweiGSMModemController::RSCPGetName(sbuff, me->huaweiCSQ.tdscdmaRSCP);
			me->txtHuaweiCSQ2->SetText(CSTRP(sbuff, sptr));
			me->lblHuaweiCSQ3->SetText(CSTR("TD-SCDMA ECIO"));
			sptr = IO::Device::HuaweiGSMModemController::ECIOGetName(sbuff, me->huaweiCSQ.tdscdmaECIO);
			me->txtHuaweiCSQ3->SetText(CSTRP(sbuff, sptr));
			me->lblHuaweiCSQ4->SetText(CSTR("-"));
			me->txtHuaweiCSQ4->SetText(CSTR(""));
		}
		else if (me->huaweiCSQ.sysmode == IO::Device::HuaweiGSMModemController::SysMode::WCDMA)
		{
			me->lblHuaweiCSQ1->SetText(CSTR("WCDMA RSSI"));
			sptr = IO::Device::HuaweiGSMModemController::RSSIGetName(sbuff, me->huaweiCSQ.wcdmaRSSI);
			me->txtHuaweiCSQ1->SetText(CSTRP(sbuff, sptr));
			me->lblHuaweiCSQ2->SetText(CSTR("WCDMA RSCP"));
			sptr = IO::Device::HuaweiGSMModemController::RSCPGetName(sbuff, me->huaweiCSQ.wcdmaRSCP);
			me->txtHuaweiCSQ2->SetText(CSTRP(sbuff, sptr));
			me->lblHuaweiCSQ3->SetText(CSTR("WCDMA ECIO"));
			sptr = IO::Device::HuaweiGSMModemController::ECIOGetName(sbuff, me->huaweiCSQ.wcdmaECIO);
			me->txtHuaweiCSQ3->SetText(CSTRP(sbuff, sptr));
			me->lblHuaweiCSQ4->SetText(CSTR("-"));
			me->txtHuaweiCSQ4->SetText(CSTR(""));
		}
		else if (me->huaweiCSQ.sysmode == IO::Device::HuaweiGSMModemController::SysMode::GSM)
		{
			me->lblHuaweiCSQ1->SetText(CSTR("GSM RSSI"));
			sptr = IO::Device::HuaweiGSMModemController::RSSIGetName(sbuff, me->huaweiCSQ.gsmRSSI);
			me->txtHuaweiCSQ1->SetText(CSTRP(sbuff, sptr));
			me->lblHuaweiCSQ2->SetText(CSTR("-"));
			me->txtHuaweiCSQ2->SetText(CSTR(""));
			me->lblHuaweiCSQ3->SetText(CSTR("-"));
			me->txtHuaweiCSQ3->SetText(CSTR(""));
			me->lblHuaweiCSQ4->SetText(CSTR("-"));
			me->txtHuaweiCSQ4->SetText(CSTR(""));
		}
		else
		{
			me->lblHuaweiCSQ1->SetText(CSTR("-"));
			me->txtHuaweiCSQ1->SetText(CSTR(""));
			me->lblHuaweiCSQ2->SetText(CSTR("-"));
			me->txtHuaweiCSQ2->SetText(CSTR(""));
			me->lblHuaweiCSQ3->SetText(CSTR("-"));
			me->txtHuaweiCSQ3->SetText(CSTR(""));
			me->lblHuaweiCSQ4->SetText(CSTR("-"));
			me->txtHuaweiCSQ4->SetText(CSTR(""));
		}
	}

	if (me->huaweiVersionUpdated)
	{
		me->huaweiVersionUpdated = false;
		if (me->huaweiVersion.biosDateTime.SetTo(s)) me->txtHuaweiBDT->SetText(s->ToCString());
		if (me->huaweiVersion.exts.SetTo(s)) me->txtHuaweiEXTS->SetText(s->ToCString());
		if (me->huaweiVersion.extd.SetTo(s)) me->txtHuaweiEXTD->SetText(s->ToCString());
		if (me->huaweiVersion.exth.SetTo(s)) me->txtHuaweiEXTH->SetText(s->ToCString());
		if (me->huaweiVersion.extu.SetTo(s)) me->txtHuaweiEXTU->SetText(s->ToCString());
		if (me->huaweiVersion.cfg.SetTo(s)) me->txtHuaweiCFG->SetText(s->ToCString());
		if (me->huaweiVersion.ini.SetTo(s)) me->txtHuaweiINI->SetText(s->ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnPhoneRClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGSMModemForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGSMModemForm>();
	if (me->port.NotNull())
	{
		me->LoadPhoneBook();
	}
}

void __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnSMSRClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGSMModemForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGSMModemForm>();
	if (me->port.NotNull())
	{
		me->LoadSMS();
	}
}

void __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnSMSSaveClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGSMModemForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGSMModemForm>();
	NN<IO::GSMModemController::SMSMessage> sms;
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	NN<Text::SMSMessage> smsMsg;
	if (me->lvSMS->GetSelectedItem().GetOpt<IO::GSMModemController::SMSMessage>().SetTo(sms) && Text::SMSMessage::CreateFromPDU(sms->pduMessage).SetTo(smsMsg))
	{
		Data::DateTime dt;
		smsMsg->GetMessageTime(dt);

		NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"SMSSave", true);
		dlg->AddFilter(CSTR("*.sms"), CSTR("SMS File"));
		
		sptr = Text::StrConcatC(sbuff, UTF8STRC("SMS"));
		sptr = Text::StrInt64(sptr, dt.ToDotNetTicks());
		sptr = Text::StrConcatC(sptr, UTF8STRC("_"));
		sptr = Text::StrInt32(sptr, sms->index);
		sptr = Text::StrConcatC(sptr, UTF8STRC("_"));
		sptr = Text::StrUTF16_UTF8(sptr, smsMsg->GetAddress().Or(U16STR("")));
		sptr = Text::StrConcatC(sptr, UTF8STRC(".sms"));
		dlg->SetFileName(CSTRP(sbuff, sptr));

		if (dlg->ShowDialog(me->GetHandle()))
		{
			IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			Text::UTF8Writer writer(fs);
			writer.WriteSignature();
			writer.Write(CSTR("From: "));
			writer.WriteLineW(smsMsg->GetAddress().Or(U16STR("")));
			if (smsMsg->GetSMSC().NotNull())
			{
				writer.Write(CSTR("SMSC: "));
				writer.WriteLineW(smsMsg->GetSMSC().Or(U16STR("")));
			}
			writer.Write(CSTR("Date: "));
			sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss zzzz");
			writer.WriteLine(CSTRP(sbuff, sptr));
			writer.WriteLine(CSTR("Content: "));
			writer.WriteLineW(smsMsg->GetContent().Or(U16STR("")));
		}
		dlg.Delete();
		smsMsg.Delete();
	}
}

void __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnSMSDeleteClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGSMModemForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGSMModemForm>();
	NN<IO::GSMModemController> modem;
	if (me->port.IsNull() || !me->modem.SetTo(modem))
	{
		return;
	}
	NN<IO::GSMModemController::SMSMessage> sms;
	UOSInt index = me->lvSMS->GetSelectedIndex();
	if (index != INVALID_INDEX && me->msgList.GetItem(index).SetTo(sms))
	{
		if (modem->SMSDeleteMessage(sms->index))
		{
			modem->SMSFreeMessage(sms);
			me->lvSMS->RemoveItem(index);
			me->msgList.RemoveAt(index);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnSMSSaveAllClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGSMModemForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGSMModemForm>();
	NN<IO::GSMModemController::SMSMessage> sms;
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	NN<Text::SMSMessage> smsMsg;
	if (me->lvSMS->GetCount() > 0)
	{
		NN<UI::GUIFolderDialog> dlg = me->ui->NewFolderDialog();
		if (dlg->ShowDialog(me->GetHandle()))
		{
			UOSInt i = 0;
			UOSInt j = me->lvSMS->GetCount();
			Text::StringBuilderUTF8 sb;
			Data::DateTime dt;
			while (i < j)
			{
				sms = me->lvSMS->GetItem(i).GetNN<IO::GSMModemController::SMSMessage>();
				if (Text::SMSMessage::CreateFromPDU(sms->pduMessage).SetTo(smsMsg))
				{
					smsMsg->GetMessageTime(dt);
					sb.ClearStr();
					sb.Append(dlg->GetFolder());
					sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
					sb.AppendC(UTF8STRC("SMS"));
					sb.AppendI64(dt.ToDotNetTicks());
					sb.AppendC(UTF8STRC("_"));
					sb.AppendI32(sms->index);
					sb.AppendC(UTF8STRC("_"));
					NN<Text::String> s = Text::String::NewNotNull(smsMsg->GetAddress().Or(U16STR("")));
					sb.Append(s);
					s->Release();
					sb.AppendC(UTF8STRC(".sms"));

					IO::FileStream fs(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
					Text::UTF8Writer writer(fs);
					writer.WriteSignature();
					writer.Write(CSTR("From: "));
					writer.WriteLineW(smsMsg->GetAddress().Or(U16STR("")));
					if (smsMsg->GetSMSC().NotNull())
					{
						writer.Write(CSTR("SMSC: "));
						writer.WriteLineW(smsMsg->GetSMSC().Or(U16STR("")));
					}
					writer.Write(CSTR("Date: "));
					sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss zzzz");
					writer.WriteLine(CSTRP(sbuff, sptr));
					writer.WriteLine(CSTR("Content: "));
					writer.WriteLineW(smsMsg->GetContent().Or(U16STR("")));
				}
				else
				{
					me->ui->ShowMsgOK(CSTR("Error in parsing SMS PDU"), CSTR("GSM Modem"), me);
				}
				i++;
			}
		}
		dlg.Delete();
	}
}

void __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnDeviceSerialClk(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGSMModemForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGSMModemForm>();
	NN<IO::SerialPort> port;
	NEW_CLASSNN(port, IO::SerialPort(me->cboDeviceSerial->GetSelectedItem().GetUOSInt(), 115200, IO::SerialPort::PARITY_NONE, false));
	if (port->IsError())
	{
		port.Delete();
		me->txtDeviceStatus->SetText(CSTR("Error in opening port"));
	}
	else
	{
		me->InitStream(port, false);
	}
}

void __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnDeviceOtherClk(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGSMModemForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGSMModemForm>();
	if (me->port.NotNull())
	{
		me->CloseStream(true);
	}
	else
	{
		IO::StreamType st;
		NN<IO::Stream> port;
		if (me->core->OpenStream(st, me, 115200, false).SetTo(port))
		{
			me->InitStream(port, true);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnLogSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGSMModemForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGSMModemForm>();
	NN<Text::String> s;
	if (me->lbLog->GetSelectedItemTextNew().SetTo(s))
	{
		me->txtLog->SetText(s->ToCString());
		s->Release();
	}
	else
	{
		me->txtLog->SetText(CSTR(""));
	}
}

void __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnATCommandClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGSMModemForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGSMModemForm>();
	NN<IO::ATCommandChannel> channel;
	if (me->port.IsNull() || !me->channel.SetTo(channel))
	{
		return;
	}
	Text::StringBuilderUTF8 sb;
	me->txtATCommand->GetText(sb);
	if (sb.StartsWith(UTF8STRC("AT")))
	{
		Data::ArrayListStringNN ret;
		if (channel->SendATCommand(ret, sb.ToString(), sb.GetLength(), 3000))
		{
			sb.Append(CSTR("\r\n"));
			sb.AppendJoin(ret.Iterator(), CSTR("\r\n"));
			ret.FreeAll();
			me->txtATCommand->SetText(CSTR(""));
			me->txtATCommandRep->SetText(sb.ToCString());
		}
	}
}

UI::EventState __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnATCommandKeyDown(AnyType userObj, UInt32 osKey)
{
	NN<SSWR::AVIRead::AVIRGSMModemForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGSMModemForm>();
	UI::GUIControl::GUIKey key = UI::GUIControl::OSKey2GUIKey(osKey);
	if (key == UI::GUIControl::GK_ENTER)
	{
		OnATCommandClicked(me);
		return UI::EventState::StopEvent;
	}
	return UI::EventState::ContinueEvent;
}

void __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnPDPContextLoadClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGSMModemForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGSMModemForm>();
	if (me->port.IsNull())
	{
		return;
	}
	me->LoadPDPContext();
}

void __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnPDPAttachClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGSMModemForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGSMModemForm>();
	NN<IO::GSMModemController> modem;
	if (me->port.IsNull() || !me->modem.SetTo(modem))
	{
		return;
	}
	if (modem->GPRSServiceSetAttached(true))
	{
		me->txtPDPContextStatus->SetText(CSTR("Attached"));
	}
	else
	{
		me->txtPDPContextStatus->SetText(CSTR("Error in attaching"));
	}
}

void __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnPDPDetachClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGSMModemForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGSMModemForm>();
	NN<IO::GSMModemController> modem;
	if (me->port.IsNull() || !me->modem.SetTo(modem))
	{
		return;
	}
	if (modem->GPRSServiceSetAttached(false))
	{
		me->txtPDPContextStatus->SetText(CSTR("Detacted"));
	}
	else
	{
		me->txtPDPContextStatus->SetText(CSTR("Error in detaching"));
	}
}

void __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnPDPContextSetClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGSMModemForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGSMModemForm>();
	NN<IO::GSMModemController> modem;
	if (me->port.IsNull() || !me->modem.SetTo(modem))
	{
		return;
	}
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	me->txtPDPCID->GetText(sb);
	UInt32 cid;
	if (!sb.ToUInt32(cid))
	{
		me->txtPDPContextStatus->SetText(CSTR("CID invalid"));
		return;
	}
	sb.ClearStr();
	me->txtPDPType->GetText(sb);
	if (!sb.Equals(UTF8STRC("IP")) && !sb.Equals(UTF8STRC("PPP")) && !sb.Equals(UTF8STRC("IPV6")) && !sb.Equals(UTF8STRC("IPV4V6")))
	{
		me->txtPDPContextStatus->SetText(CSTR("Type invalid"));
		return;
	}
	me->txtPDPAPN->GetText(sb2);
	if (sb2.GetLength() > 99)
	{
		me->txtPDPContextStatus->SetText(CSTR("APN too long"));
		return;
	}
	if (modem->GPRSSetPDPContext(cid, sb.ToCString(), sb2.ToCString()))
	{
		me->txtPDPContextStatus->SetText(CSTR("PDP Context set"));
	}
	else
	{
		me->txtPDPContextStatus->SetText(CSTR("Error in setting PDP Context"));
	}
}

void __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnPDPContextActiveAllClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGSMModemForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGSMModemForm>();
	NN<IO::GSMModemController> modem;
	if (me->modem.SetTo(modem))
	{
		if (modem->GPRSSetPDPActive(true))
		{
			me->txtPDPContextStatus->SetText(CSTR("All Context activated"));
		}
		else
		{
			me->txtPDPContextStatus->SetText(CSTR("Error in activating context"));
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnPDPContextDeactiveAllClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGSMModemForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGSMModemForm>();
	NN<IO::GSMModemController> modem;
	if (me->modem.SetTo(modem))
	{
		if (modem->GPRSSetPDPActive(false))
		{
			me->txtPDPContextStatus->SetText(CSTR("All Context deactivated"));
		}
		else
		{
			me->txtPDPContextStatus->SetText(CSTR("Error in deactivating context"));
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnPDPContextActiveSelectedClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGSMModemForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGSMModemForm>();
	NN<IO::GSMModemController> modem;
	if (me->modem.SetTo(modem))
	{
		UOSInt i = me->lvPDPContext->GetSelectedIndex();
		if (i != INVALID_INDEX)
		{
			if (modem->GPRSSetPDPActive(true, (UInt32)me->lvPDPContext->GetItem(i).GetUOSInt()))
			{
				me->txtPDPContextStatus->SetText(CSTR("Context activated"));
			}
			else
			{
				me->txtPDPContextStatus->SetText(CSTR("Error in activating context"));
			}
		}
		else
		{
			me->txtPDPContextStatus->SetText(CSTR("No context selected"));
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnPDPContextDeactiveSelectedClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGSMModemForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGSMModemForm>();
	NN<IO::GSMModemController> modem;
	if (me->modem.SetTo(modem))
	{
		UOSInt i = me->lvPDPContext->GetSelectedIndex();
		if (i != INVALID_INDEX)
		{
			if (modem->GPRSSetPDPActive(false, (UInt32)me->lvPDPContext->GetItem(i).GetUOSInt()))
			{
				me->txtPDPContextStatus->SetText(CSTR("Context deactivated"));
			}
			else
			{
				me->txtPDPContextStatus->SetText(CSTR("Error in deactivating context"));
			}
		}
		else
		{
			me->txtPDPContextStatus->SetText(CSTR("No context selected"));
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnHuaweiDHCPClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGSMModemForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGSMModemForm>();
	NN<IO::Device::HuaweiGSMModemController> huawei;
	if (me->huawei.SetTo(huawei))
	{
		UTF8Char sbuff[256];
		UnsafeArray<UTF8Char> sptr;
		UInt32 clientIP;
		UInt32 netmask;
		UInt32 gateway;
		UInt32 dhcp;
		UInt32 priDNS;
		UInt32 secDNS;
		UInt64 maxRXbps;
		UInt64 maxTXbps;
		if (huawei->HuaweiGetDHCP(clientIP, netmask, gateway, dhcp, priDNS, secDNS, maxRXbps, maxTXbps))
		{
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, clientIP);
			me->txtHuaweiDHCPClientIP->SetText(CSTRP(sbuff, sptr));
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, netmask);
			me->txtHuaweiDHCPNetmask->SetText(CSTRP(sbuff, sptr));
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, gateway);
			me->txtHuaweiDHCPGateway->SetText(CSTRP(sbuff, sptr));
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, dhcp);
			me->txtHuaweiDHCPServer->SetText(CSTRP(sbuff, sptr));
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, priDNS);
			me->txtHuaweiDHCPPriDNS->SetText(CSTRP(sbuff, sptr));
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, secDNS);
			me->txtHuaweiDHCPSecDNS->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrUInt64(sbuff, maxRXbps);
			me->txtHuaweiDHCPMaxRXbps->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrUInt64(sbuff, maxTXbps);
			me->txtHuaweiDHCPMaxTXbps->SetText(CSTRP(sbuff, sptr));
		}
		else
		{
			me->txtHuaweiDHCPClientIP->SetText(CSTR("-"));
			me->txtHuaweiDHCPNetmask->SetText(CSTR("-"));
			me->txtHuaweiDHCPGateway->SetText(CSTR("-"));
			me->txtHuaweiDHCPServer->SetText(CSTR("-"));
			me->txtHuaweiDHCPPriDNS->SetText(CSTR("-"));
			me->txtHuaweiDHCPSecDNS->SetText(CSTR("-"));
			me->txtHuaweiDHCPMaxRXbps->SetText(CSTR("-"));
			me->txtHuaweiDHCPMaxTXbps->SetText(CSTR("-"));
		}
	}
}

void SSWR::AVIRead::AVIRGSMModemForm::LoadPhoneBook()
{
	IO::GSMModemController::PBStorage store = (IO::GSMModemController::PBStorage)this->cboPhoneStorage->GetItem((UOSInt)this->cboPhoneStorage->GetSelectedIndex()).GetOSInt();
	NN<IO::GSMModemController> modem;
	if (!this->modem.SetTo(modem))
		return;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	Data::ArrayListNN<IO::GSMModemController::PBEntry> phoneList;
	NN<IO::GSMModemController::PBEntry> ent;
	this->lvPhone->ClearItems();
	if (!modem->PBSetStorage(store))
	{
		this->lblPhoneStatus->SetText(CSTR("Error in setting Phonebook Storage"));
		return;
	}
	if (!modem->PBReadAllEntries(phoneList))
	{
		this->lblPhoneStatus->SetText(CSTR("Error in reading Phonebook entries"));
		return;
	}
	i = 0;
	j = phoneList.GetCount();
	while (i < j)
	{
		ent = phoneList.GetItemNoCheck(i);
		k = this->lvPhone->AddItem(ent->name, 0);
		this->lvPhone->SetSubItem(k, 1, ent->number);
		i++;
	}
	modem->PBFreeEntries(phoneList);
	this->lblPhoneStatus->SetText(CSTR("Success"));
}

void SSWR::AVIRead::AVIRGSMModemForm::LoadSMS()
{
	NN<IO::GSMModemController> modem;
	if (!this->modem.SetTo(modem))
		return;
	NN<IO::GSMModemController::SMSMessage> sms;
	NN<Text::SMSMessage> smsMsg;
	Data::DateTime dt;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
#if _WCHAR_SIZE == 4
	WChar wbuff[256];
#endif
	UOSInt k;
	UOSInt i;
	UOSInt j;
	modem->SMSFreeMessages(this->msgList);
	this->lvSMS->ClearItems();
	this->msgList.Clear();

	IO::GSMModemController::SMSStorage store = (IO::GSMModemController::SMSStorage)this->cboSMSStorage->GetItem((UOSInt)this->cboSMSStorage->GetSelectedIndex()).GetOSInt();

	modem->SMSSetStorage(store, IO::GSMModemController::SMSSTORE_SIM, IO::GSMModemController::SMSSTORE_SIM);
	if (modem->SMSGetSMSC(sbuff).SetTo(sptr))
	{
		this->txtSMSC->SetText(CSTRP(sbuff, sptr));
	}
	else
	{
		this->txtSMSC->SetText(CSTR(""));
	}
	modem->SMSListMessages(this->msgList, IO::GSMModemController::SMSS_ALL);

	i = 0;
	j = this->msgList.GetCount();
	while (i < j)
	{
		sms = this->msgList.GetItemNoCheck(i);
		if (Text::SMSMessage::CreateFromPDU(sms->pduMessage).SetTo(smsMsg))
		{
	#if _WCHAR_SIZE == 2
			k = this->lvSMS->AddItem(smsMsg->GetAddress().Or(U16STR("")), sms);
	#elif _WCHAR_SIZE == 4
			Text::StrUTF16_UTF32(wbuff, smsMsg->GetAddress().Or(U16STR("")));
			k = this->lvSMS->AddItem(wbuff, sms);
	#endif
			smsMsg->GetMessageTime(dt);
			sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss zzzz");
			this->lvSMS->SetSubItem(k, 1, CSTRP(sbuff, sptr));
			sptr = Text::StrInt32(sbuff, sms->index);
			this->lvSMS->SetSubItem(k, 2, CSTRP(sbuff, sptr));
			UnsafeArray<const UTF16Char> cont;
			if (smsMsg->GetContent().SetTo(cont))
			{
	#if _WCHAR_SIZE == 2
				this->lvSMS->SetSubItem(k, 3, cont);
	#elif _WCHAR_SIZE == 4
				Text::StrUTF16_UTF32(wbuff, cont);
				this->lvSMS->SetSubItem(k, 3, wbuff);
	#endif
			}
			smsMsg.Delete();
		}
		else
		{
			k = this->lvSMS->AddItem(CSTR("?"), sms);
			this->lvSMS->SetSubItem(k, 1, CSTR("?"));
			sptr = Text::StrInt32(sbuff, sms->index);
			this->lvSMS->SetSubItem(k, 2, CSTRP(sbuff, sptr));
		}
		i++;
	}
}

void SSWR::AVIRead::AVIRGSMModemForm::LoadPDPContext()
{
	NN<IO::GSMModemController> modem;
	Data::ArrayListNN<IO::GSMModemController::PDPContext> ctxList;
	if (!this->modem.SetTo(modem) || !modem->GPRSGetPDPContext(ctxList))
	{
		this->txtPDPContextStatus->SetText(CSTR("Error in getting PDP Context"));
		return;
	}
	this->lvPDPContext->ClearItems();
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<IO::GSMModemController::PDPContext> ctx;
	Data::UInt32FastMap<UOSInt> dataMap;
	UOSInt i = 0;
	UOSInt j = ctxList.GetCount();
	UOSInt k;
	while (i < j)
	{
		ctx = ctxList.GetItemNoCheck(i);
		sptr = Text::StrUInt32(sbuff, ctx->cid);
		k = this->lvPDPContext->AddItem(CSTRP(sbuff, sptr), (void*)(UOSInt)ctx->cid);
		dataMap.Put(ctx->cid, k);
		this->lvPDPContext->SetSubItem(k, 1, ctx->type);
		this->lvPDPContext->SetSubItem(k, 2, ctx->apn);
		i++;
	}
	modem->GPRSFreePDPContext(ctxList);

	Data::ArrayList<IO::GSMModemController::ActiveState> actList;
	if (!modem->GPRSGetPDPActive(actList))
	{
		this->txtPDPContextStatus->SetText(CSTR("Error in getting PDP Active State"));
		return;
	}
	IO::GSMModemController::ActiveState act;
	i = 0;
	j = actList.GetCount();
	while (i < j)
	{
		act = actList.GetItem(i);
		k = dataMap.Get(act.cid);
		if (k == 0 && !dataMap.ContainsKey(act.cid))
		{
			sptr = Text::StrUInt32(sbuff, act.cid);
			k = this->lvPDPContext->AddItem(CSTRP(sbuff, sptr), (void*)(UOSInt)act.cid);
			dataMap.Put(act.cid, k);
		}
		if (act.active)
		{
			this->lvPDPContext->SetSubItem(k, 3, CSTR("Activated"));
		}
		else
		{
			this->lvPDPContext->SetSubItem(k, 3, CSTR("Deactivated"));
		}
		i++;
	}

	Bool attached;
	if (!modem->GPRSServiceIsAttached(attached))
	{
		this->txtPDPContextStatus->SetText(CSTR("Error in getting Attach status"));	
	}
	else if (attached)
	{
		this->txtPDPContextStatus->SetText(CSTR("Attached"));	
	}
	else
	{
		this->txtPDPContextStatus->SetText(CSTR("Not Attached"));	
	}
}

void SSWR::AVIRead::AVIRGSMModemForm::InitStream(NN<IO::Stream> stm, Bool updateSerial)
{
	if (this->port.IsNull())
	{
		this->port = stm;
		this->txtDeviceStatus->SetText(IO::StreamTypeGetName(stm->GetStreamType()));
		if (updateSerial && stm->GetStreamType() == IO::StreamType::SerialPort)
		{
			NN<IO::SerialPort> port = NN<IO::SerialPort>::ConvertFrom(stm);
			UOSInt portNum = port->GetPortNum();
			UOSInt i = this->cboDeviceSerial->GetCount();
			while (i-- > 0)
			{
				if (this->cboDeviceSerial->GetItem(i).GetUOSInt() == portNum)
				{
					this->cboDeviceSerial->SetSelectedIndex(i);
					break;
				}
			}
		}
		this->cboDeviceSerial->SetEnabled(false);
		this->btnDeviceSerial->SetEnabled(false);
		this->btnDeviceOther->SetText(CSTR("Close"));

		NN<IO::ATCommandChannel> channel;
		NEW_CLASSNN(channel, IO::ATCommandChannel(stm, false));
		channel->SetLogger(&this->log);
		this->channel = channel.Ptr();
		NEW_CLASSOPT(this->modem, IO::GSMModemController(channel, false));

		this->simChanged = true;
		this->toStop = false;
		this->running = false;
		Sync::ThreadUtil::Create(ModemThread, this);
		while (!this->running)
		{
			Sync::SimpleThread::Sleep(10);
		}
	}
}

void SSWR::AVIRead::AVIRGSMModemForm::CloseStream(Bool updateUI)
{
	NN<IO::ATCommandChannel> channel;
	NN<IO::Stream> port;
	NN<IO::GSMModemController> modem;
	if (this->port.SetTo(port) && this->channel.SetTo(channel) && this->modem.SetTo(modem))
	{
		this->toStop = true;
		this->modemEvt.Set();
		port->Close();
		channel->Close();
		while (this->running)
		{
			Sync::SimpleThread::Sleep(10);
		}

		modem->SMSFreeMessages(this->msgList);

		modem.Delete();
		this->channel.Delete();
		port.Delete();
		this->modem = 0;
		this->port = 0;
		this->huawei = 0;

		if (updateUI)
		{
			this->cboDeviceSerial->SetEnabled(true);
			this->btnDeviceSerial->SetEnabled(true);
			this->btnDeviceOther->SetText(CSTR("Open Stream"));
			this->txtDeviceStatus->SetText(CSTR(""));
		}
	}
}

SSWR::AVIRead::AVIRGSMModemForm::AVIRGSMModemForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Optional<IO::Stream> port) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("GSM Modem"));
	this->SetFont(nullptr, 8.25, false);
	this->core = core;
	this->modem = 0;
	this->channel = 0;
	this->port = 0;
	this->huawei = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->signalUpdated = false;
	this->signalQuality = IO::GSMModemController::RSSI_UNKNOWN;
	this->operUpdated = false;
	this->operName = 0;
	this->initStrs = 0;
	this->initModemManu = 0;
	this->initModemModel = 0;
	this->initModemVer = 0;
	this->initIMEI = 0;
	this->huaweiICCID = 0;
	this->huaweiSysInfoUpdated = false;;
	this->huaweiSysInfoSrvStatus = IO::Device::HuaweiGSMModemController::ServiceStatus::NoServices;
	this->huaweiSysInfoSrvDomain = IO::Device::HuaweiGSMModemController::ServiceDomain::NoServices;
	this->huaweiSysInfoRoamStatus = false;
	this->huaweiSysInfoSIMState = IO::Device::HuaweiGSMModemController::SIMState::InvalidSIM;
	this->huaweiSysInfoLockState = false;
	this->huaweiSysInfoSysMode = IO::Device::HuaweiGSMModemController::SysMode::NoService;
	this->huaweiSysInfoSubMode = IO::Device::HuaweiGSMModemController::SubMode::NoService;
	this->huaweiSIMType = IO::Device::HuaweiGSMModemController::SIMCardType::NoCard;
	this->huaweiCSQUpdated = false;
	this->cfgTECharset = 0;
	this->cfgTECharsetUpd = false;
	this->simChanged = false;
	this->simInfoUpdated = false;
	this->simIMSI = 0;
	this->regNetUpdated = false;
	this->regNetN = IO::GSMModemController::NetworkResult::Disable;
	this->regNetCI = 0;
	this->regNetLAC = 0;
	this->regNetStat = IO::GSMModemController::RegisterStatus::NotRegistered;
	this->regNetACT = IO::GSMModemController::AccessTech::GSM;
	this->operNextTime = Data::Timestamp::UtcNow();
	this->huaweiVersion.biosDateTime = 0;
	this->huaweiVersion.exts = 0;
	this->huaweiVersion.extd = 0;
	this->huaweiVersion.exth = 0;
	this->huaweiVersion.exts = 0;
	this->huaweiVersion.cfg = 0;
	this->huaweiVersion.ini = 0;
	this->huaweiVersionUpdated = false;

	this->pnlDevice = ui->NewPanel(*this);
	this->pnlDevice->SetRect(0, 0, 100, 48, false);
	this->pnlDevice->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblDeviceSerial = ui->NewLabel(this->pnlDevice, CSTR("Serial Port"));
	this->lblDeviceSerial->SetRect(0, 0, 100, 23, false);
	this->cboDeviceSerial = ui->NewComboBox(this->pnlDevice, false);
	this->cboDeviceSerial->SetRect(100, 0, 150, 23, false);
	this->btnDeviceSerial = ui->NewButton(this->pnlDevice, CSTR("Open"));
	this->btnDeviceSerial->SetRect(250, 0, 75, 23, false);
	this->btnDeviceSerial->HandleButtonClick(OnDeviceSerialClk, this);
	this->lblDeviceStatus = ui->NewLabel(this->pnlDevice, CSTR("Status"));
	this->lblDeviceStatus->SetRect(0, 24, 100, 23, false);
	this->txtDeviceStatus = ui->NewTextBox(this->pnlDevice, CSTR(""));
	this->txtDeviceStatus->SetRect(100, 24, 150, 23, false);
	this->txtDeviceStatus->SetReadOnly(true);
	this->btnDeviceOther = ui->NewButton(this->pnlDevice, CSTR("Open Stream"));
	this->btnDeviceOther->SetRect(250, 24, 75, 23, false);
	this->btnDeviceOther->HandleButtonClick(OnDeviceOtherClk, this);
	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpInfo = this->tcMain->AddTabPage(CSTR("Info"));
	this->rlcRSSI = ui->NewRealtimeLineChart(this->tpInfo, this->core->GetDrawEngine(), 5, 360, 10000, 0);
	this->rlcRSSI->SetRect(0, 0, 100, 100, false);
	this->rlcRSSI->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->pnlInfo = ui->NewPanel(this->tpInfo);
	this->pnlInfo->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lblModemManu = ui->NewLabel(this->pnlInfo, CSTR("Manufacture"));
	this->lblModemManu->SetRect(8, 8, 100, 23, false);
	this->txtModemManu = ui->NewTextBox(this->pnlInfo, CSTR(""));
	this->txtModemManu->SetRect(108, 8, 200, 23, false);
	this->txtModemManu->SetReadOnly(true);
	this->lblModemModel = ui->NewLabel(this->pnlInfo, CSTR("Model"));
	this->lblModemModel->SetRect(8, 32, 100, 23, false);
	this->txtModemModel = ui->NewTextBox(this->pnlInfo, CSTR(""));
	this->txtModemModel->SetRect(108, 32, 100, 23, false);
	this->txtModemModel->SetReadOnly(true);
	this->lblModemVer = ui->NewLabel(this->pnlInfo, CSTR("Modem Ver"));
	this->lblModemVer->SetRect(8, 56, 100, 23, false);
	this->txtModemVer = ui->NewTextBox(this->pnlInfo, CSTR(""));
	this->txtModemVer->SetRect(108, 56, 200, 23, false);
	this->txtModemVer->SetReadOnly(true);
	this->lblModemIMEI = ui->NewLabel(this->pnlInfo, CSTR("IMEI"));
	this->lblModemIMEI->SetRect(8, 80, 100, 23, false);
	this->txtModemIMEI = ui->NewTextBox(this->pnlInfo, CSTR(""));
	this->txtModemIMEI->SetRect(108, 80, 150, 23, false);
	this->txtModemIMEI->SetReadOnly(true);
	this->lblIMSI = ui->NewLabel(this->pnlInfo, CSTR("IMSI"));
	this->lblIMSI->SetRect(8, 104, 100, 23, false);
	this->txtIMSI = ui->NewTextBox(this->pnlInfo, CSTR(""));
	this->txtIMSI->SetRect(108, 104, 150, 23, false);
	this->txtIMSI->SetReadOnly(true);
	this->lblTECharset = ui->NewLabel(this->pnlInfo, CSTR("TE Charset"));
	this->lblTECharset->SetRect(8, 128, 100, 23, false);
	this->txtTECharset = ui->NewTextBox(this->pnlInfo, CSTR(""));
	this->txtTECharset->SetRect(108, 128, 100, 23, false);
	this->txtTECharset->SetReadOnly(true);
	this->lblOperator = ui->NewLabel(this->pnlInfo, CSTR("Operator"));
	this->lblOperator->SetRect(8, 152, 100, 23, false);
	this->txtOperator = ui->NewTextBox(this->pnlInfo, CSTR(""));
	this->txtOperator->SetRect(108, 152, 100, 23, false);
	this->txtOperator->SetReadOnly(true);
	this->lblRegStatus = ui->NewLabel(this->pnlInfo, CSTR("Register Status"));
	this->lblRegStatus->SetRect(8, 176, 100, 23, false);
	this->txtRegStatus = ui->NewTextBox(this->pnlInfo, CSTR(""));
	this->txtRegStatus->SetRect(108, 176, 200, 23, false);
	this->txtRegStatus->SetReadOnly(true);
	this->lblLAC = ui->NewLabel(this->pnlInfo, CSTR("LAC"));
	this->lblLAC->SetRect(8, 200, 100, 23, false);
	this->txtLAC = ui->NewTextBox(this->pnlInfo, CSTR(""));
	this->txtLAC->SetRect(108, 200, 100, 23, false);
	this->txtLAC->SetReadOnly(true);
	this->lblCI = ui->NewLabel(this->pnlInfo, CSTR("CI"));
	this->lblCI->SetRect(8, 224, 100, 23, false);
	this->txtCI = ui->NewTextBox(this->pnlInfo, CSTR(""));
	this->txtCI->SetRect(108, 224, 100, 23, false);
	this->txtCI->SetReadOnly(true);
	this->lblACT = ui->NewLabel(this->pnlInfo, CSTR("Access Tech"));
	this->lblACT->SetRect(8, 248, 100, 23, false);
	this->txtACT = ui->NewTextBox(this->pnlInfo, CSTR(""));
	this->txtACT->SetRect(108, 248, 200, 23, false);
	this->txtACT->SetReadOnly(true);
	this->lblSignalQuality = ui->NewLabel(this->pnlInfo, CSTR("Signal Quality"));
	this->lblSignalQuality->SetRect(8, 272, 100, 23, false);
	this->txtSignalQuality = ui->NewTextBox(this->pnlInfo, CSTR(""));
	this->txtSignalQuality->SetRect(108, 272, 100, 23, false);
	this->txtSignalQuality->SetReadOnly(true);

	this->tpPhoneBook = this->tcMain->AddTabPage(CSTR("PhoneBook"));
	this->pnlPhone = ui->NewPanel(this->tpPhoneBook);
	this->pnlPhone->SetRect(0, 0, 100, 32, false);
	this->pnlPhone->SetDockType(UI::GUIControl::DOCK_TOP);
	this->cboPhoneStorage = ui->NewComboBox(this->pnlPhone, false);
	this->cboPhoneStorage->SetRect(0, 4, 100, 23, false);
	this->cboPhoneStorage->AddItem(CSTR("SIM"), (void*)(OSInt)IO::GSMModemController::PBSTORE_SIM);
	this->cboPhoneStorage->AddItem(CSTR("Restricted Phones"), (void*)(OSInt)IO::GSMModemController::PBSTORE_SIM_RESTRICTED);
	this->cboPhoneStorage->AddItem(CSTR("Own numbers"), (void*)(OSInt)IO::GSMModemController::PBSTORE_SIM_OWN_NUMBERS);
	this->cboPhoneStorage->AddItem(CSTR("Emergency"), (void*)(OSInt)IO::GSMModemController::PBSTORE_EMERGENCY);
	this->cboPhoneStorage->AddItem(CSTR("Last Dialed"), (void*)(OSInt)IO::GSMModemController::PBSTORE_LASTNUMDIAL);
	this->cboPhoneStorage->AddItem(CSTR("Unanswered"), (void*)(OSInt)IO::GSMModemController::PBSTORE_UNANSWERED);
	this->cboPhoneStorage->AddItem(CSTR("ME"), (void*)(OSInt)IO::GSMModemController::PBSTORE_ME);
	this->cboPhoneStorage->AddItem(CSTR("ME+SIM"), (void*)(OSInt)IO::GSMModemController::PBSTORE_ME_SIM);
	this->cboPhoneStorage->AddItem(CSTR("Received Calls"), (void*)(OSInt)IO::GSMModemController::PBSTORE_RECEIVED_CALL);
	this->cboPhoneStorage->AddItem(CSTR("Service Dial Numbers"), (void*)(OSInt)IO::GSMModemController::PBSTORE_SERVICE_DIALING_NUMBERS);
	this->cboPhoneStorage->SetSelectedIndex(0);
	this->btnPhoneRead = ui->NewButton(this->pnlPhone, CSTR("&Read"));
	this->btnPhoneRead->SetRect(108, 4, 75, 23, false);
	this->btnPhoneRead->HandleButtonClick(OnPhoneRClick, this);
	this->lblPhoneStatus = ui->NewLabel(this->pnlPhone, CSTR(""));
	this->lblPhoneStatus->SetRect(188, 4, 200, 23, false);
	this->lvPhone = ui->NewListView(this->tpPhoneBook, UI::ListViewStyle::Table, 2);
	this->lvPhone->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvPhone->AddColumn(CSTR("Name"), 200);
	this->lvPhone->AddColumn(CSTR("Phone"), 200);
	this->lvPhone->SetFullRowSelect(true);
	this->lvPhone->SetShowGrid(true);

	this->tpSMS = this->tcMain->AddTabPage(CSTR("SMS"));
	this->pnlSMS = ui->NewPanel(this->tpSMS);
	this->pnlSMS->SetRect(0, 0, 100, 32, false);
	this->pnlSMS->SetDockType(UI::GUIControl::DOCK_TOP);
	this->cboSMSStorage = ui->NewComboBox(this->pnlSMS, false);
	this->cboSMSStorage->SetRect(0, 4, 100, 23, false);
	this->cboSMSStorage->AddItem(CSTR("SIM"), (void*)(OSInt)IO::GSMModemController::SMSSTORE_SIM);
	this->cboSMSStorage->AddItem(CSTR("Flash"), (void*)(OSInt)IO::GSMModemController::SMSSTORE_FLASH);
	this->cboSMSStorage->AddItem(CSTR("Status Report"), (void*)(OSInt)IO::GSMModemController::SMSSTORE_STATUSREPORT);
	this->cboSMSStorage->AddItem(CSTR("CBM"), (void*)(OSInt)IO::GSMModemController::SMSSTORE_CBMMESSAGE);
	this->cboSMSStorage->SetSelectedIndex(0);
	this->btnSMSRead = ui->NewButton(this->pnlSMS, CSTR("&Show SMS"));
	this->btnSMSRead->SetRect(108, 4, 75, 23, false);
	this->btnSMSRead->HandleButtonClick(OnSMSRClick, this);
	this->lblSMSC = ui->NewLabel(this->pnlSMS, CSTR("SMSC"));
	this->lblSMSC->SetRect(208, 4, 50, 23, false);
	this->txtSMSC = ui->NewTextBox(this->pnlSMS, CSTR(""));
	this->txtSMSC->SetReadOnly(true);
	this->txtSMSC->SetRect(258, 4, 100, 23, false);
	this->btnSMSSave = ui->NewButton(this->pnlSMS, CSTR("S&ave SMS"));
	this->btnSMSSave->SetRect(368, 4, 75, 23, false);
	this->btnSMSSave->HandleButtonClick(OnSMSSaveClick, this);
	this->btnSMSDelete = ui->NewButton(this->pnlSMS, CSTR("&Delete SMS"));
	this->btnSMSDelete->SetRect(448, 4, 75, 23, false);
	this->btnSMSDelete->HandleButtonClick(OnSMSDeleteClick, this);
	this->btnSMSSaveAll = ui->NewButton(this->pnlSMS, CSTR("Save All"));
	this->btnSMSSaveAll->SetRect(528, 4, 75, 23, false);
	this->btnSMSSaveAll->HandleButtonClick(OnSMSSaveAllClick, this);
	this->lvSMS = ui->NewListView(this->tpSMS, UI::ListViewStyle::Table, 4);
	this->lvSMS->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvSMS->AddColumn(CSTR("From"), 100);
	this->lvSMS->AddColumn(CSTR("Time"), 180);
	this->lvSMS->AddColumn(CSTR("Index"), 50);
	this->lvSMS->AddColumn(CSTR("Content"), 300);
	this->lvSMS->SetFullRowSelect(true);

	this->tpPDPContext = this->tcMain->AddTabPage(CSTR("PDP Context(APN)"));
	this->pnlPDPContext = ui->NewPanel(this->tpPDPContext);
	this->pnlPDPContext->SetRect(0, 0, 100, 72, false);
	this->pnlPDPContext->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnPDPContextLoad = ui->NewButton(this->pnlPDPContext, CSTR("Load"));
	this->btnPDPContextLoad->SetRect(0, 0, 75, 23, false);
	this->btnPDPContextLoad->HandleButtonClick(OnPDPContextLoadClicked, this);
	this->lblPDPContextStatus = ui->NewLabel(this->pnlPDPContext, CSTR("Status"));
	this->lblPDPContextStatus->SetRect(80, 0, 100, 23, false);
	this->txtPDPContextStatus = ui->NewTextBox(this->pnlPDPContext, CSTR(""));
	this->txtPDPContextStatus->SetRect(180, 0, 200, 23, false);
	this->txtPDPContextStatus->SetReadOnly(true);
	this->btnPDPAttach = ui->NewButton(this->pnlPDPContext, CSTR("Attach"));
	this->btnPDPAttach->SetRect(380, 0, 75, 23, false);
	this->btnPDPAttach->HandleButtonClick(OnPDPAttachClicked, this);
	this->btnPDPDetach = ui->NewButton(this->pnlPDPContext, CSTR("Detach"));
	this->btnPDPDetach->SetRect(460, 0, 75, 23, false);
	this->btnPDPDetach->HandleButtonClick(OnPDPDetachClicked, this);
	this->lblPDPCID = ui->NewLabel(this->pnlPDPContext, CSTR("CID"));
	this->lblPDPCID->SetRect(0, 24, 60, 23, false);
	this->txtPDPCID = ui->NewTextBox(this->pnlPDPContext, CSTR("1"));
	this->txtPDPCID->SetRect(60, 24, 60, 23, false);
	this->lblPDPType = ui->NewLabel(this->pnlPDPContext, CSTR("Type"));
	this->lblPDPType->SetRect(160, 24, 60, 23, false);
	this->txtPDPType = ui->NewTextBox(this->pnlPDPContext, CSTR("IP"));
	this->txtPDPType->SetRect(220, 24, 60, 23, false);
	this->lblPDPAPN = ui->NewLabel(this->pnlPDPContext, CSTR("APN"));
	this->lblPDPAPN->SetRect(320, 24, 60, 23, false);
	this->txtPDPAPN = ui->NewTextBox(this->pnlPDPContext, CSTR(""));
	this->txtPDPAPN->SetRect(380, 24, 160, 23, false);
	this->btnPDPContextSet = ui->NewButton(this->pnlPDPContext, CSTR("Set"));
	this->btnPDPContextSet->SetRect(540, 24, 75, 23, false);
	this->btnPDPContextSet->HandleButtonClick(OnPDPContextSetClicked, this);
	this->btnPDPContextActiveAll = ui->NewButton(this->pnlPDPContext, CSTR("Active All"));
	this->btnPDPContextActiveAll->SetRect(0, 48, 75, 23, false);
	this->btnPDPContextActiveAll->HandleButtonClick(OnPDPContextActiveAllClicked, this);
	this->btnPDPContextDeactiveAll = ui->NewButton(this->pnlPDPContext, CSTR("Deactive All"));
	this->btnPDPContextDeactiveAll->SetRect(80, 48, 75, 23, false);
	this->btnPDPContextDeactiveAll->HandleButtonClick(OnPDPContextDeactiveAllClicked, this);
	this->btnPDPContextActiveSelected = ui->NewButton(this->pnlPDPContext, CSTR("Active Selected"));
	this->btnPDPContextActiveSelected->SetRect(160, 48, 75, 23, false);
	this->btnPDPContextActiveSelected->HandleButtonClick(OnPDPContextActiveSelectedClicked, this);
	this->btnPDPContextDeactiveSelected = ui->NewButton(this->pnlPDPContext, CSTR("Deactive Selected"));
	this->btnPDPContextDeactiveSelected->SetRect(240, 48, 75, 23, false);
	this->btnPDPContextDeactiveSelected->HandleButtonClick(OnPDPContextDeactiveSelectedClicked, this);
	this->lvPDPContext = ui->NewListView(this->tpPDPContext, UI::ListViewStyle::Table, 4);
	this->lvPDPContext->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvPDPContext->SetFullRowSelect(true);
	this->lvPDPContext->SetShowGrid(true);
	this->lvPDPContext->AddColumn(CSTR("Index(cid)"), 60);
	this->lvPDPContext->AddColumn(CSTR("Type"), 60);
	this->lvPDPContext->AddColumn(CSTR("APN"), 200);
	this->lvPDPContext->AddColumn(CSTR("Activate"), 80);

	this->tpATCommand = this->tcMain->AddTabPage(CSTR("AT Command"));
	this->pnlATCommand = ui->NewPanel(this->tpATCommand);
	this->pnlATCommand->SetRect(0, 0, 100, 24, false);
	this->pnlATCommand->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblATCommand = ui->NewLabel(this->pnlATCommand, CSTR("AT Command"));
	this->lblATCommand->SetRect(0, 0, 100, 23, false);
	this->lblATCommand->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->btnATCommand = ui->NewButton(this->pnlATCommand, CSTR("Send"));
	this->btnATCommand->SetRect(0, 0, 75, 23, false);
	this->btnATCommand->SetDockType(UI::GUIControl::DOCK_RIGHT);
	this->btnATCommand->HandleButtonClick(OnATCommandClicked, this);
	this->txtATCommand = ui->NewTextBox(this->pnlATCommand, CSTR("ATZ"));
	this->txtATCommand->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtATCommand->HandleKeyDown(OnATCommandKeyDown, this);
	this->txtATCommandRep = ui->NewTextBox(this->tpATCommand, CSTR(""), true);
	this->txtATCommandRep->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtATCommandRep->SetReadOnly(true);

	this->tpLog = this->tcMain->AddTabPage(CSTR("Log"));
	this->txtLog = ui->NewTextBox(this->tpLog, CSTR(""));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lbLog = ui->NewListBox(this->tpLog, false);
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSelChg, this);
	
	this->tpHuawei = this->tcMain->AddTabPage(CSTR("Huawei"));
	this->lblHuaweiICCID = ui->NewLabel(this->tpHuawei, CSTR("ICCID"));
	this->lblHuaweiICCID->SetRect(0, 0, 100, 23, false);
	this->txtHuaweiICCID = ui->NewTextBox(this->tpHuawei, CSTR(""));
	this->txtHuaweiICCID->SetRect(100, 0, 150, 23, false);
	this->txtHuaweiICCID->SetReadOnly(true);
	this->lblHuaweiSIMType = ui->NewLabel(this->tpHuawei, CSTR("SIM Type"));
	this->lblHuaweiSIMType->SetRect(0, 24, 100, 23, false);
	this->txtHuaweiSIMType = ui->NewTextBox(this->tpHuawei, CSTR(""));
	this->txtHuaweiSIMType->SetRect(100, 24, 150, 23, false);
	this->txtHuaweiSIMType->SetReadOnly(true);
	this->lblHuaweiSrvStatus = ui->NewLabel(this->tpHuawei, CSTR("System Service"));
	this->lblHuaweiSrvStatus->SetRect(0, 48, 100, 23, false);
	this->txtHuaweiSrvStatus = ui->NewTextBox(this->tpHuawei, CSTR(""));
	this->txtHuaweiSrvStatus->SetRect(100, 48, 150, 23, false);
	this->txtHuaweiSrvStatus->SetReadOnly(true);
	this->lblHuaweiSrvDomain = ui->NewLabel(this->tpHuawei, CSTR("Service Domain"));
	this->lblHuaweiSrvDomain->SetRect(0, 72, 100, 23, false);
	this->txtHuaweiSrvDomain = ui->NewTextBox(this->tpHuawei, CSTR(""));
	this->txtHuaweiSrvDomain->SetRect(100, 72, 150, 23, false);
	this->txtHuaweiSrvDomain->SetReadOnly(true);
	this->lblHuaweiRoamStatus = ui->NewLabel(this->tpHuawei, CSTR("Roaming Status"));
	this->lblHuaweiRoamStatus->SetRect(0, 96, 100, 23, false);
	this->txtHuaweiRoamStatus = ui->NewTextBox(this->tpHuawei, CSTR(""));
	this->txtHuaweiRoamStatus->SetRect(100, 96, 100, 23, false);
	this->txtHuaweiRoamStatus->SetReadOnly(true);
	this->lblHuaweiSIMState = ui->NewLabel(this->tpHuawei, CSTR("SIM State"));
	this->lblHuaweiSIMState->SetRect(0, 120, 100, 23, false);
	this->txtHuaweiSIMState = ui->NewTextBox(this->tpHuawei, CSTR(""));
	this->txtHuaweiSIMState->SetRect(100, 120, 150, 23, false);
	this->txtHuaweiSIMState->SetReadOnly(true);
	this->lblHuaweiLockState = ui->NewLabel(this->tpHuawei, CSTR("Lock State"));
	this->lblHuaweiLockState->SetRect(0, 144, 100, 23, false);
	this->txtHuaweiLockState = ui->NewTextBox(this->tpHuawei, CSTR(""));
	this->txtHuaweiLockState->SetRect(100, 144, 150, 23, false);
	this->txtHuaweiLockState->SetReadOnly(true);
	this->lblHuaweiSysMode = ui->NewLabel(this->tpHuawei, CSTR("System Mode"));
	this->lblHuaweiSysMode->SetRect(0, 168, 100, 23, false);
	this->txtHuaweiSysMode = ui->NewTextBox(this->tpHuawei, CSTR(""));
	this->txtHuaweiSysMode->SetRect(100, 168, 150, 23, false);
	this->txtHuaweiSysMode->SetReadOnly(true);
	this->lblHuaweiSubMode = ui->NewLabel(this->tpHuawei, CSTR("System sub-mode"));
	this->lblHuaweiSubMode->SetRect(0, 192, 100, 23, false);
	this->txtHuaweiSubMode = ui->NewTextBox(this->tpHuawei, CSTR(""));
	this->txtHuaweiSubMode->SetRect(100, 192, 150, 23, false);
	this->txtHuaweiSubMode->SetReadOnly(true);
	this->lblHuaweiCSQ1 = ui->NewLabel(this->tpHuawei, CSTR("LTE RSSI"));
	this->lblHuaweiCSQ1->SetRect(0, 216, 100, 23, false);
	this->txtHuaweiCSQ1 = ui->NewTextBox(this->tpHuawei, CSTR(""));
	this->txtHuaweiCSQ1->SetRect(100, 216, 150, 23, false);
	this->txtHuaweiCSQ1->SetReadOnly(true);
	this->lblHuaweiCSQ2 = ui->NewLabel(this->tpHuawei, CSTR("LTE RSRP"));
	this->lblHuaweiCSQ2->SetRect(0, 240, 100, 23, false);
	this->txtHuaweiCSQ2 = ui->NewTextBox(this->tpHuawei, CSTR(""));
	this->txtHuaweiCSQ2->SetRect(100, 240, 150, 23, false);
	this->txtHuaweiCSQ2->SetReadOnly(true);
	this->lblHuaweiCSQ3 = ui->NewLabel(this->tpHuawei, CSTR("LTE SINR"));
	this->lblHuaweiCSQ3->SetRect(0, 264, 100, 23, false);
	this->txtHuaweiCSQ3 = ui->NewTextBox(this->tpHuawei, CSTR(""));
	this->txtHuaweiCSQ3->SetRect(100, 264, 150, 23, false);
	this->txtHuaweiCSQ3->SetReadOnly(true);
	this->lblHuaweiCSQ4 = ui->NewLabel(this->tpHuawei, CSTR("LTE RSRQ"));
	this->lblHuaweiCSQ4->SetRect(0, 288, 100, 23, false);
	this->txtHuaweiCSQ4 = ui->NewTextBox(this->tpHuawei, CSTR(""));
	this->txtHuaweiCSQ4->SetRect(100, 288, 150, 23, false);
	this->txtHuaweiCSQ4->SetReadOnly(true);
	this->grpHuaweiDHCP = ui->NewGroupBox(this->tpHuawei, CSTR("DHCP"));
	this->grpHuaweiDHCP->SetRect(280, 0, 300, 248, false);
	this->btnHuaweiDHCP = ui->NewButton(this->grpHuaweiDHCP, CSTR("Update"));
	this->btnHuaweiDHCP->SetRect(0, 0, 75, 23, false);
	this->btnHuaweiDHCP->HandleButtonClick(OnHuaweiDHCPClicked, this);
	this->lblHuaweiDHCPClientIP = ui->NewLabel(this->grpHuaweiDHCP, CSTR("Client IP"));
	this->lblHuaweiDHCPClientIP->SetRect(0, 24, 100, 23, false);
	this->txtHuaweiDHCPClientIP = ui->NewTextBox(this->grpHuaweiDHCP, CSTR(""));
	this->txtHuaweiDHCPClientIP->SetRect(100, 24, 150, 23, false);
	this->txtHuaweiDHCPClientIP->SetReadOnly(true);
	this->lblHuaweiDHCPNetmask = ui->NewLabel(this->grpHuaweiDHCP, CSTR("Subnet Mask"));
	this->lblHuaweiDHCPNetmask->SetRect(0, 48, 100, 23, false);
	this->txtHuaweiDHCPNetmask = ui->NewTextBox(this->grpHuaweiDHCP, CSTR(""));
	this->txtHuaweiDHCPNetmask->SetRect(100, 48, 150, 23, false);
	this->txtHuaweiDHCPNetmask->SetReadOnly(true);
	this->lblHuaweiDHCPGateway = ui->NewLabel(this->grpHuaweiDHCP, CSTR("Gateway"));
	this->lblHuaweiDHCPGateway->SetRect(0, 72, 100, 23, false);
	this->txtHuaweiDHCPGateway = ui->NewTextBox(this->grpHuaweiDHCP, CSTR(""));
	this->txtHuaweiDHCPGateway->SetRect(100, 72, 150, 23, false);
	this->txtHuaweiDHCPGateway->SetReadOnly(true);
	this->lblHuaweiDHCPServer = ui->NewLabel(this->grpHuaweiDHCP, CSTR("DHCP Server"));
	this->lblHuaweiDHCPServer->SetRect(0, 96, 100, 23, false);
	this->txtHuaweiDHCPServer = ui->NewTextBox(this->grpHuaweiDHCP, CSTR(""));
	this->txtHuaweiDHCPServer->SetRect(100, 96, 150, 23, false);
	this->txtHuaweiDHCPServer->SetReadOnly(true);
	this->lblHuaweiDHCPPriDNS = ui->NewLabel(this->grpHuaweiDHCP, CSTR("Primary DNS"));
	this->lblHuaweiDHCPPriDNS->SetRect(0, 120, 100, 23, false);
	this->txtHuaweiDHCPPriDNS = ui->NewTextBox(this->grpHuaweiDHCP, CSTR(""));
	this->txtHuaweiDHCPPriDNS->SetRect(100, 120, 150, 23, false);
	this->txtHuaweiDHCPPriDNS->SetReadOnly(true);
	this->lblHuaweiDHCPSecDNS = ui->NewLabel(this->grpHuaweiDHCP, CSTR("Seconday DNS"));
	this->lblHuaweiDHCPSecDNS->SetRect(0, 144, 100, 23, false);
	this->txtHuaweiDHCPSecDNS = ui->NewTextBox(this->grpHuaweiDHCP, CSTR(""));
	this->txtHuaweiDHCPSecDNS->SetRect(100, 144, 150, 23, false);
	this->txtHuaweiDHCPSecDNS->SetReadOnly(true);
	this->lblHuaweiDHCPMaxRXbps = ui->NewLabel(this->grpHuaweiDHCP, CSTR("Max TX Speed(bps)"));
	this->lblHuaweiDHCPMaxRXbps->SetRect(0, 168, 100, 23, false);
	this->txtHuaweiDHCPMaxRXbps = ui->NewTextBox(this->grpHuaweiDHCP, CSTR(""));
	this->txtHuaweiDHCPMaxRXbps->SetRect(100, 168, 150, 23, false);
	this->txtHuaweiDHCPMaxRXbps->SetReadOnly(true);
	this->lblHuaweiDHCPMaxTXbps = ui->NewLabel(this->grpHuaweiDHCP, CSTR("Max RX Speed(bps)"));
	this->lblHuaweiDHCPMaxTXbps->SetRect(0, 192, 100, 23, false);
	this->txtHuaweiDHCPMaxTXbps = ui->NewTextBox(this->grpHuaweiDHCP, CSTR(""));
	this->txtHuaweiDHCPMaxTXbps->SetRect(100, 192, 150, 23, false);
	this->txtHuaweiDHCPMaxTXbps->SetReadOnly(true);
	this->grpHuaweiVersion = ui->NewGroupBox(this->tpHuawei, CSTR("Version"));
	this->grpHuaweiVersion->SetRect(280, 248, 300, 200, false);
	this->lblHuaweiBDT = ui->NewLabel(this->grpHuaweiVersion, CSTR("BIOS DateTime"));
	this->lblHuaweiBDT->SetRect(0, 0, 100, 23, false);
	this->txtHuaweiBDT = ui->NewTextBox(this->grpHuaweiVersion, CSTR(""));
	this->txtHuaweiBDT->SetRect(100, 0, 150, 23, false);
	this->txtHuaweiBDT->SetReadOnly(true);
	this->lblHuaweiEXTS = ui->NewLabel(this->grpHuaweiVersion, CSTR("EXTS"));
	this->lblHuaweiEXTS->SetRect(0, 24, 100, 23, false);
	this->txtHuaweiEXTS = ui->NewTextBox(this->grpHuaweiVersion, CSTR(""));
	this->txtHuaweiEXTS->SetRect(100, 24, 150, 23, false);
	this->txtHuaweiEXTS->SetReadOnly(true);
	this->lblHuaweiEXTD = ui->NewLabel(this->grpHuaweiVersion, CSTR("EXTD"));
	this->lblHuaweiEXTD->SetRect(0, 48, 100, 23, false);
	this->txtHuaweiEXTD = ui->NewTextBox(this->grpHuaweiVersion, CSTR(""));
	this->txtHuaweiEXTD->SetRect(100, 48, 200, 23, false);
	this->txtHuaweiEXTD->SetReadOnly(true);
	this->lblHuaweiEXTH = ui->NewLabel(this->grpHuaweiVersion, CSTR("EXTH"));
	this->lblHuaweiEXTH->SetRect(0, 72, 100, 23, false);
	this->txtHuaweiEXTH = ui->NewTextBox(this->grpHuaweiVersion, CSTR(""));
	this->txtHuaweiEXTH->SetRect(100, 72, 200, 23, false);
	this->txtHuaweiEXTH->SetReadOnly(true);
	this->lblHuaweiEXTU = ui->NewLabel(this->grpHuaweiVersion, CSTR("EXTV"));
	this->lblHuaweiEXTU->SetRect(0, 96, 100, 23, false);
	this->txtHuaweiEXTU = ui->NewTextBox(this->grpHuaweiVersion, CSTR(""));
	this->txtHuaweiEXTU->SetRect(100, 96, 150, 23, false);
	this->txtHuaweiEXTU->SetReadOnly(true);
	this->lblHuaweiCFG = ui->NewLabel(this->grpHuaweiVersion, CSTR("CFG"));
	this->lblHuaweiCFG->SetRect(0, 120, 100, 23, false);
	this->txtHuaweiCFG = ui->NewTextBox(this->grpHuaweiVersion, CSTR(""));
	this->txtHuaweiCFG->SetRect(100, 120, 150, 23, false);
	this->txtHuaweiCFG->SetReadOnly(true);
	this->lblHuaweiINI = ui->NewLabel(this->grpHuaweiVersion, CSTR("INI"));
	this->lblHuaweiINI->SetRect(0, 144, 100, 23, false);
	this->txtHuaweiINI = ui->NewTextBox(this->grpHuaweiVersion, CSTR(""));
	this->txtHuaweiINI->SetRect(100, 144, 200, 23, false);
	this->txtHuaweiINI->SetReadOnly(true);

	this->toStop = false;
	this->running = false;
	NEW_CLASSNN(this->logger, UI::ListBoxLogger(*this, this->lbLog, 200, false));
	this->log.AddLogHandler(this->logger, IO::LogHandler::LogLevel::Raw);

	this->AddTimer(1000, OnTimerTick, this);

	Data::ArrayList<UOSInt> ports;
	Data::ArrayList<IO::SerialPort::SerialPortType> portTypes;
	if (IO::SerialPort::GetAvailablePorts(ports, &portTypes))
	{
		UTF8Char sbuff[256];
		UnsafeArray<UTF8Char> sptr;
		UOSInt i = 0;
		UOSInt j = ports.GetCount();
		while (i < j)
		{
			sbuff[0] = 0;
			sptr = IO::SerialPort::GetPortName(sbuff, ports.GetItem(i)).Or(sbuff);
			this->cboDeviceSerial->AddItem(CSTRP(sbuff, sptr), (void*)ports.GetItem(i));
			i++;
		}
		if (j > 0)
		{
			this->cboDeviceSerial->SetSelectedIndex(0);
		}
	}

	NN<IO::Stream> nnPort;
	if (port.SetTo(nnPort))
	{
		this->InitStream(nnPort, true);
	}
}

SSWR::AVIRead::AVIRGSMModemForm::~AVIRGSMModemForm()
{
	this->log.RemoveLogHandler(this->logger);
	this->logger.Delete();
	this->CloseStream(false);

	OPTSTR_DEL(this->operName);
	OPTSTR_DEL(this->initModemManu);
	OPTSTR_DEL(this->initModemModel);
	OPTSTR_DEL(this->initModemVer);
	OPTSTR_DEL(this->initIMEI);
	OPTSTR_DEL(this->huaweiICCID);
	OPTSTR_DEL(this->simIMSI);
	OPTSTR_DEL(this->cfgTECharset);
	IO::Device::HuaweiGSMModemController::FreeVersionInfo(this->huaweiVersion);
}

void SSWR::AVIRead::AVIRGSMModemForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
