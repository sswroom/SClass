#include "Stdafx.h"
#include "Manage/HiResClock.h"
#include "Net/WhoisGlobalClient.h"
#include "SSWR/AVIRead/AVIRWHOISClientForm.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

void __stdcall SSWR::AVIRead::AVIRWHOISClientForm::OnRequestClicked(void *userObj)
{
	SSWR::AVIRead::AVIRWHOISClientForm *me = (SSWR::AVIRead::AVIRWHOISClientForm*)userObj;
	Text::StringBuilderUTF8 sb;
	UInt32 ip;
	Net::WhoisRecord *rec;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	me->txtIP->GetText(sb);
	ip = Net::SocketUtil::GetIPAddr(sb.ToCString());
	if (ip == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid IP"), CSTR("Request"), me);
		return;
	}

	Net::WhoisGlobalClient cli(me->sockf);
	Manage::HiResClock clk;
	Double t;
	rec = cli.RequestIP(ip, 15000);
	t = clk.GetTimeDiff();
	sptr = Text::StrDoubleFmt(sbuff, t, "0.0000000000");
	me->txtRespTime->SetText(CSTRP(sbuff, sptr));
	me->lbResponse->ClearItems();
	if (rec)
	{
		UOSInt i = 0;
		UOSInt j = rec->GetCount();
		while (i < j)
		{
			me->lbResponse->AddItem(Text::String::OrEmpty(rec->GetItem(i)), 0);
			i++;
		}
		DEL_CLASS(rec);
	}
}

SSWR::AVIRead::AVIRWHOISClientForm::AVIRWHOISClientForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("WHOIS Client"));

	this->core = core;
	this->sockf = core->GetSocketFactory();
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlRequest = ui->NewPanel(*this);
	this->pnlRequest->SetRect(0, 0, 100, 55, false);
	this->pnlRequest->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblIP = ui->NewLabel(this->pnlRequest, CSTR("IP"));
	this->lblIP->SetRect(4, 4, 100, 23, false);
	this->txtIP = ui->NewTextBox(this->pnlRequest, CSTR(""));
	this->txtIP->SetRect(104, 4, 400, 23, false);
	this->btnRequest = ui->NewButton(this->pnlRequest, CSTR("Request"));
	this->btnRequest->SetRect(504, 4, 75, 23, false);
	this->btnRequest->HandleButtonClick(OnRequestClicked, this);
	this->lblRespTime = ui->NewLabel(this->pnlRequest, CSTR("Response Time"));
	this->lblRespTime->SetRect(4, 28, 100, 23, false);
	this->txtRespTime = ui->NewTextBox(this->pnlRequest, CSTR(""));
	this->txtRespTime->SetRect(104, 28, 400, 23, false);
	this->txtRespTime->SetReadOnly(true);
	this->lbResponse = ui->NewListBox(*this, false);
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
