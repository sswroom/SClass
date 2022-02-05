#include "Stdafx.h"
#include "Manage/HiResClock.h"
#include "SSWR/AVIRead/AVIRDNSClientForm.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRDNSClientForm::OnRequestClicked(void *userObj)
{
	SSWR::AVIRead::AVIRDNSClientForm *me = (SSWR::AVIRead::AVIRDNSClientForm*)userObj;
	Net::SocketUtil::AddressInfo dnsAddr;
	UInt32 reqIP;
	Text::StringBuilderUTF8 sb;
	Net::DNSClient *dnsCli;
	Net::DNSClient::RequestAnswer *ans;
	Manage::HiResClock *clk;
	UOSInt i;
	UOSInt j;
	UOSInt bestInd;
	me->txtServer->GetText(&sb);
	if (!Net::SocketUtil::GetIPAddr((Char*)sb.ToString(), &dnsAddr))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Invalid server input", (const UTF8Char*)"Error", me);
		return;
	}
	sb.ClearStr();
	me->txtRequest->GetText(&sb);
	if (sb.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter request", (const UTF8Char*)"Error", me);
		return;
	}
	reqIP = Net::SocketUtil::GetIPAddr(sb.ToString(), sb.GetLength());

	NEW_CLASS(dnsCli, Net::DNSClient(me->sockf, &dnsAddr));
	NEW_CLASS(clk, Manage::HiResClock());
	me->lbAnswer->ClearItems();
	Net::DNSClient::FreeAnswers(me->ansList);
	Double t;
	if (reqIP == 0)
	{
		UInt16 reqType = (UInt16)(UOSInt)me->cboRequest->GetSelectedItem();
		clk->Start();
		dnsCli->GetByType(me->ansList, sb.ToString(), sb.GetLength(), reqType);
		t = clk->GetTimeDiff();
	}
	else
	{
		clk->Start();
		dnsCli->GetByIPv4Name(me->ansList, reqIP);
		t = clk->GetTimeDiff();
	}
	bestInd = (UOSInt)-1;
	i = 0;
	j = me->ansList->GetCount();
	while (i < j)
	{
		ans = me->ansList->GetItem(i);
		if (bestInd == (UOSInt)-1 && ans->recType == 1)
		{
			bestInd = i;
		}
		sb.ClearStr();
		sb.Append(ans->name);
		me->lbAnswer->AddItem(sb.ToCString(), ans);
		i++;
	}
	if (j > 0)
	{
		if (bestInd == (UOSInt)-1)
		{
			bestInd = 0;
		}
		me->lbAnswer->SetSelectedIndex(bestInd);
	}
	sb.ClearStr();
	Text::SBAppendF64(&sb, t);
	me->txtRequestTime->SetText(sb.ToString());
	DEL_CLASS(clk);
	DEL_CLASS(dnsCli);
}

void __stdcall SSWR::AVIRead::AVIRDNSClientForm::OnAnswerSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRDNSClientForm *me = (SSWR::AVIRead::AVIRDNSClientForm*)userObj;
	Net::DNSClient::RequestAnswer *ans = (Net::DNSClient::RequestAnswer*)me->lbAnswer->GetSelectedItem();
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	Text::CString cstr;
	if (ans)
	{
		Text::StringBuilderUTF8 sb;
		sb.Append(ans->name);
		me->txtAnsName->SetText(sb.ToString());
		sptr = Text::StrInt32(sbuff, ans->recType);
		cstr = Net::DNSClient::TypeGetID(ans->recType);
		if (cstr.v)
		{
			sptr = Text::StrConcatC(sptr, UTF8STRC(" ("));
			sptr = cstr.ConcatTo(sptr);
			*sptr++ = ')';
			*sptr = 0;
		}
		me->txtAnsType->SetText(sbuff);
		Text::StrInt32(sbuff, ans->recClass);
		me->txtAnsClass->SetText(sbuff);
		Text::StrUInt32(sbuff, ans->ttl);
		me->txtAnsTTL->SetText(sbuff);
		if (ans->rd)
		{
			sb.ClearStr();
			sb.Append(ans->rd);
			me->txtAnsRD->SetText(sb.ToString());
		}
		else
		{
			me->txtAnsRD->SetText((const UTF8Char*)"");
		}
	}
	else
	{
		me->txtAnsName->SetText((const UTF8Char*)"");
		me->txtAnsType->SetText((const UTF8Char*)"");
		me->txtAnsClass->SetText((const UTF8Char*)"");
		me->txtAnsTTL->SetText((const UTF8Char*)"");
		me->txtAnsRD->SetText((const UTF8Char*)"");
	}
}

SSWR::AVIRead::AVIRDNSClientForm::AVIRDNSClientForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText((const UTF8Char*)"DNS Client");

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->sockf = core->GetSocketFactory();
	NEW_CLASS(this->ansList, Data::ArrayList<Net::DNSClient::RequestAnswer*>());

	NEW_CLASS(this->pnlRequest, UI::GUIPanel(ui, this));
	this->pnlRequest->SetRect(0, 0, 100, 104, false);
	this->pnlRequest->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblServer, UI::GUILabel(ui, this->pnlRequest, (const UTF8Char*)"DNS Server"));
	this->lblServer->SetRect(4, 4, 120, 23, false);
	NEW_CLASS(this->txtServer, UI::GUITextBox(ui, this->pnlRequest, CSTR("")));
	this->txtServer->SetRect(124, 4, 120, 23, false);
	NEW_CLASS(this->lblRequest, UI::GUILabel(ui, this->pnlRequest, (const UTF8Char*)"Request"));
	this->lblRequest->SetRect(4, 28, 120, 23, false);
	NEW_CLASS(this->txtRequest, UI::GUITextBox(ui, this->pnlRequest, CSTR("")));
	this->txtRequest->SetRect(124, 28, 160, 23, false);
	this->txtRequest->Focus();
	NEW_CLASS(this->cboRequest, UI::GUIComboBox(ui, this->pnlRequest, false));
	this->cboRequest->SetRect(124, 52, 200, 23, false);
	this->cboRequest->AddItem(CSTR("1 - A (IPv4)"), (void*)1);
	this->cboRequest->AddItem(CSTR("2 - NS (Name Server)"), (void*)2);
	this->cboRequest->AddItem(CSTR("6 - SOA (Start of authority)"), (void*)6);
	this->cboRequest->AddItem(CSTR("12 - PTR (Domain from IP)"), (void*)12);
	this->cboRequest->AddItem(CSTR("15 - MX (Mail)"), (void*)15);
	this->cboRequest->AddItem(CSTR("28 - AAAA (IPv6)"), (void*)28);
	this->cboRequest->SetSelectedIndex(0);
	NEW_CLASS(this->btnRequest, UI::GUIButton(ui, this->pnlRequest, (const UTF8Char*)"&Request"));
	this->btnRequest->SetRect(324, 52, 75, 23, false);
	this->btnRequest->HandleButtonClick(OnRequestClicked, this);
	NEW_CLASS(this->lblRequestTime, UI::GUILabel(ui, this->pnlRequest, (const UTF8Char*)"Response Time(sec.)"));
	this->lblRequestTime->SetRect(4, 76, 120, 23, false);
	NEW_CLASS(this->txtRequestTime, UI::GUITextBox(ui, this->pnlRequest, CSTR("")));
	this->txtRequestTime->SetRect(124, 76, 150, 23, false);
	this->txtRequestTime->SetReadOnly(true);
	NEW_CLASS(this->lbAnswer, UI::GUIListBox(ui, this, false));
	this->lbAnswer->SetRect(0, 0, 200, 23, false);
	this->lbAnswer->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbAnswer->HandleSelectionChange(OnAnswerSelChg, this);
	NEW_CLASS(this->hspAnswer, UI::GUIHSplitter(ui, this, 3, false));
	NEW_CLASS(this->pnlAnswer, UI::GUIPanel(ui, this));
	this->pnlAnswer->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->lblAnsName, UI::GUILabel(ui, this->pnlAnswer, (const UTF8Char*)"Name"));
	this->lblAnsName->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtAnsName, UI::GUITextBox(ui, this->pnlAnswer, CSTR("")));
	this->txtAnsName->SetRect(104, 4, 200, 23, false);
	this->txtAnsName->SetReadOnly(true);
	NEW_CLASS(this->lblAnsType, UI::GUILabel(ui, this->pnlAnswer, (const UTF8Char*)"Type"));
	this->lblAnsType->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtAnsType, UI::GUITextBox(ui, this->pnlAnswer, CSTR("")));
	this->txtAnsType->SetRect(104, 28, 100, 23, false);
	this->txtAnsType->SetReadOnly(true);
	NEW_CLASS(this->lblAnsClass, UI::GUILabel(ui, this->pnlAnswer, (const UTF8Char*)"Class"));
	this->lblAnsClass->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtAnsClass, UI::GUITextBox(ui, this->pnlAnswer, CSTR("")));
	this->txtAnsClass->SetRect(104, 52, 100, 23, false);
	this->txtAnsClass->SetReadOnly(true);
	NEW_CLASS(this->lblAnsTTL, UI::GUILabel(ui, this->pnlAnswer, (const UTF8Char*)"TTL"));
	this->lblAnsTTL->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtAnsTTL, UI::GUITextBox(ui, this->pnlAnswer, CSTR("")));
	this->txtAnsTTL->SetRect(104, 76, 100, 23, false);
	this->txtAnsTTL->SetReadOnly(true);
	NEW_CLASS(this->lblAnsRD, UI::GUILabel(ui, this->pnlAnswer, (const UTF8Char*)"RD"));
	this->lblAnsRD->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtAnsRD, UI::GUITextBox(ui, this->pnlAnswer, CSTR("")));
	this->txtAnsRD->SetRect(104, 100, 300, 23, false);
	this->txtAnsRD->SetReadOnly(true);

	this->SetDefaultButton(this->btnRequest);
	Net::SocketUtil::AddressInfo addr;
	this->sockf->GetDefDNS(&addr);
	UTF8Char sbuff[64];
	Net::SocketUtil::GetAddrName(sbuff, &addr);
	this->txtServer->SetText(sbuff);
}

SSWR::AVIRead::AVIRDNSClientForm::~AVIRDNSClientForm()
{
	Net::DNSClient::FreeAnswers(this->ansList);
	DEL_CLASS(this->ansList);
}

void SSWR::AVIRead::AVIRDNSClientForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
