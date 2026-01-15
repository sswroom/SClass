#include "Stdafx.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRSyslogServerForm.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRSyslogServerForm::OnStartClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSyslogServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSyslogServerForm>();
	NN<Net::SyslogServer> svr;
	if (me->svr.SetTo(svr))
	{
		svr.Delete();
		me->svr = nullptr;
		me->txtPort->SetReadOnly(false);
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		UInt16 port;
		UTF8Char sbuff[512];
		UnsafeArray<UTF8Char> sptr;
		me->txtPort->GetText(sb);
		if (sb.ToUInt16(port))
		{
			sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
			sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("LogSvr"));
			NEW_CLASSNN(svr, Net::SyslogServer(me->core->GetSocketFactory(), port, CSTRP(sbuff, sptr), me->core->GetLog(), false));
			if (svr->IsError())
			{
				me->ui->ShowMsgOK(CSTR("Error in listening the port"), CSTR("Error"), me);
				svr.Delete();
			}
			else
			{
				me->svr = svr;
				svr->HandleClientLog(OnClientLog, me);
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
	me->currIP = (UInt32)(UIntOS)me->lbClient->GetSelectedItem().p;
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

void __stdcall SSWR::AVIRead::AVIRSyslogServerForm::OnClientLog(AnyType userObj, UInt32 ip, Text::CStringNN message)
{
	NN<SSWR::AVIRead::AVIRSyslogServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSyslogServerForm>();
	NN<IPLog> ipLog;
	NN<Text::String> s;
	Sync::MutexUsage mutUsage(me->ipMut);
	if (!me->ipMap.Get(ip).SetTo(ipLog))
	{
		NEW_CLASSNN(ipLog, IPLog());
		ipLog->ip = ip;
		me->ipMap.Put(ip, ipLog);
		me->ipListUpd = true;
	}

	while (ipLog->logMessage.GetCount() >= 100)
	{
		if (ipLog->logMessage.RemoveAt(0).SetTo(s)) s->Release();
	}
	ipLog->logMessage.Add(Text::String::New(message));
	if (me->currIP == ip)
	{
		me->msgListUpd = true;
	}
}

void __stdcall SSWR::AVIRead::AVIRSyslogServerForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSyslogServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSyslogServerForm>();
	UTF8Char sbuff[20];
	UnsafeArray<UTF8Char> sptr;
	UIntOS i;
	UIntOS j;
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
			me->lbClient->AddItem(CSTRP(sbuff, sptr), me->ipMap.GetItemNoCheck(i));
			i++;
		}
	}
	if (me->msgListUpd)
	{
		me->msgListUpd = false;
		NN<IPLog> ipLog;
		Sync::MutexUsage mutUsage(me->ipMut);
		me->lbLog->ClearItems();
		if (me->ipMap.Get(me->currIP).SetTo(ipLog))
		{
			i = ipLog->logMessage.GetCount();
			while (i-- > 0)
			{
				me->lbLog->AddItem(ipLog->logMessage.GetItemNoCheck(i), 0);
			}
		}
		mutUsage.EndUse();
	}
}

SSWR::AVIRead::AVIRSyslogServerForm::AVIRSyslogServerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->SetText(CSTR("Syslog Server"));
	this->SetFont(nullptr, 8.25, false);
	this->svr = nullptr;
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
	this->svr.Delete();

	NN<IPLog> ipLog;
	UIntOS i = this->ipMap.GetCount();
	while (i-- > 0)
	{
		ipLog = this->ipMap.GetItemNoCheck(i);
		ipLog->logMessage.FreeAll();
		ipLog.Delete();
	}
}

void SSWR::AVIRead::AVIRSyslogServerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
