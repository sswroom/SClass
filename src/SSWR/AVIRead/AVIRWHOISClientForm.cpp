#include "Stdafx.h"
#include "Manage/HiResClock.h"
#include "Net/WhoisGlobalClient.h"
#include "SSWR/AVIRead/AVIRWHOISClientForm.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRWHOISClientForm::OnRequestClicked(void *userObj)
{
	SSWR::AVIRead::AVIRWHOISClientForm *me = (SSWR::AVIRead::AVIRWHOISClientForm*)userObj;
	Text::StringBuilderUTF8 sb;
	UInt32 ip;
	Net::WhoisRecord *rec;
	UTF8Char sbuff[32];
	me->txtIP->GetText(&sb);
	ip = Net::SocketUtil::GetIPAddr(sb.ToString());
	if (ip == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter valid IP", (const UTF8Char*)"Request", me);
		return;
	}

	Net::WhoisGlobalClient cli(me->sockf);
	Manage::HiResClock clk;
	Double t;
	rec = cli.RequestIP(ip);
	t = clk.GetTimeDiff();
	Text::StrDoubleFmt(sbuff, t, "0.0000000000");
	me->txtRespTime->SetText(sbuff);
	me->lbResponse->ClearItems();
	if (rec)
	{
		UOSInt i = 0;
		UOSInt j = rec->GetCount();
		while (i < j)
		{
			me->lbResponse->AddItem(rec->GetItem(i), 0);
			i++;
		}
		DEL_CLASS(rec);
	}
}

SSWR::AVIRead::AVIRWHOISClientForm::AVIRWHOISClientForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"WHOIS Client");

	this->core = core;
	this->sockf = core->GetSocketFactory();
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlRequest, UI::GUIPanel(ui, this));
	this->pnlRequest->SetRect(0, 0, 100, 55, false);
	this->pnlRequest->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblIP, UI::GUILabel(ui, this->pnlRequest, (const UTF8Char*)"IP"));
	this->lblIP->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtIP, UI::GUITextBox(ui, this->pnlRequest, (const UTF8Char*)""));
	this->txtIP->SetRect(104, 4, 400, 23, false);
	NEW_CLASS(this->btnRequest, UI::GUIButton(ui, this->pnlRequest, (const UTF8Char*)"Request"));
	this->btnRequest->SetRect(504, 4, 75, 23, false);
	this->btnRequest->HandleButtonClick(OnRequestClicked, this);
	NEW_CLASS(this->lblRespTime, UI::GUILabel(ui, this->pnlRequest, (const UTF8Char*)"Response Time"));
	this->lblRespTime->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtRespTime, UI::GUITextBox(ui, this->pnlRequest, (const UTF8Char*)""));
	this->txtRespTime->SetRect(104, 28, 400, 23, false);
	this->txtRespTime->SetReadOnly(true);
	NEW_CLASS(this->lbResponse, UI::GUIListBox(ui, this, false));
	this->lbResponse->SetDockType(UI::GUIControl::DOCK_FILL);
	
	this->SetDefaultButton(this->btnRequest);
	this->txtIP->Focus();
}

SSWR::AVIRead::AVIRWHOISClientForm::~AVIRWHOISClientForm()
{
}

void SSWR::AVIRead::AVIRWHOISClientForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
