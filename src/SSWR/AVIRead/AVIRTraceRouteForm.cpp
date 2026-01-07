#include "Stdafx.h"
#include "Manage/HiResClock.h"
#include "Net/ConnectionInfo.h"
#include "Net/TraceRoute.h"
#include "SSWR/AVIRead/AVIRTraceRouteForm.h"

void __stdcall SSWR::AVIRead::AVIRTraceRouteForm::OnStartClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRTraceRouteForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTraceRouteForm>();
	Text::StringBuilderUTF8 sb;
	UInt32 targetIP;
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	me->txtTargetIP->GetText(sb);
	targetIP = me->sockf->DNSResolveIPv4(sb.ToCString());
	if (targetIP == 0)
	{
		me->ui->ShowMsgOK(CSTR("Error in resolving target host"), CSTR("Trace Route"), me);
		return;
	}
	UInt32 ip = (UInt32)me->cboSelfIP->GetSelectedItem().GetOSInt();
	if (ip)
	{
		Data::ArrayList<UInt32> ipList;
		Net::TraceRoute *tracert;
		NEW_CLASS(tracert, Net::TraceRoute(me->sockf, ip));
		if (tracert->IsError())
		{
			me->ui->ShowMsgOK(CSTR("Error in creating ICMP socket"), CSTR("Trace Route"), me);
		}
		else
		{
			if (tracert->Tracev4(targetIP, &ipList))
			{
				me->lbIP->ClearItems();
				i = 0;
				j = ipList.GetCount();
				while (i < j)
				{
					ip = ipList.GetItem(i);
					sptr = Net::SocketUtil::GetIPv4Name(sbuff, ip);
					me->lbIP->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)ip);
					i++;
				}
			}
			else
			{
				me->ui->ShowMsgOK(CSTR("Error in tracing to target"), CSTR("Trace Route"), me);
			}
		}
		DEL_CLASS(tracert);
	}
}

void __stdcall SSWR::AVIRead::AVIRTraceRouteForm::OnIPSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRTraceRouteForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTraceRouteForm>();
	UInt32 ip = (UInt32)(UOSInt)me->lbIP->GetSelectedItem().p;
	if (ip)
	{
		Text::StringBuilderUTF8 sb;
		NN<Net::WhoisRecord> rec = me->whois.RequestIP(ip);
		sb.AppendJoin(rec->Iterator(), CSTR("\r\n"));
		me->txtIPWhois->SetText(sb.ToCString());
	}
	else
	{
		me->txtIPWhois->SetText(CSTR(""));
	}
}


SSWR::AVIRead::AVIRTraceRouteForm::AVIRTraceRouteForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui), whois(core->GetSocketFactory(), 15000)
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(CSTR("Trace Route"));

	this->core = core;
	this->sockf = core->GetSocketFactory();
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlControl = ui->NewPanel(*this);
	this->pnlControl->SetRect(0, 0, 100, 55, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblSelfIP = ui->NewLabel(this->pnlControl, CSTR("Self IP"));
	this->lblSelfIP->SetRect(4, 4, 100, 23, false);
	this->cboSelfIP = ui->NewComboBox(this->pnlControl, false);
	this->cboSelfIP->SetRect(104, 4, 150, 23, false);
	this->lblTargetIP = ui->NewLabel(this->pnlControl, CSTR("Target IP"));
	this->lblTargetIP->SetRect(4, 28, 100, 23, false);
	this->txtTargetIP = ui->NewTextBox(this->pnlControl, CSTR(""));
	this->txtTargetIP->SetRect(104, 28, 150, 23, false);
	this->btnStart = ui->NewButton(this->pnlControl, CSTR("Start"));
	this->btnStart->SetRect(254, 28, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	
	this->lbIP = ui->NewListBox(*this, false);
	this->lbIP->SetRect(0, 0, 150, 23, false);
	this->lbIP->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbIP->HandleSelectionChange(OnIPSelChg, this);
	this->hspIP = ui->NewHSplitter(*this, 3, false);
	this->txtIPWhois = ui->NewTextBox(*this, CSTR(""), true);
	this->txtIPWhois->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtIPWhois->SetReadOnly(true);

	Data::ArrayListNN<Net::ConnectionInfo> connInfoList;
	NN<Net::ConnectionInfo> connInfo;
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UInt32 ip;
	this->sockf->GetConnInfoList(connInfoList);
	i = 0;
	j = connInfoList.GetCount();
	while (i < j)
	{
		connInfo = connInfoList.GetItemNoCheck(i);
		k = 0;
		while (true)
		{
			ip = connInfo->GetIPAddress(k);
			if (ip == 0)
				break;
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, ip);
			this->cboSelfIP->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)ip);
			k++;
		}
		connInfo.Delete();
		i++;
	}
	if (this->cboSelfIP->GetCount() > 0)
	{
		this->cboSelfIP->SetSelectedIndex(0);
	}
}

SSWR::AVIRead::AVIRTraceRouteForm::~AVIRTraceRouteForm()
{
}

void SSWR::AVIRead::AVIRTraceRouteForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
