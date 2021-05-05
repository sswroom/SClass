#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Manage/HiResClock.h"
#include "Net/ConnectionInfo.h"
#include "SSWR/AVIRead/AVIRARPPingForm.h"
#include "Sync/Thread.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRARPPingForm::OnARPHandler(const UInt8 *hwAddr, UInt32 ipAddr, void *userObj)
{
	SSWR::AVIRead::AVIRARPPingForm *me = (SSWR::AVIRead::AVIRARPPingForm *)userObj;
	if (me->requested)
	{
		UInt32 reqIP = ReadNUInt32(me->targetAddr.addr);
		if (reqIP == ipAddr)
		{
			Double t = me->clk->GetTimeDiff() * 1000;
			Text::StringBuilderUTF8 sb;
			me->requested = false;
			sb.ClearStr();
			sb.Append((const UTF8Char*)"Ping time: round trip = ");
			Text::SBAppendF64(&sb, t);
			sb.Append((const UTF8Char*)"ms");
			me->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_COMMAND);
			me->rlcPing->AddSample(&t);
			if (me->reqEvt)
			{
				me->reqEvt->Set();
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRARPPingForm::OnPingClicked(void *userObj)
{
	SSWR::AVIRead::AVIRARPPingForm *me = (SSWR::AVIRead::AVIRARPPingForm*)userObj;
	if (me->arpHdlr && me->targetAddr.addrType != Net::SocketUtil::AT_UNKNOWN)
	{
		SDEL_CLASS(me->arpHdlr);
		me->targetAddr.addrType = Net::SocketUtil::AT_UNKNOWN;
		me->chkRepeat->SetEnabled(true);
		me->txtTarget->SetReadOnly(false);
		me->requested = false;
	}
	else
	{
		Net::SocketUtil::AddressInfo addr;
		Text::StringBuilderUTF8 sb;
		me->txtTarget->GetText(&sb);
		if (!me->sockf->DNSResolveIP(sb.ToString(), &addr))
		{
			UI::MessageDialog::ShowDialog((const UTF8Char *)"Error, target name is not valid", (const UTF8Char *)"Error", me);
			return;
		}
		SSWR::AVIRead::AVIRARPPingForm::AdapterInfo *adapter;
		adapter = (SSWR::AVIRead::AVIRARPPingForm::AdapterInfo*)me->cboAdapter->GetSelectedItem();
		if (adapter == 0)
		{
			UI::MessageDialog::ShowDialog((const UTF8Char *)"Error, no adapter is selected", (const UTF8Char *)"Error", me);
			return;
		}
		NEW_CLASS(me->arpHdlr, Net::ARPHandler(me->core->GetSocketFactory(), adapter->ifName, adapter->hwAddr, adapter->ipAddr, OnARPHandler, me, 1));
		if (me->arpHdlr->IsError())
		{
			DEL_CLASS(me->arpHdlr);
			me->arpHdlr = 0;
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in listening to ARP data", (const UTF8Char*)"ARP Ping", me);
			return;
		}

		me->targetAddr = addr;
		if (me->chkRepeat->IsChecked())
		{
			me->chkRepeat->SetEnabled(false);
			me->txtTarget->SetReadOnly(true);
			me->rlcPing->ClearChart();
		}
		else
		{
			NEW_CLASS(me->reqEvt, Sync::Event(true, (const UTF8Char*)"SSWR.AVIRead.AVIRARPPingForm.reqEvt"));
			me->requested = true;
			me->clk->Start();
			if (me->arpHdlr->MakeRequest(ReadNUInt32(me->targetAddr.addr)))
			{
				me->reqEvt->Wait(1000);
				DEL_CLASS(me->arpHdlr);
				me->arpHdlr = 0;
				if (me->requested)
				{
					me->log->LogMessage((const UTF8Char*)"Ping: no response from target", IO::ILogHandler::LOG_LEVEL_COMMAND);
				}
			}
			else
			{
				DEL_CLASS(me->arpHdlr);
				me->arpHdlr = 0;
				me->log->LogMessage((const UTF8Char*)"Ping: Cannot send request to target", IO::ILogHandler::LOG_LEVEL_COMMAND);
			}
			DEL_CLASS(me->reqEvt);
			me->reqEvt = 0;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRARPPingForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRARPPingForm *me = (SSWR::AVIRead::AVIRARPPingForm*)userObj;
	if (me->arpHdlr && me->targetAddr.addrType != Net::SocketUtil::AT_UNKNOWN)
	{
		Double t;
		if (me->requested)
		{
			t = 1000;
			me->rlcPing->AddSample(&t);
		}
		me->requested = true;
		me->clk->Start();
		me->arpHdlr->MakeRequest(ReadNUInt32(me->targetAddr.addr));
	}
}

SSWR::AVIRead::AVIRARPPingForm::AVIRARPPingForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"ARP Ping");

	this->core = core;
	this->arpHdlr = 0;
	this->reqEvt = 0;
	NEW_CLASS(this->clk, Manage::HiResClock());
	this->sockf = core->GetSocketFactory();
	this->targetAddr.addrType = Net::SocketUtil::AT_UNKNOWN;
	NEW_CLASS(this->log, IO::LogTool());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlRequest, UI::GUIPanel(ui, this));
	this->pnlRequest->SetRect(0, 0, 100, 104, false);
	this->pnlRequest->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->vspRequest, UI::GUIVSplitter(ui, this, 3, false));
	NEW_CLASS(this->lblAdapter, UI::GUILabel(ui, this->pnlRequest, (const UTF8Char*)"Adapter"));
	this->lblAdapter->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->cboAdapter, UI::GUIComboBox(ui, this->pnlRequest, false));
	this->cboAdapter->SetRect(104, 4, 200, 23, false);
	NEW_CLASS(this->lblTarget, UI::GUILabel(ui, this->pnlRequest, (const UTF8Char*)"Target"));
	this->lblTarget->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtTarget, UI::GUITextBox(ui, this->pnlRequest, (const UTF8Char*)""));
	this->txtTarget->SetRect(104, 28, 150, 23, false);
	NEW_CLASS(this->lblRepeat, UI::GUILabel(ui, this->pnlRequest, (const UTF8Char*)"Repeat"));
	this->lblRepeat->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->chkRepeat, UI::GUICheckBox(ui, this->pnlRequest, (const UTF8Char*)"", false));
	this->chkRepeat->SetRect(104, 52, 100, 23, false);
	NEW_CLASS(this->btnPing, UI::GUIButton(ui, this->pnlRequest, (const UTF8Char*)"&Ping"));
	this->btnPing->SetRect(104, 76, 75, 23, false);
	this->btnPing->HandleButtonClick(OnPingClicked, this);
	NEW_CLASS(this->rlcPing, UI::GUIRealtimeLineChart(ui, this, this->core->GetDrawEngine(), 1, 600, 1000));
	this->rlcPing->SetRect(0, 0, 100, 100, false);
	this->rlcPing->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->rlcPing->SetUnit((const UTF8Char*)"ms");
	NEW_CLASS(this->vspPing, UI::GUIVSplitter(ui, this, 3, true));
	NEW_CLASS(this->lbLog, UI::GUIListBox(ui, this, false));
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);

	NEW_CLASS(this->adapters, Data::ArrayList<SSWR::AVIRead::AVIRARPPingForm::AdapterInfo*>());

	Data::ArrayList<Net::ConnectionInfo*> connInfoList;
	Net::ConnectionInfo *connInfo;
	SSWR::AVIRead::AVIRARPPingForm::AdapterInfo *adapter;
	UInt8 hwAddr[32];
	UTF8Char sbuff[128];
	UOSInt i;
	UOSInt j;
	UInt32 ip;
	this->core->GetSocketFactory()->GetConnInfoList(&connInfoList);
	i = 0;
	j = connInfoList.GetCount();
	while (i < j)
	{
		connInfo = connInfoList.GetItem(i);
		if (connInfo->GetConnectionType() != Net::ConnectionInfo::CT_LOOPBACK)
		{
			if (connInfo->GetPhysicalAddress(hwAddr, 32) == 6)
			{
				adapter = MemAlloc(SSWR::AVIRead::AVIRARPPingForm::AdapterInfo, 1);
				ip = connInfo->GetIPAddress(0);
				sbuff[0] = 0;
				connInfo->GetName(sbuff);
				adapter->ifName = Text::StrCopyNew(sbuff);
				adapter->ipAddr = ip;
				MemCopyNO(adapter->hwAddr, hwAddr, 6);
				this->adapters->Add(adapter);
				this->cboAdapter->AddItem(adapter->ifName, adapter);
			}
		}
		DEL_CLASS(connInfo);
		i++;
	}
	if (this->adapters->GetCount() > 0)
	{
		this->cboAdapter->SetSelectedIndex(0);
	}

	NEW_CLASS(this->logger, UI::ListBoxLogger(this, this->lbLog, 500, true));
	this->log->AddLogHandler(this->logger, IO::ILogHandler::LOG_LEVEL_RAW);
	this->SetDefaultButton(this->btnPing);

	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRARPPingForm::~AVIRARPPingForm()
{
	SDEL_CLASS(this->arpHdlr);
	UOSInt i;
	SSWR::AVIRead::AVIRARPPingForm::AdapterInfo *adapter;
	i = this->adapters->GetCount();
	while (i-- > 0)
	{
		adapter = this->adapters->GetItem(i);
		Text::StrDelNew(adapter->ifName);
		MemFree(adapter);
	}
	DEL_CLASS(this->adapters);

	DEL_CLASS(this->clk);
	DEL_CLASS(this->log);
	DEL_CLASS(this->logger);
}

void SSWR::AVIRead::AVIRARPPingForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
