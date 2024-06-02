#include "Stdafx.h"
#include "Manage/HiResClock.h"
#include "SSWR/AVIRead/AVIRDNSClientForm.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRDNSClientForm::OnRequestClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDNSClientForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDNSClientForm>();
	Net::SocketUtil::AddressInfo dnsAddr;
	UInt32 reqIP;
	Text::StringBuilderUTF8 sb;
	Net::DNSClient *dnsCli;
	NN<Net::DNSClient::RequestAnswer> ans;
	Manage::HiResClock *clk;
	UOSInt i;
	UOSInt j;
	UOSInt bestInd;
	me->txtServer->GetText(sb);
	if (!Net::SocketUtil::SetAddrInfo(dnsAddr, sb.ToCString()))
	{
		me->ui->ShowMsgOK(CSTR("Invalid server input"), CSTR("Error"), me);
		return;
	}
	sb.ClearStr();
	me->txtRequest->GetText(sb);
	if (sb.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter request"), CSTR("Error"), me);
		return;
	}
	reqIP = Net::SocketUtil::GetIPAddr(sb.ToCString());

	NEW_CLASS(dnsCli, Net::DNSClient(me->sockf, dnsAddr, me->core->GetLog()));
	NEW_CLASS(clk, Manage::HiResClock());
	me->lbAnswer->ClearItems();
	Net::DNSClient::FreeAnswers(me->ansList);
	Double t;
	if (reqIP == 0)
	{
		UInt16 reqType = (UInt16)me->cboRequest->GetSelectedItem().GetUOSInt();
		clk->Start();
		dnsCli->GetByType(me->ansList, sb.ToCString(), reqType);
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
	j = me->ansList.GetCount();
	while (i < j)
	{
		ans = me->ansList.GetItemNoCheck(i);
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
	sb.AppendDouble(t);
	me->txtRequestTime->SetText(sb.ToCString());
	DEL_CLASS(clk);
	DEL_CLASS(dnsCli);
}

void __stdcall SSWR::AVIRead::AVIRDNSClientForm::OnAnswerSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDNSClientForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDNSClientForm>();
	Net::DNSClient::RequestAnswer *ans = (Net::DNSClient::RequestAnswer*)me->lbAnswer->GetSelectedItem().p;
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	Text::CStringNN cstr;
	if (ans)
	{
		Text::StringBuilderUTF8 sb;
		sb.Append(ans->name);
		me->txtAnsName->SetText(sb.ToCString());
		sptr = Text::StrInt32(sbuff, ans->recType);
		if (Net::DNSClient::TypeGetID(ans->recType).SetTo(cstr))
		{
			sptr = Text::StrConcatC(sptr, UTF8STRC(" ("));
			sptr = cstr.ConcatTo(sptr);
			*sptr++ = ')';
			*sptr = 0;
		}
		me->txtAnsType->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrInt32(sbuff, ans->recClass);
		me->txtAnsClass->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrUInt32(sbuff, ans->ttl);
		me->txtAnsTTL->SetText(CSTRP(sbuff, sptr));
		if (ans->rd)
		{
			sb.ClearStr();
			sb.Append(ans->rd);
			me->txtAnsRD->SetText(sb.ToCString());
		}
		else
		{
			me->txtAnsRD->SetText(CSTR(""));
		}
	}
	else
	{
		me->txtAnsName->SetText(CSTR(""));
		me->txtAnsType->SetText(CSTR(""));
		me->txtAnsClass->SetText(CSTR(""));
		me->txtAnsTTL->SetText(CSTR(""));
		me->txtAnsRD->SetText(CSTR(""));
	}
}

SSWR::AVIRead::AVIRDNSClientForm::AVIRDNSClientForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("DNS Client"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->sockf = core->GetSocketFactory();

	this->pnlRequest = ui->NewPanel(*this);
	this->pnlRequest->SetRect(0, 0, 100, 104, false);
	this->pnlRequest->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblServer = ui->NewLabel(this->pnlRequest, CSTR("DNS Server"));
	this->lblServer->SetRect(4, 4, 120, 23, false);
	this->txtServer = ui->NewTextBox(this->pnlRequest, CSTR(""));
	this->txtServer->SetRect(124, 4, 120, 23, false);
	this->lblRequest = ui->NewLabel(this->pnlRequest, CSTR("Request"));
	this->lblRequest->SetRect(4, 28, 120, 23, false);
	this->txtRequest = ui->NewTextBox(this->pnlRequest, CSTR(""));
	this->txtRequest->SetRect(124, 28, 160, 23, false);
	this->txtRequest->Focus();
	this->cboRequest = ui->NewComboBox(this->pnlRequest, false);
	this->cboRequest->SetRect(124, 52, 200, 23, false);
	this->cboRequest->AddItem(CSTR("1 - A (IPv4)"), (void*)1);
	this->cboRequest->AddItem(CSTR("2 - NS (Name Server)"), (void*)2);
	this->cboRequest->AddItem(CSTR("5 - CNAME (the canonical name for an alias)"), (void*)5);
	this->cboRequest->AddItem(CSTR("6 - SOA (Start of authority)"), (void*)6);
	this->cboRequest->AddItem(CSTR("12 - PTR (Domain from IP)"), (void*)12);
	this->cboRequest->AddItem(CSTR("15 - MX (Mail)"), (void*)15);
	this->cboRequest->AddItem(CSTR("16 - TXT (Text)"), (void*)16);
	this->cboRequest->AddItem(CSTR("28 - AAAA (IPv6)"), (void*)28);
	this->cboRequest->AddItem(CSTR("257 - CAA"), (void*)257);
	this->cboRequest->SetSelectedIndex(0);
	this->btnRequest = ui->NewButton(this->pnlRequest, CSTR("&Request"));
	this->btnRequest->SetRect(324, 52, 75, 23, false);
	this->btnRequest->HandleButtonClick(OnRequestClicked, this);
	this->lblRequestTime = ui->NewLabel(this->pnlRequest, CSTR("Response Time(sec.)"));
	this->lblRequestTime->SetRect(4, 76, 120, 23, false);
	this->txtRequestTime = ui->NewTextBox(this->pnlRequest, CSTR(""));
	this->txtRequestTime->SetRect(124, 76, 150, 23, false);
	this->txtRequestTime->SetReadOnly(true);
	this->lbAnswer = ui->NewListBox(*this, false);
	this->lbAnswer->SetRect(0, 0, 200, 23, false);
	this->lbAnswer->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbAnswer->HandleSelectionChange(OnAnswerSelChg, this);
	this->hspAnswer = ui->NewHSplitter(*this, 3, false);
	this->pnlAnswer = ui->NewPanel(*this);
	this->pnlAnswer->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lblAnsName = ui->NewLabel(this->pnlAnswer, CSTR("Name"));
	this->lblAnsName->SetRect(4, 4, 100, 23, false);
	this->txtAnsName = ui->NewTextBox(this->pnlAnswer, CSTR(""));
	this->txtAnsName->SetRect(104, 4, 200, 23, false);
	this->txtAnsName->SetReadOnly(true);
	this->lblAnsType = ui->NewLabel(this->pnlAnswer, CSTR("Type"));
	this->lblAnsType->SetRect(4, 28, 100, 23, false);
	this->txtAnsType = ui->NewTextBox(this->pnlAnswer, CSTR(""));
	this->txtAnsType->SetRect(104, 28, 100, 23, false);
	this->txtAnsType->SetReadOnly(true);
	this->lblAnsClass = ui->NewLabel(this->pnlAnswer, CSTR("Class"));
	this->lblAnsClass->SetRect(4, 52, 100, 23, false);
	this->txtAnsClass = ui->NewTextBox(this->pnlAnswer, CSTR(""));
	this->txtAnsClass->SetRect(104, 52, 100, 23, false);
	this->txtAnsClass->SetReadOnly(true);
	this->lblAnsTTL = ui->NewLabel(this->pnlAnswer, CSTR("TTL"));
	this->lblAnsTTL->SetRect(4, 76, 100, 23, false);
	this->txtAnsTTL = ui->NewTextBox(this->pnlAnswer, CSTR(""));
	this->txtAnsTTL->SetRect(104, 76, 100, 23, false);
	this->txtAnsTTL->SetReadOnly(true);
	this->lblAnsRD = ui->NewLabel(this->pnlAnswer, CSTR("RD"));
	this->lblAnsRD->SetRect(4, 100, 100, 23, false);
	this->txtAnsRD = ui->NewTextBox(this->pnlAnswer, CSTR(""));
	this->txtAnsRD->SetRect(104, 100, 300, 23, false);
	this->txtAnsRD->SetReadOnly(true);

	this->SetDefaultButton(this->btnRequest);
	Net::SocketUtil::AddressInfo addr;
	this->sockf->GetDefDNS(addr);
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	sptr = Net::SocketUtil::GetAddrName(sbuff, addr).Or(sbuff);
	this->txtServer->SetText(CSTRP(sbuff, sptr));
}

SSWR::AVIRead::AVIRDNSClientForm::~AVIRDNSClientForm()
{
	Net::DNSClient::FreeAnswers(this->ansList);
}

void SSWR::AVIRead::AVIRDNSClientForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
