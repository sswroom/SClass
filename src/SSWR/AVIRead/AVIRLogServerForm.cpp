#include "Stdafx.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRLogServerForm.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRLogServerForm::OnStartClick(void *userObj)
{
	SSWR::AVIRead::AVIRLogServerForm *me = (SSWR::AVIRead::AVIRLogServerForm*)userObj;
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
		me->txtPort->GetText(&sb);
		if (sb.ToUInt16(&port))
		{
			sptr = IO::Path::GetProcessFileName(sbuff);
			IO::Path::AppendPathC(sbuff, sptr, UTF8STRC("LogSvr"));
			NEW_CLASS(me->svr, Net::LogServer(me->core->GetSocketFactory(), port, sbuff, 0, false));
			if (me->svr->IsError())
			{
				UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in listening the port", (const UTF8Char*)"Error", me);
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
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Invalid port number", (const UTF8Char*)"Error", me);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRLogServerForm::OnClientSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRLogServerForm *me = (SSWR::AVIRead::AVIRLogServerForm*)userObj;
	me->currIP = (UInt32)(UOSInt)me->lbClient->GetSelectedItem();
	me->lbLog->ClearItems();
	me->msgListUpd = true;
}

void __stdcall SSWR::AVIRead::AVIRLogServerForm::OnLogSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRLogServerForm *me = (SSWR::AVIRead::AVIRLogServerForm*)userObj;
	Text::String *txt = Text::String::OrEmpty(me->lbLog->GetSelectedItemTextNew());
	me->txtLog->SetText(txt->v);
	txt->Release();
}

void __stdcall SSWR::AVIRead::AVIRLogServerForm::OnClientLog(void *userObj, UInt32 ip, const UTF8Char *message)
{
	SSWR::AVIRead::AVIRLogServerForm *me = (SSWR::AVIRead::AVIRLogServerForm*)userObj;
	IPLog *ipLog;
	Sync::MutexUsage mutUsage(me->ipMut);
	ipLog = me->ipMap->Get(ip);
	if (ipLog == 0)
	{
		ipLog = MemAlloc(IPLog, 1);
		ipLog->ip = ip;
		NEW_CLASS(ipLog->logMessage, Data::ArrayList<Text::String*>());
		me->ipMap->Put(ip, ipLog);
		me->ipListUpd = true;
	}

	while (ipLog->logMessage->GetCount() >= 100)
	{
		ipLog->logMessage->RemoveAt(0)->Release();
	}
	ipLog->logMessage->Add(Text::String::NewNotNull(message));
	if (me->currIP == ip)
	{
		me->msgListUpd = true;
	}
	mutUsage.EndUse();
}

void __stdcall SSWR::AVIRead::AVIRLogServerForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRLogServerForm *me = (SSWR::AVIRead::AVIRLogServerForm*)userObj;
	UTF8Char sbuff[20];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	if (me->ipListUpd)
	{
		me->ipListUpd = false;
		Data::ArrayList<UInt32> *ipList;
		Sync::MutexUsage mutUsage(me->ipMut);
		ipList = me->ipMap->GetKeys();
		me->lbClient->ClearItems();
		i = 0;
		j = ipList->GetCount();
		while (i < j)
		{
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, ipList->GetItem(i));
			me->lbClient->AddItem({sbuff, (UOSInt)(sptr - sbuff)}, (void*)(OSInt)ipList->GetItem(i));
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
		ipLog = me->ipMap->Get(me->currIP);
		if (ipLog)
		{
			i = ipLog->logMessage->GetCount();
			while (i-- > 0)
			{
				me->lbLog->AddItem(ipLog->logMessage->GetItem(i), 0);
			}
		}
		mutUsage.EndUse();
	}
}

SSWR::AVIRead::AVIRLogServerForm::AVIRLogServerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->SetText((const UTF8Char*)"Log Server");
	this->SetFont(0, 0, 8.25, false);
	this->svr = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->currIP = 0;
	this->ipListUpd = false;
	this->msgListUpd = false;
	NEW_CLASS(this->ipMut, Sync::Mutex());
	NEW_CLASS(this->ipMap, Data::UInt32Map<IPLog*>());

	NEW_CLASS(this->pnlControl, UI::GUIPanel(ui, this));
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this->pnlControl, (const UTF8Char*)"Port"));
	this->lblPort->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this->pnlControl, CSTR("1234")));
	this->txtPort->SetRect(104, 4, 100, 23, false);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this->pnlControl, (const UTF8Char*)"Start"));
	this->btnStart->SetRect(204, 4, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClick, this);
	NEW_CLASS(this->lbClient, UI::GUIListBox(ui, this, false));
	this->lbClient->SetRect(0, 0, 150, 23, false);
	this->lbClient->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbClient->HandleSelectionChange(OnClientSelChg, this);
	NEW_CLASS(this->hspClient, UI::GUIHSplitter(ui, this, 3, false));
	NEW_CLASS(this->txtLog, UI::GUITextBox(ui, this, CSTR("")));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lbLog, UI::GUIListBox(ui, this, false));
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSelChg, this);

	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRLogServerForm::~AVIRLogServerForm()
{
	SDEL_CLASS(this->svr);

	Data::ArrayList<IPLog*> *ipList = this->ipMap->GetValues();
	IPLog *ipLog;
	UOSInt i = ipList->GetCount();
	UOSInt j;
	while (i-- > 0)
	{
		ipLog = ipList->GetItem(i);
		j = ipLog->logMessage->GetCount();
		while (j-- > 0)
		{
			ipLog->logMessage->GetItem(j)->Release();
		}
		DEL_CLASS(ipLog->logMessage);
		MemFree(ipLog);
	}
	DEL_CLASS(this->ipMap);
	DEL_CLASS(this->ipMut);
}

void SSWR::AVIRead::AVIRLogServerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
