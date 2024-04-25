#include "Stdafx.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRSyslogServerForm.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRSyslogServerForm::OnStartClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSyslogServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSyslogServerForm>();
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
			NEW_CLASS(me->svr, Net::SyslogServer(me->core->GetSocketFactory(), port, CSTRP(sbuff, sptr), me->core->GetLog(), false));
			if (me->svr->IsError())
			{
				me->ui->ShowMsgOK(CSTR("Error in listening the port"), CSTR("Error"), me);
				DEL_CLASS(me->svr);
				me->svr = 0;
			}
			else
			{
				me->svr->HandleClientLog(OnClientLog, me);
				me->txtPort->SetReadOnly(true);
			}
		}
		else
		{
			me->ui->ShowMsgOK(CSTR("Invalid port number"), CSTR("Error"), me);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRSyslogServerForm::OnClientSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSyslogServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSyslogServerForm>();
	me->currIP = (UInt32)(UOSInt)me->lbClient->GetSelectedItem().p;
	me->lbLog->ClearItems();
	me->msgListUpd = true;
}

void __stdcall SSWR::AVIRead::AVIRSyslogServerForm::OnLogSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSyslogServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSyslogServerForm>();
	NN<Text::String> s = Text::String::OrEmpty(me->lbLog->GetSelectedItemTextNew());
	me->txtLog->SetText(s->ToCString());
	s->Release();
}

void __stdcall SSWR::AVIRead::AVIRSyslogServerForm::OnClientLog(AnyType userObj, UInt32 ip, Text::CString message)
{
	NN<SSWR::AVIRead::AVIRSyslogServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSyslogServerForm>();
	IPLog *ipLog;
	Sync::MutexUsage mutUsage(me->ipMut);
	ipLog = me->ipMap.Get(ip);
	if (ipLog == 0)
	{
		ipLog = MemAlloc(IPLog, 1);
		ipLog->ip = ip;
		NEW_CLASS(ipLog->logMessage, Data::ArrayListStringNN());
		me->ipMap.Put(ip, ipLog);
		me->ipListUpd = true;
	}

	while (ipLog->logMessage->GetCount() >= 100)
	{
		OPTSTR_DEL(ipLog->logMessage->RemoveAt(0));
	}
	ipLog->logMessage->Add(Text::String::New(message));
	if (me->currIP == ip)
	{
		me->msgListUpd = true;
	}
}

void __stdcall SSWR::AVIRead::AVIRSyslogServerForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSyslogServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSyslogServerForm>();
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
			me->lbClient->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)me->ipMap.GetItem(i));
			i++;
		}
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
			i = ipLog->logMessage->GetCount();
			while (i-- > 0)
			{
				me->lbLog->AddItem(Text::String::OrEmpty(ipLog->logMessage->GetItem(i)), 0);
			}
		}
		mutUsage.EndUse();
	}
}

SSWR::AVIRead::AVIRSyslogServerForm::AVIRSyslogServerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->SetText(CSTR("Syslog Server"));
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
	this->txtPort = ui->NewTextBox(this->pnlControl, CSTR("514"));
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

SSWR::AVIRead::AVIRSyslogServerForm::~AVIRSyslogServerForm()
{
	SDEL_CLASS(this->svr);

	IPLog *ipLog;
	UOSInt i = this->ipMap.GetCount();
	UOSInt j;
	while (i-- > 0)
	{
		ipLog = this->ipMap.GetItem(i);
		j = ipLog->logMessage->GetCount();
		while (j-- > 0)
		{
			OPTSTR_DEL(ipLog->logMessage->GetItem(j));
		}
		DEL_CLASS(ipLog->logMessage);
		MemFree(ipLog);
	}
}

void SSWR::AVIRead::AVIRSyslogServerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
