#include "Stdafx.h"
#include "Manage/HiResClock.h"
#include "Net/ConnectionInfo.h"
#include "Net/TraceRoute.h"
#include "SSWR/AVIRead/AVIRTraceRouteForm.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRTraceRouteForm::OnStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRTraceRouteForm *me = (SSWR::AVIRead::AVIRTraceRouteForm*)userObj;
	Text::StringBuilderUTF8 sb;
	UInt32 targetIP;
	UTF8Char sbuff[32];
	UOSInt i;
	UOSInt j;
	me->txtTargetIP->GetText(&sb);
	targetIP = me->sockf->DNSResolveIPv4(sb.ToString());
	if (targetIP == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in resolving target host", (const UTF8Char*)"Trace Route", me);
		return;
	}
	UInt32 ip = (UInt32)(OSInt)me->cboSelfIP->GetSelectedItem();
	if (ip)
	{
		Data::ArrayList<UInt32> ipList;
		Net::TraceRoute *tracert;
		NEW_CLASS(tracert, Net::TraceRoute(me->sockf, ip));
		if (tracert->IsError())
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in creating ICMP socket", (const UTF8Char*)"Trace Route", me);
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
					Net::SocketUtil::GetIPv4Name(sbuff, ip);
					me->lbIP->AddItem(sbuff, (void*)(OSInt)ip);
					i++;
				}
			}
			else
			{
				UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in tracing to target", (const UTF8Char*)"Trace Route", me);
			}
		}
		DEL_CLASS(tracert);
	}
}

void __stdcall SSWR::AVIRead::AVIRTraceRouteForm::OnIPSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRTraceRouteForm *me = (SSWR::AVIRead::AVIRTraceRouteForm*)userObj;
	UInt32 ip = (UInt32)(UOSInt)me->lbIP->GetSelectedItem();
	if (ip)
	{
		Text::StringBuilderUTF8 sb;
		Net::WhoisRecord *rec = me->whois->RequestIP(ip);
		if (rec)
		{
			UOSInt i = 0;
			UOSInt j = rec->GetCount();
			while (i < j)
			{
				sb.Append(rec->GetItem(i));
				sb.AppendC(UTF8STRC("\r\n"));
				i++;
			}
		}
		me->txtIPWhois->SetText(sb.ToString());
	}
	else
	{
		me->txtIPWhois->SetText((const UTF8Char*)"");
	}
}


SSWR::AVIRead::AVIRTraceRouteForm::AVIRTraceRouteForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText((const UTF8Char*)"Trace Route");

	this->core = core;
	this->sockf = core->GetSocketFactory();
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	NEW_CLASS(this->whois, Net::WhoisHandler(this->sockf));

	NEW_CLASS(this->pnlControl, UI::GUIPanel(ui, this));
	this->pnlControl->SetRect(0, 0, 100, 55, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblSelfIP, UI::GUILabel(ui, this->pnlControl, (const UTF8Char*)"Self IP"));
	this->lblSelfIP->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->cboSelfIP, UI::GUIComboBox(ui, this->pnlControl, false));
	this->cboSelfIP->SetRect(104, 4, 150, 23, false);
	NEW_CLASS(this->lblTargetIP, UI::GUILabel(ui, this->pnlControl, (const UTF8Char*)"Target IP"));
	this->lblTargetIP->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtTargetIP, UI::GUITextBox(ui, this->pnlControl, (const UTF8Char*)""));
	this->txtTargetIP->SetRect(104, 28, 150, 23, false);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this->pnlControl, (const UTF8Char*)"Start"));
	this->btnStart->SetRect(254, 28, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	
	NEW_CLASS(this->lbIP, UI::GUIListBox(ui, this, false));
	this->lbIP->SetRect(0, 0, 150, 23, false);
	this->lbIP->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbIP->HandleSelectionChange(OnIPSelChg, this);
	NEW_CLASS(this->hspIP, UI::GUIHSplitter(ui, this, 3, false));
	NEW_CLASS(this->txtIPWhois, UI::GUITextBox(ui, this, (const UTF8Char*)"", true));
	this->txtIPWhois->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtIPWhois->SetReadOnly(true);

	Data::ArrayList<Net::ConnectionInfo*> connInfoList;
	Net::ConnectionInfo *connInfo;
	UTF8Char sbuff[32];
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UInt32 ip;
	this->sockf->GetConnInfoList(&connInfoList);
	i = 0;
	j = connInfoList.GetCount();
	while (i < j)
	{
		connInfo = connInfoList.GetItem(i);
		k = 0;
		while (true)
		{
			ip = connInfo->GetIPAddress(k);
			if (ip == 0)
				break;
			Net::SocketUtil::GetIPv4Name(sbuff, ip);
			this->cboSelfIP->AddItem(sbuff, (void*)(OSInt)ip);
			k++;
		}
		DEL_CLASS(connInfo);
		i++;
	}
	if (this->cboSelfIP->GetCount() > 0)
	{
		this->cboSelfIP->SetSelectedIndex(0);
	}
}

SSWR::AVIRead::AVIRTraceRouteForm::~AVIRTraceRouteForm()
{
	DEL_CLASS(this->whois);
}

void SSWR::AVIRead::AVIRTraceRouteForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
