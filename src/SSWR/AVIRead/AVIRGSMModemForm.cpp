#include "Stdafx.h"
#include "Text/MyString.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/SerialPort.h"
#include "IO/StreamWriter.h"
#include "SSWR/AVIRead/AVIRGSMModemForm.h"
#include "Sync/Thread.h"
#include "Text/MyStringW.h"
#include "Text/UTF8Writer.h"
#include "UI/FileDialog.h"
#include "UI/FolderDialog.h"
#include "UI/MessageDialog.h"

UInt32 __stdcall SSWR::AVIRead::AVIRGSMModemForm::ModemThread(void *userObj)
{
	SSWR::AVIRead::AVIRGSMModemForm *me = (SSWR::AVIRead::AVIRGSMModemForm*)userObj;
	Data::Timestamp currTime;
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Bool init = false;
	IO::GSMModemController::BER ber;

	currTime = Data::Timestamp();
	me->running = true;

	while (!me->toStop)
	{
		if (!init)
		{
			init = true;
			if ((sptr = me->modem->GSMGetManufacturer(sbuff)) != 0)
				me->initModemManu = Text::String::NewP(sbuff, sptr);
			if ((sptr = me->modem->GSMGetModelIdent(sbuff)) != 0)
				me->initModemModel = Text::String::NewP(sbuff, sptr);
			if ((sptr = me->modem->GSMGetModemVer(sbuff)) != 0)
				me->initModemVer = Text::String::NewP(sbuff, sptr);
			if ((sptr = me->modem->GSMGetIMEI(sbuff)) != 0)
				me->initIMEI = Text::String::NewP(sbuff, sptr);
			me->initStrs = true;

			if ((sptr = me->modem->GSMGetTECharset(sbuff)) != 0)
			{
				me->cfgTECharset = Text::String::NewP(sbuff, sptr);
				me->cfgTECharsetUpd = true;
			}
		}
		if (me->simChanged)
		{
			me->simChanged = false;
			if ((sptr = me->modem->GSMGetIMSI(sbuff)) != 0)
			{
				SDEL_STRING(me->simIMSI);
				me->simIMSI = Text::String::NewP(sbuff, sptr);
			}
			me->simInfoUpdated = true;
		}

		currTime = Data::Timestamp::UtcNow();
		if (currTime >= me->operNextTime)
		{
			me->operNextTime = me->operNextTime.AddSecond(30);
			if ((sptr = me->modem->GSMGetCurrPLMN(sbuff)) != 0)
			{
				SDEL_STRING(me->operName);
				me->operName = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
				me->operUpdated = true;
			}
			if (me->modem->GSMGetRegisterNetwork(&me->regNetN, &me->regNetStat, &me->regNetLAC, &me->regNetCI, &me->regNetACT))
			{
				me->regNetUpdated = true;
			}
		}
		me->modem->GSMGetSignalQuality(&me->signalQuality, &ber);
		me->modemEvt.Wait(1000);
	}
	me->running = false;
	return 0;
}

void __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnTimerTick(void *userObj)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	SSWR::AVIRead::AVIRGSMModemForm *me = (SSWR::AVIRead::AVIRGSMModemForm*)userObj;

	if (me->initStrs)
	{
		me->initStrs = false;
		if (me->initModemManu)
		{
			me->txtModemManu->SetText(me->initModemManu->ToCString());
		}
		if (me->initModemModel)
		{
			me->txtModemModel->SetText(me->initModemModel->ToCString());
		}
		if (me->initModemVer)
		{
			me->txtModemVer->SetText(me->initModemVer->ToCString());
		}
		if (me->initIMEI)
		{
			me->txtModemIMEI->SetText(me->initIMEI->ToCString());
		}
	}
	if (me->simInfoUpdated)
	{
		me->simInfoUpdated = false;
		me->txtIMSI->SetText(me->simIMSI->ToCString());
	}
	if (me->cfgTECharsetUpd)
	{
		me->cfgTECharsetUpd = false;
		me->txtTECharset->SetText(me->cfgTECharset->ToCString());
	}

	if (me->operUpdated)
	{
		me->operUpdated = false;
		me->txtOperator->SetText(me->operName->ToCString());
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

	sptr = IO::GSMModemController::GetRSSIString(sbuff, me->signalQuality);
	me->txtSignalQuality->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnPhoneRClick(void *userObj)
{
	SSWR::AVIRead::AVIRGSMModemForm *me = (SSWR::AVIRead::AVIRGSMModemForm*)userObj;
	if (me->port)
	{
		me->LoadPhoneBook();
	}
}

void __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnSMSRClick(void *userObj)
{
	SSWR::AVIRead::AVIRGSMModemForm *me = (SSWR::AVIRead::AVIRGSMModemForm*)userObj;
	if (me->port)
	{
		me->LoadSMS();
	}
}

void __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnSMSSaveClick(void *userObj)
{
	SSWR::AVIRead::AVIRGSMModemForm *me = (SSWR::AVIRead::AVIRGSMModemForm*)userObj;
	IO::GSMModemController::SMSMessage *sms;
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	Text::SMSMessage *smsMsg;
	sms = (IO::GSMModemController::SMSMessage *)me->lvSMS->GetSelectedItem();
	if (sms)
	{
		Data::DateTime dt;
		smsMsg = Text::SMSMessage::CreateFromPDU(sms->pduMessage);
		smsMsg->GetMessageTime(&dt);

		UI::FileDialog dlg(L"SSWR", L"AVIRead", L"SMSSave", true);
		dlg.AddFilter(CSTR("*.sms"), CSTR("SMS File"));
		
		sptr = Text::StrConcatC(sbuff, UTF8STRC("SMS"));
		sptr = Text::StrInt64(sptr, dt.ToDotNetTicks());
		sptr = Text::StrConcatC(sptr, UTF8STRC("_"));
		sptr = Text::StrInt32(sptr, sms->index);
		sptr = Text::StrConcatC(sptr, UTF8STRC("_"));
		sptr = Text::StrUTF16_UTF8(sptr, smsMsg->GetAddress());
		sptr = Text::StrConcatC(sptr, UTF8STRC(".sms"));
		dlg.SetFileName(CSTRP(sbuff, sptr));

		if (dlg.ShowDialog(me->GetHandle()))
		{
			IO::FileStream fs(dlg.GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			Text::UTF8Writer writer(&fs);
			writer.WriteSignature();
			writer.WriteStrC(UTF8STRC("From: "));
			writer.WriteLineW(smsMsg->GetAddress());
			if (smsMsg->GetSMSC())
			{
				writer.WriteStrC(UTF8STRC("SMSC: "));
				writer.WriteLineW(smsMsg->GetSMSC());
			}
			writer.WriteStrC(UTF8STRC("Date: "));
			sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss zzzz");
			writer.WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
			writer.WriteLineC(UTF8STRC("Content: "));
			writer.WriteLineW(smsMsg->GetContent());
		}
		DEL_CLASS(smsMsg);
	}
}

void __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnSMSDeleteClick(void *userObj)
{
	SSWR::AVIRead::AVIRGSMModemForm *me = (SSWR::AVIRead::AVIRGSMModemForm*)userObj;
	if (me->port == 0)
	{
		return;
	}
	IO::GSMModemController::SMSMessage *sms;
	UOSInt index = me->lvSMS->GetSelectedIndex();
	if (index != INVALID_INDEX)
	{
		sms = me->msgList.GetItem(index);
		if (me->modem->SMSDeleteMessage(sms->index))
		{
			me->modem->SMSFreeMessage(sms);
			me->lvSMS->RemoveItem(index);
			me->msgList.RemoveAt(index);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnSMSSaveAllClick(void *userObj)
{
	SSWR::AVIRead::AVIRGSMModemForm *me = (SSWR::AVIRead::AVIRGSMModemForm*)userObj;
	IO::GSMModemController::SMSMessage *sms;
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	Text::SMSMessage *smsMsg;
	if (me->lvSMS->GetCount() > 0)
	{
		UI::FolderDialog dlg(L"SSWR", L"AVIRead", L"SMSSaveAll");
		if (dlg.ShowDialog(me->GetHandle()))
		{
			UOSInt i = 0;
			UOSInt j = me->lvSMS->GetCount();
			Text::StringBuilderUTF8 sb;
			Data::DateTime dt;
			while (i < j)
			{
				sms = (IO::GSMModemController::SMSMessage *)me->lvSMS->GetItem(i);
				smsMsg = Text::SMSMessage::CreateFromPDU(sms->pduMessage);
				smsMsg->GetMessageTime(&dt);
				sb.ClearStr();
				sb.Append(dlg.GetFolder());
				sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
				sb.AppendC(UTF8STRC("SMS"));
				sb.AppendI64(dt.ToDotNetTicks());
				sb.AppendC(UTF8STRC("_"));
				sb.AppendI32(sms->index);
				sb.AppendC(UTF8STRC("_"));
				Text::String *s = Text::String::NewNotNull(smsMsg->GetAddress());
				sb.Append(s);
				s->Release();
				sb.AppendC(UTF8STRC(".sms"));

				IO::FileStream fs(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
				Text::UTF8Writer writer(&fs);
				writer.WriteSignature();
				writer.WriteStrC(UTF8STRC("From: "));
				writer.WriteLineW(smsMsg->GetAddress());
				if (smsMsg->GetSMSC())
				{
					writer.WriteStrC(UTF8STRC("SMSC: "));
					writer.WriteLineW(smsMsg->GetSMSC());
				}
				writer.WriteStrC(UTF8STRC("Date: "));
				sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss zzzz");
				writer.WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
				writer.WriteLineC(UTF8STRC("Content: "));
				writer.WriteLineW(smsMsg->GetContent());
				i++;
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnDeviceSerialClk(void *userObj)
{
	SSWR::AVIRead::AVIRGSMModemForm *me = (SSWR::AVIRead::AVIRGSMModemForm*)userObj;
	IO::SerialPort *port;
	NEW_CLASS(port, IO::SerialPort((UOSInt)me->cboDeviceSerial->GetSelectedItem(), 115200, IO::SerialPort::PARITY_NONE, false));
	if (port->IsError())
	{
		DEL_CLASS(port);
		me->txtDeviceStatus->SetText(CSTR("Error in opening port"));
	}
	else
	{
		me->InitStream(port, false);
	}
}

void __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnDeviceOtherClk(void *userObj)
{
	SSWR::AVIRead::AVIRGSMModemForm *me = (SSWR::AVIRead::AVIRGSMModemForm*)userObj;
	if (me->port)
	{
		me->CloseStream(true);
	}
	else
	{
		IO::StreamType st;
		IO::Stream *port = me->core->OpenStream(&st, me, 115200, false);
		if (port)
		{
			me->InitStream(port, true);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnLogSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRGSMModemForm *me = (SSWR::AVIRead::AVIRGSMModemForm*)userObj;
	Text::String *s = me->lbLog->GetSelectedItemTextNew();
	if (s)
	{
		me->txtLog->SetText(s->ToCString());
		s->Release();
	}
	else
	{
		me->txtLog->SetText(CSTR(""));
	}
}

void __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnATCommandClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGSMModemForm *me = (SSWR::AVIRead::AVIRGSMModemForm*)userObj;
	if (me->port == 0)
	{
		return;
	}
	Text::StringBuilderUTF8 sb;
	me->txtATCommand->GetText(&sb);
	if (sb.StartsWith(UTF8STRC("AT")))
	{
		Data::ArrayList<Text::String*> ret;
		if (me->channel->SendATCommand(&ret, sb.ToString(), sb.GetLength(), 3000))
		{
			sb.ClearStr();
			Text::String *s;
			UOSInt i = 0;
			UOSInt j = ret.GetCount();
			while (i < j)
			{
				s = ret.GetItem(i);
				if (i > 0)
					sb.AppendC(UTF8STRC("\r\n"));
				sb.Append(s);
				s->Release();
				i++;
			}
			me->txtATCommand->SetText(CSTR(""));
			me->txtATCommandRep->SetText(sb.ToCString());
		}
	}
}

Bool __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnATCommandKeyDown(void *userObj, UInt32 osKey)
{
	SSWR::AVIRead::AVIRGSMModemForm *me = (SSWR::AVIRead::AVIRGSMModemForm*)userObj;
	UI::GUIControl::GUIKey key = UI::GUIControl::OSKey2GUIKey(osKey);
	if (key == UI::GUIControl::GK_ENTER)
	{
		OnATCommandClicked(me);
		return true;
	}
	return false;
}

void __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnPDPContextLoadClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGSMModemForm *me = (SSWR::AVIRead::AVIRGSMModemForm*)userObj;
	if (me->port == 0)
	{
		return;
	}
	me->LoadPDPContext();
}

void __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnPDPAttachClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGSMModemForm *me = (SSWR::AVIRead::AVIRGSMModemForm*)userObj;
	if (me->port == 0)
	{
		return;
	}
	if (me->modem->GPRSServiceSetAttached(true))
	{
		me->txtPDPContextStatus->SetText(CSTR("Attached"));
	}
	else
	{
		me->txtPDPContextStatus->SetText(CSTR("Error in attaching"));
	}
}

void __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnPDPDetachClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGSMModemForm *me = (SSWR::AVIRead::AVIRGSMModemForm*)userObj;
	if (me->port == 0)
	{
		return;
	}
	if (me->modem->GPRSServiceSetAttached(false))
	{
		me->txtPDPContextStatus->SetText(CSTR("Detacted"));
	}
	else
	{
		me->txtPDPContextStatus->SetText(CSTR("Error in detaching"));
	}
}

void __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnPDPContextSetClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGSMModemForm *me = (SSWR::AVIRead::AVIRGSMModemForm*)userObj;
	if (me->port == 0)
	{
		return;
	}
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	me->txtPDPCID->GetText(&sb);
	UInt32 cid;
	if (!sb.ToUInt32(&cid))
	{
		me->txtPDPContextStatus->SetText(CSTR("CID invalid"));
		return;
	}
	sb.ClearStr();
	me->txtPDPType->GetText(&sb);
	if (!sb.Equals(UTF8STRC("IP")) && !sb.Equals(UTF8STRC("PPP")) && !sb.Equals(UTF8STRC("IPV6")) && !sb.Equals(UTF8STRC("IPV4V6")))
	{
		me->txtPDPContextStatus->SetText(CSTR("Type invalid"));
		return;
	}
	me->txtPDPAPN->GetText(&sb2);
	if (sb2.GetLength() > 99)
	{
		me->txtPDPContextStatus->SetText(CSTR("APN too long"));
		return;
	}
	if (me->modem->GPRSSetPDPContext(cid, sb.ToCString(), sb2.ToCString()))
	{
		me->txtPDPContextStatus->SetText(CSTR("PDP Context set"));
	}
	else
	{
		me->txtPDPContextStatus->SetText(CSTR("Error in setting PDP Context"));
	}
}

void SSWR::AVIRead::AVIRGSMModemForm::LoadPhoneBook()
{
	IO::GSMModemController::PBStorage store = (IO::GSMModemController::PBStorage)(OSInt)this->cboPhoneStorage->GetItem((UOSInt)this->cboPhoneStorage->GetSelectedIndex());
	UOSInt i;
	UOSInt j;
	UOSInt k;
	Data::ArrayList<IO::GSMModemController::PBEntry*> phoneList;
	IO::GSMModemController::PBEntry *ent;
	this->lvPhone->ClearItems();
	if (!this->modem->PBSetStorage(store))
	{
		this->lblPhoneStatus->SetText(CSTR("Error in setting Phonebook Storage"));
		return;
	}
	if (!this->modem->PBReadAllEntries(&phoneList))
	{
		this->lblPhoneStatus->SetText(CSTR("Error in reading Phonebook entries"));
		return;
	}
	i = 0;
	j = phoneList.GetCount();
	while (i < j)
	{
		ent = phoneList.GetItem(i);
		k = this->lvPhone->AddItem(ent->name, 0);
		this->lvPhone->SetSubItem(k, 1, ent->number);
		i++;
	}
	this->modem->PBFreeEntries(&phoneList);
	this->lblPhoneStatus->SetText(CSTR("Success"));
}

void SSWR::AVIRead::AVIRGSMModemForm::LoadSMS()
{
	IO::GSMModemController::SMSMessage *sms;
	Text::SMSMessage *smsMsg;
	Data::DateTime dt;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
#if _WCHAR_SIZE == 4
	WChar wbuff[64];
#endif
	UOSInt k;
	UOSInt i;
	UOSInt j;
	this->modem->SMSFreeMessages(&this->msgList);
	this->lvSMS->ClearItems();
	this->msgList.Clear();

	IO::GSMModemController::SMSStorage store = (IO::GSMModemController::SMSStorage)(OSInt)this->cboSMSStorage->GetItem((UOSInt)this->cboSMSStorage->GetSelectedIndex());

	this->modem->SMSSetStorage(store, IO::GSMModemController::SMSSTORE_SIM, IO::GSMModemController::SMSSTORE_SIM);
	sptr = this->modem->SMSGetSMSC(sbuff);
	if (sptr)
	{
		this->txtSMSC->SetText(CSTRP(sbuff, sptr));
	}
	else
	{
		this->txtSMSC->SetText(CSTR(""));
	}
	this->modem->SMSListMessages(&this->msgList, IO::GSMModemController::SMSS_ALL);

	i = 0;
	j = this->msgList.GetCount();
	while (i < j)
	{
		sms = this->msgList.GetItem(i);
		smsMsg = Text::SMSMessage::CreateFromPDU(sms->pduMessage);
#if _WCHAR_SIZE == 2
		k = this->lvSMS->AddItem(smsMsg->GetAddress(), sms);
#elif _WCHAR_SIZE == 4
		Text::StrUTF16_UTF32(wbuff, smsMsg->GetAddress());
		k = this->lvSMS->AddItem(wbuff, sms);
#endif
		smsMsg->GetMessageTime(&dt);
		sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss zzzz");
		this->lvSMS->SetSubItem(k, 1, CSTRP(sbuff, sptr));
		const UTF16Char *cont = smsMsg->GetContent();
		if (cont)
		{
#if _WCHAR_SIZE == 2
			this->lvSMS->SetSubItem(k, 2, cont);
#elif _WCHAR_SIZE == 4
			Text::StrUTF16_UTF32(wbuff, cont);
			this->lvSMS->SetSubItem(k, 2, wbuff);
#endif
		}
		DEL_CLASS(smsMsg);
		i++;
	}
}

void SSWR::AVIRead::AVIRGSMModemForm::LoadPDPContext()
{
	Data::ArrayList<IO::GSMModemController::PDPContext*> ctxList;
	if (!this->modem->GPRSGetPDPContext(&ctxList))
	{
		this->txtPDPContextStatus->SetText(CSTR("Error in getting PDP Context"));
		return;
	}
	this->lvPDPContext->ClearItems();
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	IO::GSMModemController::PDPContext *ctx;
	Data::UInt32FastMap<UOSInt> dataMap;
	UOSInt i = 0;
	UOSInt j = ctxList.GetCount();
	UOSInt k;
	while (i < j)
	{
		ctx = ctxList.GetItem(i);
		sptr = Text::StrUInt32(sbuff, ctx->cid);
		k = this->lvPDPContext->AddItem(CSTRP(sbuff, sptr), (void*)(UOSInt)ctx->cid);
		dataMap.Put(ctx->cid, k);
		this->lvPDPContext->SetSubItem(k, 1, ctx->type);
		this->lvPDPContext->SetSubItem(k, 2, ctx->apn);
		i++;
	}
	this->modem->GPRSFreePDPContext(&ctxList);

	Data::ArrayList<IO::GSMModemController::ActiveState> actList;
	if (!this->modem->GPRSGetPDPActive(&actList))
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
	if (!this->modem->GPRSServiceIsAttached(&attached))
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

void SSWR::AVIRead::AVIRGSMModemForm::InitStream(IO::Stream *stm, Bool updateSerial)
{
	if (this->port == 0)
	{
		this->port = stm;
		this->txtDeviceStatus->SetText(IO::StreamTypeGetName(this->port->GetStreamType()));
		if (updateSerial && this->port->GetStreamType() == IO::StreamType::SerialPort)
		{
			IO::SerialPort *port = (IO::SerialPort*)this->port;
			UOSInt portNum = port->GetPortNum();
			UOSInt i = this->cboDeviceSerial->GetCount();
			while (i-- > 0)
			{
				if ((UOSInt)this->cboDeviceSerial->GetItem(i) == portNum)
				{
					this->cboDeviceSerial->SetSelectedIndex(i);
					break;
				}
			}
		}
		this->cboDeviceSerial->SetEnabled(false);
		this->btnDeviceSerial->SetEnabled(false);
		this->btnDeviceOther->SetText(CSTR("Close"));

		NEW_CLASS(this->channel, IO::ATCommandChannel(this->port, false));
		this->channel->SetLogger(&this->log);
		NEW_CLASS(this->modem, IO::GSMModemController(this->channel, false));

		this->simChanged = true;
		this->toStop = false;
		this->running = false;
		Sync::Thread::Create(ModemThread, this);
		while (!this->running)
		{
			Sync::Thread::Sleep(10);
		}
	}
}

void SSWR::AVIRead::AVIRGSMModemForm::CloseStream(Bool updateUI)
{
	if (this->port)
	{
		this->toStop = true;
		this->modemEvt.Set();
		this->port->Close();
		this->channel->Close();
		while (this->running)
		{
			Sync::Thread::Sleep(10);
		}

		this->modem->SMSFreeMessages(&this->msgList);

		DEL_CLASS(this->modem);
		DEL_CLASS(this->channel);
		DEL_CLASS(this->port);
		this->modem = 0;
		this->channel = 0;
		this->port = 0;

		if (updateUI)
		{
			this->cboDeviceSerial->SetEnabled(true);
			this->btnDeviceSerial->SetEnabled(true);
			this->btnDeviceOther->SetText(CSTR("Open Stream"));
			this->txtDeviceStatus->SetText(CSTR(""));
		}
	}
}

SSWR::AVIRead::AVIRGSMModemForm::AVIRGSMModemForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, IO::Stream *port) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("GSM Modem"));
	this->SetFont(0, 0, 8.25, false);
	this->core = core;
	this->modem = 0;
	this->channel = 0;
	this->port = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->signalQuality = IO::GSMModemController::RSSI_UNKNOWN;
	this->operUpdated = false;
	this->operName = 0;
	this->initStrs = 0;
	this->initModemManu = 0;
	this->initModemModel = 0;
	this->initModemVer = 0;
	this->initIMEI = 0;
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

	NEW_CLASS(this->pnlDevice, UI::GUIPanel(ui, this));
	this->pnlDevice->SetRect(0, 0, 100, 48, false);
	this->pnlDevice->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblDeviceSerial, UI::GUILabel(ui, this->pnlDevice, CSTR("Serial Port")));
	this->lblDeviceSerial->SetRect(0, 0, 100, 23, false);
	NEW_CLASS(this->cboDeviceSerial, UI::GUIComboBox(ui, this->pnlDevice, false));
	this->cboDeviceSerial->SetRect(100, 0, 150, 23, false);
	NEW_CLASS(this->btnDeviceSerial, UI::GUIButton(ui, this->pnlDevice, CSTR("Open")));
	this->btnDeviceSerial->SetRect(250, 0, 75, 23, false);
	this->btnDeviceSerial->HandleButtonClick(OnDeviceSerialClk, this);
	NEW_CLASS(this->lblDeviceStatus, UI::GUILabel(ui, this->pnlDevice, CSTR("Status")));
	this->lblDeviceStatus->SetRect(0, 24, 100, 23, false);
	NEW_CLASS(this->txtDeviceStatus, UI::GUITextBox(ui, this->pnlDevice, CSTR("")));
	this->txtDeviceStatus->SetRect(100, 24, 150, 23, false);
	this->txtDeviceStatus->SetReadOnly(true);
	NEW_CLASS(this->btnDeviceOther, UI::GUIButton(ui, this->pnlDevice, CSTR("Open Stream")));
	this->btnDeviceOther->SetRect(250, 24, 75, 23, false);
	this->btnDeviceOther->HandleButtonClick(OnDeviceOtherClk, this);
	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpInfo = this->tcMain->AddTabPage(CSTR("Info"));
	this->tpPhoneBook = this->tcMain->AddTabPage(CSTR("PhoneBook"));
	this->tpSMS = this->tcMain->AddTabPage(CSTR("SMS"));
	NEW_CLASS(this->lblModemManu, UI::GUILabel(ui, this->tpInfo, CSTR("Manufacture")));
	this->lblModemManu->SetRect(8, 8, 100, 23, false);
	NEW_CLASS(this->txtModemManu, UI::GUITextBox(ui, this->tpInfo, CSTR("")));
	this->txtModemManu->SetRect(108, 8, 100, 23, false);
	this->txtModemManu->SetReadOnly(true);
	NEW_CLASS(this->lblModemModel, UI::GUILabel(ui, this->tpInfo, CSTR("Model")));
	this->lblModemModel->SetRect(8, 32, 100, 23, false);
	NEW_CLASS(this->txtModemModel, UI::GUITextBox(ui, this->tpInfo, CSTR("")));
	this->txtModemModel->SetRect(108, 32, 100, 23, false);
	this->txtModemModel->SetReadOnly(true);
	NEW_CLASS(this->lblModemVer, UI::GUILabel(ui, this->tpInfo, CSTR("Modem Ver")));
	this->lblModemVer->SetRect(8, 56, 100, 23, false);
	NEW_CLASS(this->txtModemVer, UI::GUITextBox(ui, this->tpInfo, CSTR("")));
	this->txtModemVer->SetRect(108, 56, 100, 23, false);
	this->txtModemVer->SetReadOnly(true);
	NEW_CLASS(this->lblModemIMEI, UI::GUILabel(ui, this->tpInfo, CSTR("IMEI")));
	this->lblModemIMEI->SetRect(8, 80, 100, 23, false);
	NEW_CLASS(this->txtModemIMEI, UI::GUITextBox(ui, this->tpInfo, CSTR("")));
	this->txtModemIMEI->SetRect(108, 80, 100, 23, false);
	this->txtModemIMEI->SetReadOnly(true);
	NEW_CLASS(this->lblIMSI, UI::GUILabel(ui, this->tpInfo, CSTR("IMSI")));
	this->lblIMSI->SetRect(8, 104, 100, 23, false);
	NEW_CLASS(this->txtIMSI, UI::GUITextBox(ui, this->tpInfo, CSTR("")));
	this->txtIMSI->SetRect(108, 104, 100, 23, false);
	this->txtIMSI->SetReadOnly(true);
	NEW_CLASS(this->lblTECharset, UI::GUILabel(ui, this->tpInfo, CSTR("TE Charset")));
	this->lblTECharset->SetRect(8, 128, 100, 23, false);
	NEW_CLASS(this->txtTECharset, UI::GUITextBox(ui, this->tpInfo, CSTR("")));
	this->txtTECharset->SetRect(108, 128, 100, 23, false);
	this->txtTECharset->SetReadOnly(true);
	NEW_CLASS(this->lblOperator, UI::GUILabel(ui, this->tpInfo, CSTR("Operator")));
	this->lblOperator->SetRect(8, 152, 100, 23, false);
	NEW_CLASS(this->txtOperator, UI::GUITextBox(ui, this->tpInfo, CSTR("")));
	this->txtOperator->SetRect(108, 152, 100, 23, false);
	this->txtOperator->SetReadOnly(true);
	NEW_CLASS(this->lblRegStatus, UI::GUILabel(ui, this->tpInfo, CSTR("Register Status")));
	this->lblRegStatus->SetRect(8, 176, 100, 23, false);
	NEW_CLASS(this->txtRegStatus, UI::GUITextBox(ui, this->tpInfo, CSTR("")));
	this->txtRegStatus->SetRect(108, 176, 100, 23, false);
	this->txtRegStatus->SetReadOnly(true);
	NEW_CLASS(this->lblLAC, UI::GUILabel(ui, this->tpInfo, CSTR("LAC")));
	this->lblLAC->SetRect(8, 200, 100, 23, false);
	NEW_CLASS(this->txtLAC, UI::GUITextBox(ui, this->tpInfo, CSTR("")));
	this->txtLAC->SetRect(108, 200, 100, 23, false);
	this->txtLAC->SetReadOnly(true);
	NEW_CLASS(this->lblCI, UI::GUILabel(ui, this->tpInfo, CSTR("CI")));
	this->lblCI->SetRect(8, 224, 100, 23, false);
	NEW_CLASS(this->txtCI, UI::GUITextBox(ui, this->tpInfo, CSTR("")));
	this->txtCI->SetRect(108, 224, 100, 23, false);
	this->txtCI->SetReadOnly(true);
	NEW_CLASS(this->lblACT, UI::GUILabel(ui, this->tpInfo, CSTR("Access Tech")));
	this->lblACT->SetRect(8, 248, 100, 23, false);
	NEW_CLASS(this->txtACT, UI::GUITextBox(ui, this->tpInfo, CSTR("")));
	this->txtACT->SetRect(108, 248, 200, 23, false);
	this->txtACT->SetReadOnly(true);
	NEW_CLASS(this->lblSignalQuality, UI::GUILabel(ui, this->tpInfo, CSTR("Signal Quality")));
	this->lblSignalQuality->SetRect(8, 272, 100, 23, false);
	NEW_CLASS(this->txtSignalQuality, UI::GUITextBox(ui, this->tpInfo, CSTR("")));
	this->txtSignalQuality->SetRect(108, 272, 100, 23, false);
	this->txtSignalQuality->SetReadOnly(true);

	NEW_CLASS(this->pnlPhone, UI::GUIPanel(ui, this->tpPhoneBook));
	this->pnlPhone->SetRect(0, 0, 100, 32, false);
	this->pnlPhone->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->cboPhoneStorage, UI::GUIComboBox(ui, this->pnlPhone, false));
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
	NEW_CLASS(this->btnPhoneRead, UI::GUIButton(ui, this->pnlPhone, CSTR("&Read")));
	this->btnPhoneRead->SetRect(108, 4, 75, 23, false);
	this->btnPhoneRead->HandleButtonClick(OnPhoneRClick, this);
	NEW_CLASS(this->lblPhoneStatus, UI::GUILabel(ui, this->pnlPhone, CSTR("")));
	this->lblPhoneStatus->SetRect(188, 4, 200, 23, false);
	NEW_CLASS(this->lvPhone, UI::GUIListView(ui, this->tpPhoneBook, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvPhone->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvPhone->AddColumn(CSTR("Name"), 200);
	this->lvPhone->AddColumn(CSTR("Phone"), 200);
	this->lvPhone->SetFullRowSelect(true);
	this->lvPhone->SetShowGrid(true);

	NEW_CLASS(this->pnlSMS, UI::GUIPanel(ui, this->tpSMS));
	this->pnlSMS->SetRect(0, 0, 100, 32, false);
	this->pnlSMS->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->cboSMSStorage, UI::GUIComboBox(ui, this->pnlSMS, false));
	this->cboSMSStorage->SetRect(0, 4, 100, 23, false);
	this->cboSMSStorage->AddItem(CSTR("SIM"), (void*)(OSInt)IO::GSMModemController::SMSSTORE_SIM);
	this->cboSMSStorage->AddItem(CSTR("Flash"), (void*)(OSInt)IO::GSMModemController::SMSSTORE_FLASH);
	this->cboSMSStorage->AddItem(CSTR("Status Report"), (void*)(OSInt)IO::GSMModemController::SMSSTORE_STATUSREPORT);
	this->cboSMSStorage->AddItem(CSTR("CBM"), (void*)(OSInt)IO::GSMModemController::SMSSTORE_CBMMESSAGE);
	this->cboSMSStorage->SetSelectedIndex(0);
	NEW_CLASS(this->btnSMSRead, UI::GUIButton(ui, this->pnlSMS, CSTR("&Show SMS")));
	this->btnSMSRead->SetRect(108, 4, 75, 23, false);
	this->btnSMSRead->HandleButtonClick(OnSMSRClick, this);
	NEW_CLASS(this->lblSMSC, UI::GUILabel(ui, this->pnlSMS, CSTR("SMSC")));
	this->lblSMSC->SetRect(208, 4, 50, 23, false);
	NEW_CLASS(this->txtSMSC, UI::GUITextBox(ui, this->pnlSMS, CSTR("")));
	this->txtSMSC->SetReadOnly(true);
	this->txtSMSC->SetRect(258, 4, 100, 23, false);
	NEW_CLASS(this->btnSMSSave, UI::GUIButton(ui, this->pnlSMS, CSTR("S&ave SMS")));
	this->btnSMSSave->SetRect(368, 4, 75, 23, false);
	this->btnSMSSave->HandleButtonClick(OnSMSSaveClick, this);
	NEW_CLASS(this->btnSMSDelete, UI::GUIButton(ui, this->pnlSMS, CSTR("&Delete SMS")));
	this->btnSMSDelete->SetRect(448, 4, 75, 23, false);
	this->btnSMSDelete->HandleButtonClick(OnSMSDeleteClick, this);
	NEW_CLASS(this->btnSMSSaveAll, UI::GUIButton(ui, this->pnlSMS, CSTR("Save All")));
	this->btnSMSSaveAll->SetRect(528, 4, 75, 23, false);
	this->btnSMSSaveAll->HandleButtonClick(OnSMSSaveAllClick, this);
	NEW_CLASS(this->lvSMS, UI::GUIListView(ui, this->tpSMS, UI::GUIListView::LVSTYLE_TABLE, 3));
	this->lvSMS->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvSMS->AddColumn(CSTR("From"), 100);
	this->lvSMS->AddColumn(CSTR("Time"), 180);
	this->lvSMS->AddColumn(CSTR("Content"), 300);
	this->lvSMS->SetFullRowSelect(true);

	this->tpPDPContext = this->tcMain->AddTabPage(CSTR("PDP Context(APN)"));
	NEW_CLASS(this->pnlPDPContext, UI::GUIPanel(ui, this->tpPDPContext));
	this->pnlPDPContext->SetRect(0, 0, 100, 48, false);
	this->pnlPDPContext->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnPDPContextLoad, UI::GUIButton(ui, this->pnlPDPContext, CSTR("Load")));
	this->btnPDPContextLoad->SetRect(0, 0, 75, 23, false);
	this->btnPDPContextLoad->HandleButtonClick(OnPDPContextLoadClicked, this);
	NEW_CLASS(this->lblPDPContextStatus, UI::GUILabel(ui, this->pnlPDPContext, CSTR("Status")));
	this->lblPDPContextStatus->SetRect(80, 0, 100, 23, false);
	NEW_CLASS(this->txtPDPContextStatus, UI::GUITextBox(ui, this->pnlPDPContext, CSTR("")));
	this->txtPDPContextStatus->SetRect(180, 0, 200, 23, false);
	this->txtPDPContextStatus->SetReadOnly(true);
	NEW_CLASS(this->btnPDPAttach, UI::GUIButton(ui, this->pnlPDPContext, CSTR("Attach")));
	this->btnPDPAttach->SetRect(380, 0, 75, 23, false);
	this->btnPDPAttach->HandleButtonClick(OnPDPAttachClicked, this);
	NEW_CLASS(this->btnPDPDetach, UI::GUIButton(ui, this->pnlPDPContext, CSTR("Detach")));
	this->btnPDPDetach->SetRect(460, 0, 75, 23, false);
	this->btnPDPDetach->HandleButtonClick(OnPDPDetachClicked, this);
	NEW_CLASS(this->lblPDPCID, UI::GUILabel(ui, this->pnlPDPContext, CSTR("CID")));
	this->lblPDPCID->SetRect(0, 24, 60, 23, false);
	NEW_CLASS(this->txtPDPCID, UI::GUITextBox(ui, this->pnlPDPContext, CSTR("1")));
	this->txtPDPCID->SetRect(60, 24, 60, 23, false);
	NEW_CLASS(this->lblPDPType, UI::GUILabel(ui, this->pnlPDPContext, CSTR("Type")));
	this->lblPDPType->SetRect(160, 24, 60, 23, false);
	NEW_CLASS(this->txtPDPType, UI::GUITextBox(ui, this->pnlPDPContext, CSTR("IP")));
	this->txtPDPType->SetRect(220, 24, 60, 23, false);
	NEW_CLASS(this->lblPDPAPN, UI::GUILabel(ui, this->pnlPDPContext, CSTR("APN")));
	this->lblPDPAPN->SetRect(320, 24, 60, 23, false);
	NEW_CLASS(this->txtPDPAPN, UI::GUITextBox(ui, this->pnlPDPContext, CSTR("")));
	this->txtPDPAPN->SetRect(380, 24, 160, 23, false);
	NEW_CLASS(this->btnPDPContextSet, UI::GUIButton(ui, this->pnlPDPContext, CSTR("Set")));
	this->btnPDPContextSet->SetRect(540, 24, 75, 23, false);
	this->btnPDPContextSet->HandleButtonClick(OnPDPContextSetClicked, this);

	NEW_CLASS(this->lvPDPContext, UI::GUIListView(ui, this->tpPDPContext, UI::GUIListView::LVSTYLE_TABLE, 4));
	this->lvPDPContext->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvPDPContext->SetFullRowSelect(true);
	this->lvPDPContext->SetShowGrid(true);
	this->lvPDPContext->AddColumn(CSTR("Index(cid)"), 60);
	this->lvPDPContext->AddColumn(CSTR("Type"), 60);
	this->lvPDPContext->AddColumn(CSTR("APN"), 200);
	this->lvPDPContext->AddColumn(CSTR("Activate"), 80);

	this->tpATCommand = this->tcMain->AddTabPage(CSTR("AT Command"));
	NEW_CLASS(this->pnlATCommand, UI::GUIPanel(ui, this->tpATCommand));
	this->pnlATCommand->SetRect(0, 0, 100, 24, false);
	this->pnlATCommand->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblATCommand, UI::GUILabel(ui, this->pnlATCommand, CSTR("AT Command")));
	this->lblATCommand->SetRect(0, 0, 100, 23, false);
	this->lblATCommand->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->btnATCommand, UI::GUIButton(ui, this->pnlATCommand, CSTR("Send")));
	this->btnATCommand->SetRect(0, 0, 75, 23, false);
	this->btnATCommand->SetDockType(UI::GUIControl::DOCK_RIGHT);
	this->btnATCommand->HandleButtonClick(OnATCommandClicked, this);
	NEW_CLASS(this->txtATCommand, UI::GUITextBox(ui, this->pnlATCommand, CSTR("ATZ")));
	this->txtATCommand->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtATCommand->HandleKeyDown(OnATCommandKeyDown, this);
	NEW_CLASS(this->txtATCommandRep, UI::GUITextBox(ui, this->tpATCommand, CSTR(""), true));
	this->txtATCommandRep->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtATCommandRep->SetReadOnly(true);

	this->tpLog = this->tcMain->AddTabPage(CSTR("Log"));
	NEW_CLASS(this->txtLog, UI::GUITextBox(ui, this->tpLog, CSTR("")));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lbLog, UI::GUIListBox(ui, this->tpLog, false));
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSelChg, this);
	
	this->toStop = false;
	this->running = false;
	NEW_CLASS(this->logger, UI::ListBoxLogger(this, this->lbLog, 200, false));
	this->log.AddLogHandler(this->logger, IO::ILogHandler::LogLevel::Raw);

	this->AddTimer(1000, OnTimerTick, this);

	Data::ArrayList<UOSInt> ports;
	Data::ArrayList<IO::SerialPort::SerialPortType> portTypes;
	if (IO::SerialPort::GetAvailablePorts(&ports, &portTypes))
	{
		UTF8Char sbuff[256];
		UTF8Char *sptr;
		UOSInt i = 0;
		UOSInt j = ports.GetCount();
		while (i < j)
		{
			sptr = IO::SerialPort::GetPortName(sbuff, ports.GetItem(i));
			this->cboDeviceSerial->AddItem(CSTRP(sbuff, sptr), (void*)ports.GetItem(i));
			i++;
		}
		if (j > 0)
		{
			this->cboDeviceSerial->SetSelectedIndex(0);
		}
	}

	if (port)
	{
		this->InitStream(port, true);
	}
}

SSWR::AVIRead::AVIRGSMModemForm::~AVIRGSMModemForm()
{
	this->log.RemoveLogHandler(this->logger);
	DEL_CLASS(this->logger);
	this->CloseStream(false);

	SDEL_STRING(this->operName);
	SDEL_STRING(this->initModemManu);
	SDEL_STRING(this->initModemModel);
	SDEL_STRING(this->initModemVer);
	SDEL_STRING(this->initIMEI);
	SDEL_STRING(this->simIMSI);
	SDEL_STRING(this->cfgTECharset);
}

void SSWR::AVIRead::AVIRGSMModemForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
