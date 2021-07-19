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
#include "UI/GUITabPage.h"

UInt32 __stdcall SSWR::AVIRead::AVIRGSMModemForm::ModemThread(void *userObj)
{
	SSWR::AVIRead::AVIRGSMModemForm *me = (SSWR::AVIRead::AVIRGSMModemForm*)userObj;
	Data::DateTime *currTime;
	UTF8Char sbuff[256];
	Bool init = false;
	IO::GSMModemController::BER ber;

	NEW_CLASS(currTime, Data::DateTime());
	me->running = true;

	while (!me->toStop)
	{
		if (!init)
		{
			init = true;
			if (me->modem->GSMGetManufacturer(sbuff))
				me->initModemManu = Text::StrCopyNew(sbuff);
			if (me->modem->GSMGetModelIdent(sbuff))
				me->initModemModel = Text::StrCopyNew(sbuff);
			if (me->modem->GSMGetModemVer(sbuff))
				me->initModemVer = Text::StrCopyNew(sbuff);
			if (me->modem->GSMGetIMEI(sbuff))
				me->initIMEI = Text::StrCopyNew(sbuff);
			me->initStrs = true;
		}

		currTime->SetCurrTimeUTC();
		if (currTime->CompareTo(me->operNextTime) >= 0)
		{
			me->operNextTime->AddSecond(30);
			if (me->modem->GSMGetCurrPLMN(sbuff))
			{
				SDEL_TEXT(me->operName);
				me->operName = Text::StrCopyNew(sbuff);
				me->operUpdated = true;
			}
		}
		me->modem->GSMGetSignalQuality(&me->signalQuality, &ber);
		me->modemEvt->Wait(1000);
	}
	DEL_CLASS(currTime);
	me->running = false;
	return 0;
}

void __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnTimerTick(void *userObj)
{
	UTF8Char sbuff[256];
	SSWR::AVIRead::AVIRGSMModemForm *me = (SSWR::AVIRead::AVIRGSMModemForm*)userObj;

	if (me->initStrs)
	{
		me->initStrs = false;
		if (me->initModemManu)
		{
			me->txtModemManu->SetText(me->initModemManu);
		}
		if (me->initModemModel)
		{
			me->txtModemModel->SetText(me->initModemModel);
		}
		if (me->initModemVer)
		{
			me->txtModemVer->SetText(me->initModemVer);
		}
		if (me->initIMEI)
		{
			me->txtModemIMEI->SetText(me->initIMEI);
		}
	}

	if (me->operUpdated)
	{
		me->operUpdated = false;
		me->txtOperator->SetText(me->operName);
	}

	IO::GSMModemController::GetRSSIString(sbuff, me->signalQuality);
	me->txtSignalQuality->SetText(sbuff);
}

void __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnPhoneRClick(void *userObj)
{
	SSWR::AVIRead::AVIRGSMModemForm *me = (SSWR::AVIRead::AVIRGSMModemForm*)userObj;
	me->LoadPhoneBook();
}

void __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnSMSRClick(void *userObj)
{
	SSWR::AVIRead::AVIRGSMModemForm *me = (SSWR::AVIRead::AVIRGSMModemForm*)userObj;
	me->LoadSMS();
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
		UI::FileDialog *dlg;
		Data::DateTime dt;
		smsMsg = Text::SMSMessage::CreateFromPDU(sms->pduMessage);
		smsMsg->GetMessageTime(&dt);

		NEW_CLASS(dlg, UI::FileDialog(L"sswr", L"AVIRead", L"SMSSave", true));
		dlg->AddFilter((const UTF8Char*)"*.sms", (const UTF8Char*)"SMS File");
		
		sptr = Text::StrConcat(sbuff, (const UTF8Char*)"SMS");
		sptr = Text::StrInt64(sptr, dt.ToDotNetTicks());
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"_");
		sptr = Text::StrInt32(sptr, sms->index);
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"_");
		sptr = Text::StrUTF16_UTF8(sptr, smsMsg->GetAddress());
		sptr = Text::StrConcat(sptr, (const UTF8Char*)".sms");
		dlg->SetFileName(sbuff);

		if (dlg->ShowDialog(me->GetHandle()))
		{
			IO::FileStream *fs;
			Text::UTF8Writer *writer;
			NEW_CLASS(fs, IO::FileStream(dlg->GetFileName(), IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
			NEW_CLASS(writer, Text::UTF8Writer(fs));
			writer->WriteSignature();
			writer->Write((const UTF8Char*)"From: ");
			writer->WriteLineW(smsMsg->GetAddress());
			if (smsMsg->GetSMSC())
			{
				writer->Write((const UTF8Char*)"SMSC: ");
				writer->WriteLineW(smsMsg->GetSMSC());
			}
			writer->Write((const UTF8Char*)"Date: ");
			dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss zzz");
			writer->WriteLine(sbuff);
			writer->WriteLine((const UTF8Char*)"Content: ");
			writer->WriteLineW(smsMsg->GetContent());
			
			DEL_CLASS(writer);
			DEL_CLASS(fs);
		}
		DEL_CLASS(dlg);
		DEL_CLASS(smsMsg);
	}
}

void __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnSMSDeleteClick(void *userObj)
{
	SSWR::AVIRead::AVIRGSMModemForm *me = (SSWR::AVIRead::AVIRGSMModemForm*)userObj;
	IO::GSMModemController::SMSMessage *sms;
	OSInt index = me->lvSMS->GetSelectedIndex();
	if (index >= 0)
	{
		sms = me->msgList->GetItem((UOSInt)index);
		if (me->modem->SMSDeleteMessage(sms->index))
		{
			me->modem->SMSFreeMessage(sms);
			me->lvSMS->RemoveItem((UOSInt)index);
			me->msgList->RemoveAt((UOSInt)index);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGSMModemForm::OnSMSSaveAllClick(void *userObj)
{
	SSWR::AVIRead::AVIRGSMModemForm *me = (SSWR::AVIRead::AVIRGSMModemForm*)userObj;
	IO::GSMModemController::SMSMessage *sms;
	UTF8Char sbuff[128];
	Text::SMSMessage *smsMsg;
	IO::FileStream *fs;
	Text::UTF8Writer *writer;
	if (me->lvSMS->GetCount() > 0)
	{
		UI::FolderDialog *dlg;
		NEW_CLASS(dlg, UI::FolderDialog(L"SSWR", L"AVIRead", L"SMSSaveAll"));
		if (dlg->ShowDialog(me->GetHandle()))
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
				sb.Append(dlg->GetFolder());
				sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
				sb.Append((const UTF8Char*)"SMS");
				sb.AppendI64(dt.ToDotNetTicks());
				sb.Append((const UTF8Char*)"_");
				sb.AppendI32(sms->index);
				sb.Append((const UTF8Char*)"_");
				const UTF8Char *csptr = Text::StrToUTF8New(smsMsg->GetAddress());
				sb.Append(csptr);
				Text::StrDelNew(csptr);
				sb.Append((const UTF8Char*)".sms");

				NEW_CLASS(fs, IO::FileStream(sb.ToString(), IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
				NEW_CLASS(writer, Text::UTF8Writer(fs));
				writer->WriteSignature();
				writer->Write((const UTF8Char*)"From: ");
				writer->WriteLineW(smsMsg->GetAddress());
				if (smsMsg->GetSMSC())
				{
					writer->Write((const UTF8Char*)"SMSC: ");
					writer->WriteLineW(smsMsg->GetSMSC());
				}
				writer->Write((const UTF8Char*)"Date: ");
				dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss zzz");
				writer->WriteLine(sbuff);
				writer->WriteLine((const UTF8Char*)"Content: ");
				writer->WriteLineW(smsMsg->GetContent());
				
				DEL_CLASS(writer);
				DEL_CLASS(fs);
				i++;
			}
		}
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
	this->modem->PBSetStorage(store);
	this->modem->PBReadAllEntries(&phoneList);
	this->lvPhone->ClearItems();
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
}

void SSWR::AVIRead::AVIRGSMModemForm::LoadSMS()
{
	IO::GSMModemController::SMSMessage *sms;
	Text::SMSMessage *smsMsg;
	Data::DateTime dt;
	UTF8Char sbuff[64];
#if _WCHAR_SIZE == 4
	WChar wbuff[64];
#endif
	UOSInt k;
	UOSInt i;
	UOSInt j;
	this->modem->SMSFreeMessages(this->msgList);
	this->lvSMS->ClearItems();
	this->msgList->Clear();

	IO::GSMModemController::SMSStorage store = (IO::GSMModemController::SMSStorage)(OSInt)this->cboSMSStorage->GetItem((UOSInt)this->cboSMSStorage->GetSelectedIndex());

	this->modem->SMSSetStorage(store, IO::GSMModemController::SMSSTORE_SIM, IO::GSMModemController::SMSSTORE_SIM);
	this->modem->SMSGetSMSC(sbuff);
	this->txtSMSC->SetText(sbuff);
	this->modem->SMSListMessages(this->msgList, IO::GSMModemController::SMSS_ALL);
	i = 0;
	j = this->msgList->GetCount();
	while (i < j)
	{
		sms = this->msgList->GetItem(i);
		smsMsg = Text::SMSMessage::CreateFromPDU(sms->pduMessage);
#if _WCHAR_SIZE == 2
		k = this->lvSMS->AddItem(smsMsg->GetAddress(), sms);
#elif _WCHAR_SIZE == 4
		Text::StrUTF16_UTF32(wbuff, smsMsg->GetAddress());
		k = this->lvSMS->AddItem(wbuff, sms);
#endif
		smsMsg->GetMessageTime(&dt);
		dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss zzz");
		this->lvSMS->SetSubItem(k, 1, sbuff);
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

SSWR::AVIRead::AVIRGSMModemForm::AVIRGSMModemForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, IO::GSMModemController *modem, IO::ATCommandChannel *channel, IO::Stream *port) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText((const UTF8Char*)"GSM Modem");
	this->SetFont(0, 8.25, false);
	this->core = core;
	this->modem = modem;
	this->channel = channel;
	this->port = port;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->msgList, Data::ArrayList<IO::GSMModemController::SMSMessage*>());
	this->signalQuality = IO::GSMModemController::RSSI_UNKNOWN;
	this->operUpdated = false;
	this->operName = 0;
	this->initStrs = 0;
	this->initModemManu = 0;
	this->initModemModel = 0;
	this->initModemVer = 0;
	this->initIMEI = 0;

	NEW_CLASS(this->operNextTime, Data::DateTime());
	this->operNextTime->SetCurrTimeUTC();

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpInfo = this->tcMain->AddTabPage((const UTF8Char*)"Info");
	this->tpPhoneBook = this->tcMain->AddTabPage((const UTF8Char*)"PhoneBook");
	this->tpSMS = this->tcMain->AddTabPage((const UTF8Char*)"SMS");
	NEW_CLASS(this->lblModemManu, UI::GUILabel(ui, this->tpInfo, (const UTF8Char*)"Manufacture"));
	this->lblModemManu->SetRect(8, 8, 100, 23, false);
	NEW_CLASS(this->txtModemManu, UI::GUITextBox(ui, this->tpInfo, (const UTF8Char*)""));
	this->txtModemManu->SetRect(108, 8, 100, 23, false);
	this->txtModemManu->SetReadOnly(true);
	NEW_CLASS(this->lblModemModel, UI::GUILabel(ui, this->tpInfo, (const UTF8Char*)"Model"));
	this->lblModemModel->SetRect(8, 32, 100, 23, false);
	NEW_CLASS(this->txtModemModel, UI::GUITextBox(ui, this->tpInfo, (const UTF8Char*)""));
	this->txtModemModel->SetRect(108, 32, 100, 23, false);
	this->txtModemModel->SetReadOnly(true);
	NEW_CLASS(this->lblModemVer, UI::GUILabel(ui, this->tpInfo, (const UTF8Char*)"Modem Ver"));
	this->lblModemVer->SetRect(8, 56, 100, 23, false);
	NEW_CLASS(this->txtModemVer, UI::GUITextBox(ui, this->tpInfo, (const UTF8Char*)""));
	this->txtModemVer->SetRect(108, 56, 100, 23, false);
	this->txtModemVer->SetReadOnly(true);
	NEW_CLASS(this->lblModemIMEI, UI::GUILabel(ui, this->tpInfo, (const UTF8Char*)"IMEI"));
	this->lblModemIMEI->SetRect(8, 80, 100, 23, false);
	NEW_CLASS(this->txtModemIMEI, UI::GUITextBox(ui, this->tpInfo, (const UTF8Char*)""));
	this->txtModemIMEI->SetRect(108, 80, 100, 23, false);
	this->txtModemIMEI->SetReadOnly(true);
	NEW_CLASS(this->lblOperator, UI::GUILabel(ui, this->tpInfo, (const UTF8Char*)"Operator"));
	this->lblOperator->SetRect(8, 104, 100, 23, false);
	NEW_CLASS(this->txtOperator, UI::GUITextBox(ui, this->tpInfo, (const UTF8Char*)""));
	this->txtOperator->SetRect(108, 104, 100, 23, false);
	this->txtOperator->SetReadOnly(true);
	NEW_CLASS(this->lblSignalQuality, UI::GUILabel(ui, this->tpInfo, (const UTF8Char*)"Signal Quality"));
	this->lblSignalQuality->SetRect(8, 128, 100, 23, false);
	NEW_CLASS(this->txtSignalQuality, UI::GUITextBox(ui, this->tpInfo, (const UTF8Char*)""));
	this->txtSignalQuality->SetRect(108, 128, 100, 23, false);
	this->txtSignalQuality->SetReadOnly(true);

	NEW_CLASS(this->pnlPhone, UI::GUIPanel(ui, this->tpPhoneBook));
	this->pnlPhone->SetRect(0, 0, 100, 32, false);
	this->pnlPhone->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->cboPhoneStorage, UI::GUIComboBox(ui, this->pnlPhone, false));
	this->cboPhoneStorage->SetRect(0, 4, 100, 23, false);
	this->cboPhoneStorage->AddItem((const UTF8Char*)"SIM", (void*)(OSInt)IO::GSMModemController::PBSTORE_SIM);
	this->cboPhoneStorage->AddItem((const UTF8Char*)"Restricted Phones", (void*)(OSInt)IO::GSMModemController::PBSTORE_SIM_RESTRICTED);
	this->cboPhoneStorage->AddItem((const UTF8Char*)"Own numbers", (void*)(OSInt)IO::GSMModemController::PBSTORE_SIM_OWN_NUMBERS);
	this->cboPhoneStorage->AddItem((const UTF8Char*)"Emergency", (void*)(OSInt)IO::GSMModemController::PBSTORE_EMERGENCY);
	this->cboPhoneStorage->AddItem((const UTF8Char*)"Last Dialed", (void*)(OSInt)IO::GSMModemController::PBSTORE_LASTNUMDIAL);
	this->cboPhoneStorage->AddItem((const UTF8Char*)"Unanswered", (void*)(OSInt)IO::GSMModemController::PBSTORE_UNANSWERED);
	this->cboPhoneStorage->AddItem((const UTF8Char*)"ME", (void*)(OSInt)IO::GSMModemController::PBSTORE_ME);
	this->cboPhoneStorage->AddItem((const UTF8Char*)"ME+SIM", (void*)(OSInt)IO::GSMModemController::PBSTORE_ME_SIM);
	this->cboPhoneStorage->AddItem((const UTF8Char*)"Received Calls", (void*)(OSInt)IO::GSMModemController::PBSTORE_RECEIVED_CALL);
	this->cboPhoneStorage->AddItem((const UTF8Char*)"Service Dial Numbers", (void*)(OSInt)IO::GSMModemController::PBSTORE_SERVICE_DIALING_NUMBERS);
	this->cboPhoneStorage->SetSelectedIndex(0);
	NEW_CLASS(this->btnPhoneRead, UI::GUIButton(ui, this->pnlPhone, (const UTF8Char*)"&Read"));
	this->btnPhoneRead->SetRect(108, 4, 75, 23, false);
	this->btnPhoneRead->HandleButtonClick(OnPhoneRClick, this);
	NEW_CLASS(this->lvPhone, UI::GUIListView(ui, this->tpPhoneBook, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvPhone->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvPhone->AddColumn((const UTF8Char*)"Name", 200);
	this->lvPhone->AddColumn((const UTF8Char*)"Phone", 200);
	this->lvPhone->SetFullRowSelect(true);
	this->lvPhone->SetShowGrid(true);

	NEW_CLASS(this->pnlSMS, UI::GUIPanel(ui, this->tpSMS));
	this->pnlSMS->SetRect(0, 0, 100, 32, false);
	this->pnlSMS->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->cboSMSStorage, UI::GUIComboBox(ui, this->pnlSMS, false));
	this->cboSMSStorage->SetRect(0, 4, 100, 23, false);
	this->cboSMSStorage->AddItem((const UTF8Char*)"SIM", (void*)(OSInt)IO::GSMModemController::SMSSTORE_SIM);
	this->cboSMSStorage->AddItem((const UTF8Char*)"Flash", (void*)(OSInt)IO::GSMModemController::SMSSTORE_FLASH);
	this->cboSMSStorage->AddItem((const UTF8Char*)"Status Report", (void*)(OSInt)IO::GSMModemController::SMSSTORE_STATUSREPORT);
	this->cboSMSStorage->AddItem((const UTF8Char*)"CBM", (void*)(OSInt)IO::GSMModemController::SMSSTORE_CBMMESSAGE);
	this->cboSMSStorage->SetSelectedIndex(0);
	NEW_CLASS(this->btnSMSRead, UI::GUIButton(ui, this->pnlSMS, (const UTF8Char*)"&Show SMS"));
	this->btnSMSRead->SetRect(108, 4, 75, 23, false);
	this->btnSMSRead->HandleButtonClick(OnSMSRClick, this);
	NEW_CLASS(this->lblSMSC, UI::GUILabel(ui, this->pnlSMS, (const UTF8Char*)"SMSC"));
	this->lblSMSC->SetRect(208, 4, 50, 23, false);
	NEW_CLASS(this->txtSMSC, UI::GUITextBox(ui, this->pnlSMS, (const UTF8Char*)""));
	this->txtSMSC->SetReadOnly(true);
	this->txtSMSC->SetRect(258, 4, 100, 23, false);
	NEW_CLASS(this->btnSMSSave, UI::GUIButton(ui, this->pnlSMS, (const UTF8Char*)"S&ave SMS"));
	this->btnSMSSave->SetRect(368, 4, 75, 23, false);
	this->btnSMSSave->HandleButtonClick(OnSMSSaveClick, this);
	NEW_CLASS(this->btnSMSDelete, UI::GUIButton(ui, this->pnlSMS, (const UTF8Char*)"&Delete SMS"));
	this->btnSMSDelete->SetRect(448, 4, 75, 23, false);
	this->btnSMSDelete->HandleButtonClick(OnSMSDeleteClick, this);
	NEW_CLASS(this->btnSMSSaveAll, UI::GUIButton(ui, this->pnlSMS, (const UTF8Char*)"Save All"));
	this->btnSMSSaveAll->SetRect(528, 4, 75, 23, false);
	this->btnSMSSaveAll->HandleButtonClick(OnSMSSaveAllClick, this);
	NEW_CLASS(this->lvSMS, UI::GUIListView(ui, this->tpSMS, UI::GUIListView::LVSTYLE_TABLE, 3));
	this->lvSMS->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvSMS->AddColumn((const UTF8Char*)"From", 100);
	this->lvSMS->AddColumn((const UTF8Char*)"Time", 180);
	this->lvSMS->AddColumn((const UTF8Char*)"Content", 300);
	this->lvSMS->SetFullRowSelect(true);

	this->AddTimer(1000, OnTimerTick, this);

	NEW_CLASS(this->modemEvt, Sync::Event(true, (const UTF8Char*)"SSWR.AVIRead.AVIRGSMModemForm.modemEvt"));
	this->toStop = false;
	this->running = false;
	Sync::Thread::Create(ModemThread, this);
	while (!this->running)
	{
		Sync::Thread::Sleep(10);
	}
}

SSWR::AVIRead::AVIRGSMModemForm::~AVIRGSMModemForm()
{
	this->toStop = true;
	this->modemEvt->Set();
	this->port->Close();
	this->channel->Close();
	while (this->running)
	{
		Sync::Thread::Sleep(10);
	}

	this->modem->SMSFreeMessages(this->msgList);
	DEL_CLASS(this->msgList);

	DEL_CLASS(this->modem);
	DEL_CLASS(this->channel);
	DEL_CLASS(this->port);

	DEL_CLASS(this->modemEvt);
	DEL_CLASS(this->operNextTime);

	SDEL_TEXT(this->operName);
	SDEL_TEXT(this->initModemManu);
	SDEL_TEXT(this->initModemModel);
	SDEL_TEXT(this->initModemVer);
	SDEL_TEXT(this->initIMEI);
}

void SSWR::AVIRead::AVIRGSMModemForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
