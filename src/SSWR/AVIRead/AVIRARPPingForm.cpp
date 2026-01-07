#include "Stdafx.h"
#include "Core/ByteTool_C.h"
#include "Manage/HiResClock.h"
#include "Net/ConnectionInfo.h"
#include "SSWR/AVIRead/AVIRARPPingForm.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRARPPingForm::OnARPHandler(UnsafeArray<const UInt8> hwAddr, UInt32 ipAddr, AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRARPPingForm> me = userObj.GetNN<SSWR::AVIRead::AVIRARPPingForm>();
	NN<Sync::Event> reqEvt;
	if (me->requested)
	{
		UInt32 reqIP = ReadNUInt32(me->targetAddr.addr);
		if (reqIP == ipAddr)
		{
			Double t = me->clk.GetTimeDiff() * 1000;
			Text::StringBuilderUTF8 sb;
			me->requested = false;
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Ping time: round trip = "));
			sb.AppendDouble(t);
			sb.AppendC(UTF8STRC("ms"));
			me->log.LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Command);
			me->rlcPing->AddSample(&t);
			if (me->reqEvt.SetTo(reqEvt))
			{
				reqEvt->Set();
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRARPPingForm::OnPingClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRARPPingForm> me = userObj.GetNN<SSWR::AVIRead::AVIRARPPingForm>();
	NN<Net::ARPHandler> arpHdlr;
	if (me->arpHdlr.SetTo(arpHdlr) && me->targetAddr.addrType != Net::AddrType::Unknown)
	{
		arpHdlr.Delete();
		me->arpHdlr = 0;
		me->targetAddr.addrType = Net::AddrType::Unknown;
		me->chkRepeat->SetEnabled(true);
		me->txtTarget->SetReadOnly(false);
		me->requested = false;
	}
	else
	{
		Net::SocketUtil::AddressInfo addr;
		Text::StringBuilderUTF8 sb;
		me->txtTarget->GetText(sb);
		if (!me->sockf->DNSResolveIP(sb.ToCString(), addr))
		{
			me->ui->ShowMsgOK(CSTR("Error, target name is not valid"), CSTR("Error"), me);
			return;
		}
		NN<SSWR::AVIRead::AVIRARPPingForm::AdapterInfo> adapter;
		if (!me->cboAdapter->GetSelectedItem().GetOpt<SSWR::AVIRead::AVIRARPPingForm::AdapterInfo>().SetTo(adapter))
		{
			me->ui->ShowMsgOK(CSTR("Error, no adapter is selected"), CSTR("Error"), me);
			return;
		}
		NEW_CLASSNN(arpHdlr, Net::ARPHandler(me->core->GetSocketFactory(), adapter->ifName->v, adapter->hwAddr, adapter->ipAddr, OnARPHandler, me, 1));
		if (arpHdlr->IsError())
		{
			arpHdlr.Delete();
			me->ui->ShowMsgOK(CSTR("Error in listening to ARP data"), CSTR("ARP Ping"), me);
			return;
		}
		me->arpHdlr = arpHdlr;

		me->targetAddr = addr;
		if (me->chkRepeat->IsChecked())
		{
			me->chkRepeat->SetEnabled(false);
			me->txtTarget->SetReadOnly(true);
			me->rlcPing->ClearChart();
		}
		else
		{
			NN<Sync::Event> reqEvt;
			NEW_CLASSNN(reqEvt, Sync::Event(true));
			me->reqEvt = reqEvt;
			me->requested = true;
			me->clk.Start();
			if (arpHdlr->MakeRequest(ReadNUInt32(me->targetAddr.addr)))
			{
				reqEvt->Wait(1000);
				arpHdlr.Delete();
				me->arpHdlr = 0;
				if (me->requested)
				{
					me->log.LogMessage(CSTR("Ping: no response from target"), IO::LogHandler::LogLevel::Command);
				}
			}
			else
			{
				arpHdlr.Delete();
				me->arpHdlr = 0;
				me->log.LogMessage(CSTR("Ping: Cannot send request to target"), IO::LogHandler::LogLevel::Command);
			}
			me->reqEvt = 0;
			reqEvt.Delete();
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRARPPingForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRARPPingForm> me = userObj.GetNN<SSWR::AVIRead::AVIRARPPingForm>();
	NN<Net::ARPHandler> arpHdlr;
	if (me->arpHdlr.SetTo(arpHdlr) && me->targetAddr.addrType != Net::AddrType::Unknown)
	{
		Double t;
		if (me->requested)
		{
			t = 1000;
			me->rlcPing->AddSample(&t);
		}
		me->requested = true;
		me->clk.Start();
		arpHdlr->MakeRequest(ReadNUInt32(me->targetAddr.addr));
	}
}

SSWR::AVIRead::AVIRARPPingForm::AVIRARPPingForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(CSTR("ARP Ping"));

	this->core = core;
	this->arpHdlr = 0;
	this->reqEvt = 0;
	this->sockf = core->GetSocketFactory();
	this->targetAddr.addrType = Net::AddrType::Unknown;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlRequest = ui->NewPanel(*this);
	this->pnlRequest->SetRect(0, 0, 100, 104, false);
	this->pnlRequest->SetDockType(UI::GUIControl::DOCK_TOP);
	this->vspRequest = ui->NewVSplitter(*this, 3, false);
	this->lblAdapter = ui->NewLabel(this->pnlRequest, CSTR("Adapter"));
	this->lblAdapter->SetRect(4, 4, 100, 23, false);
	this->cboAdapter = ui->NewComboBox(this->pnlRequest, false);
	this->cboAdapter->SetRect(104, 4, 200, 23, false);
	this->lblTarget = ui->NewLabel(this->pnlRequest, CSTR("Target"));
	this->lblTarget->SetRect(4, 28, 100, 23, false);
	this->txtTarget = ui->NewTextBox(this->pnlRequest, CSTR(""));
	this->txtTarget->SetRect(104, 28, 150, 23, false);
	this->lblRepeat = ui->NewLabel(this->pnlRequest, CSTR("Repeat"));
	this->lblRepeat->SetRect(4, 52, 100, 23, false);
	this->chkRepeat = ui->NewCheckBox(this->pnlRequest, CSTR(""), false);
	this->chkRepeat->SetRect(104, 52, 100, 23, false);
	this->btnPing = ui->NewButton(this->pnlRequest, CSTR("&Ping"));
	this->btnPing->SetRect(104, 76, 75, 23, false);
	this->btnPing->HandleButtonClick(OnPingClicked, this);
	this->rlcPing = ui->NewRealtimeLineChart(*this, this->core->GetDrawEngine(), 1, 600, 1000, 0);
	this->rlcPing->SetRect(0, 0, 100, 100, false);
	this->rlcPing->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->rlcPing->SetUnit(CSTR("ms"));
	this->vspPing = ui->NewVSplitter(*this, 3, true);
	this->lbLog = ui->NewListBox(*this, false);
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);

	Data::ArrayListNN<Net::ConnectionInfo> connInfoList;
	NN<Net::ConnectionInfo> connInfo;
	NN<SSWR::AVIRead::AVIRARPPingForm::AdapterInfo> adapter;
	UInt8 hwAddr[32];
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	UInt32 ip;
	this->core->GetSocketFactory()->GetConnInfoList(connInfoList);
	i = 0;
	j = connInfoList.GetCount();
	while (i < j)
	{
		connInfo = connInfoList.GetItemNoCheck(i);
		if (connInfo->GetConnectionType() != Net::ConnectionInfo::ConnectionType::Loopback)
		{
			if (connInfo->GetPhysicalAddress(hwAddr, 32) == 6)
			{
				adapter = MemAllocNN(SSWR::AVIRead::AVIRARPPingForm::AdapterInfo);
				ip = connInfo->GetIPAddress(0);
				sbuff[0] = 0;
				sptr = connInfo->GetName(sbuff).Or(sbuff);
				adapter->ifName = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
				adapter->ipAddr = ip;
				MemCopyNO(adapter->hwAddr, hwAddr, 6);
				this->adapters.Add(adapter);
				this->cboAdapter->AddItem(adapter->ifName, adapter);
			}
		}
		connInfo.Delete();
		i++;
	}
	if (this->adapters.GetCount() > 0)
	{
		this->cboAdapter->SetSelectedIndex(0);
	}

	NEW_CLASSNN(this->logger, UI::ListBoxLogger(*this, this->lbLog, 500, true));
	this->log.AddLogHandler(this->logger, IO::LogHandler::LogLevel::Raw);
	this->SetDefaultButton(this->btnPing);

	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRARPPingForm::~AVIRARPPingForm()
{
	this->arpHdlr.Delete();
	UOSInt i;
	NN<SSWR::AVIRead::AVIRARPPingForm::AdapterInfo> adapter;
	i = this->adapters.GetCount();
	while (i-- > 0)
	{
		adapter = this->adapters.GetItemNoCheck(i);
		adapter->ifName->Release();
		MemFreeNN(adapter);
	}
	this->logger.Delete();
}

void SSWR::AVIRead::AVIRARPPingForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
