#include "Stdafx.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRLogServerForm.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRLogServerForm::OnStartClick(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRLogServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRLogServerForm>();
	if (me->svr)
	{
		DEL_CLASS(me->svr);
		me->svr = 0;
		me->txtPort->SetReadOnly(false);
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		UInt16 port;
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		me->txtPort->GetText(sb);
		if (sb.ToUInt16(port))
		{
			sptr = IO::Path::GetProcessFileName(sbuff);
			sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("LogSvr"));
			NEW_CLASS(me->svr, Net::LogServer(me->core->GetSocketFactory(), port, CSTRP(sbuff, sptr), me->log, false, false));
			if (me->svr->IsError())
			{
				me->ui->ShowMsgOK(CSTR("Error in listening the port"), CSTR("Error"), me);
				DEL_CLASS(me->svr);
				me->svr = 0;
			}
			else
			{
				me->svr->HandleClientLog(OnClientLog, me);
				if (me->svr->Start())
				{
					me->txtPort->SetReadOnly(true);
				}
				else
				{
					me->ui->ShowMsgOK(CSTR("Error in starting LogServer"), CSTR("Error"), me);
					DEL_CLASS(me->svr);
					me->svr = 0;
				}
			}
		}
		else
		{
			me->ui->ShowMsgOK(CSTR("Invalid port number"), CSTR("Error"), me);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRLogServerForm::OnClientSelChg(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRLogServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRLogServerForm>();
	me->currIP = (UInt32)(UOSInt)me->lbClient->GetSelectedItem().p;
	me->lbLog->ClearItems();
	me->msgListUpd = true;
}

void __stdcall SSWR::AVIRead::AVIRLogServerForm::OnLogSelChg(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRLogServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRLogServerForm>();
	NotNullPtr<Text::String> txt = Text::String::OrEmpty(me->lbLog->GetSelectedItemTextNew());
	me->txtLog->SetText(txt->ToCString());
	txt->Release();
}

void __stdcall SSWR::AVIRead::AVIRLogServerForm::OnClientLog(AnyType userObj, UInt32 ip, Text::CString message)
{
	NotNullPtr<SSWR::AVIRead::AVIRLogServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRLogServerForm>();
	IPLog *ipLog;
	Sync::MutexUsage mutUsage(me->ipMut);
	ipLog = me->ipMap.Get(ip);
	if (ipLog == 0)
	{
		NEW_CLASS(ipLog, IPLog());
		ipLog->ip = ip;
		me->ipMap.Put(ip, ipLog);
		me->ipListUpd = true;
	}

	while (ipLog->logMessage.GetCount() >= 100)
	{
		OPTSTR_DEL(ipLog->logMessage.RemoveAt(0));
	}
	ipLog->logMessage.Add(Text::String::New(message));
	if (me->currIP == ip)
	{
		me->msgListUpd = true;
	}
}

void __stdcall SSWR::AVIRead::AVIRLogServerForm::OnTimerTick(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRLogServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRLogServerForm>();
	UTF8Char sbuff[20];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	if (me->ipListUpd)
	{
		me->ipListUpd = false;
		Sync::MutexUsage mutUsage(me->ipMut);
		me->lbClient->ClearItems();
		i = 0;
		j = me->ipMap.GetCount();
		while (i < j)
		{
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, me->ipMap.GetKey(i));
			me->lbClient->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)me->ipMap.GetKey(i));
			i++;
		}
		mutUsage.EndUse();
	}
	if (me->msgListUpd)
	{
		me->msgListUpd = false;
		IPLog *ipLog;
		Sync::MutexUsage mutUsage(me->ipMut);
		me->lbLog->ClearItems();
		ipLog = me->ipMap.Get(me->currIP);
		if (ipLog)
		{
			i = ipLog->logMessage.GetCount();
			while (i-- > 0)
			{
				me->lbLog->AddItem(Text::String::OrEmpty(ipLog->logMessage.GetItem(i)), 0);
			}
		}
		mutUsage.EndUse();
	}
}

SSWR::AVIRead::AVIRLogServerForm::AVIRLogServerForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->SetText(CSTR("Log Server"));
	this->SetFont(0, 0, 8.25, false);
	this->svr = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->currIP = 0;
	this->ipListUpd = false;
	this->msgListUpd = false;

	this->pnlControl = ui->NewPanel(*this);
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblPort = ui->NewLabel(this->pnlControl, CSTR("Port"));
	this->lblPort->SetRect(4, 4, 100, 23, false);
	this->txtPort = ui->NewTextBox(this->pnlControl, CSTR("1234"));
	this->txtPort->SetRect(104, 4, 100, 23, false);
	this->btnStart = ui->NewButton(this->pnlControl, CSTR("Start"));
	this->btnStart->SetRect(204, 4, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClick, this);
	this->lbClient = ui->NewListBox(*this, false);
	this->lbClient->SetRect(0, 0, 150, 23, false);
	this->lbClient->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbClient->HandleSelectionChange(OnClientSelChg, this);
	this->hspClient = ui->NewHSplitter(*this, 3, false);
	this->txtLog = ui->NewTextBox(*this, CSTR(""));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lbLog = ui->NewListBox(*this, false);
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSelChg, this);

	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRLogServerForm::~AVIRLogServerForm()
{
	SDEL_CLASS(this->svr);

	IPLog *ipLog;
	UOSInt i = this->ipMap.GetCount();
	UOSInt j;
	while (i-- > 0)
	{
		ipLog = this->ipMap.GetItem(i);
		j = ipLog->logMessage.GetCount();
		while (j-- > 0)
		{
			OPTSTR_DEL(ipLog->logMessage.GetItem(j));
		}
		DEL_CLASS(ipLog);
	}
}

void SSWR::AVIRead::AVIRLogServerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
