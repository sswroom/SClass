#include "Stdafx.h"
#include "Manage/HiResClock.h"
#include "Net/WhoisGlobalClient.h"
#include "SSWR/AVIRead/AVIRWHOISClientForm.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

void __stdcall SSWR::AVIRead::AVIRWHOISClientForm::OnRequestClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRWHOISClientForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWHOISClientForm>();
	Text::StringBuilderUTF8 sb;
	UInt32 ip;
	NN<Net::WhoisRecord> rec;
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
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
	UIntOS i = 0;
	UIntOS j = rec->GetCount();
	while (i < j)
	{
		me->lbResponse->AddItem(Text::String::OrEmpty(rec->GetItem(i)), 0);
		i++;
	}
	rec.Delete();
}

SSWR::AVIRead::AVIRWHOISClientForm::AVIRWHOISClientForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(nullptr, 8.25, false);
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
