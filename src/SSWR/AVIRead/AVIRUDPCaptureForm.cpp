#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRUDPCaptureForm.h"
#include "Sync/MutexUsage.h"

#define PACKETCOUNT 128

void __stdcall SSWR::AVIRead::AVIRUDPCaptureForm::OnStartClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRUDPCaptureForm> me = userObj.GetNN<SSWR::AVIRead::AVIRUDPCaptureForm>();
	if (me->svr.NotNull())
	{
		me->svr.Delete();
		me->txtPort->SetReadOnly(false);
		me->lbMulticastCurr->ClearItems();
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		UInt16 port;
		me->txtPort->GetText(sb);
		if (!sb.ToUInt16(port))
		{
			me->ui->ShowMsgOK(CSTR("Please enter valid port"), CSTR("Error"), me);
			return;
		}
		if (port <= 0 || port > 65535)
		{
			me->ui->ShowMsgOK(CSTR("Please enter valid port"), CSTR("Error"), me);
			return;
		}
		NN<Net::UDPServer> svr;
		NEW_CLASSNN(svr, Net::UDPServer(me->core->GetSocketFactory(), 0, port, nullptr, OnUDPPacket, me, me->log, CSTR("UDP: "), 4, me->chkReuseAddr->IsChecked()));
		if (svr->IsError())
		{
			svr.Delete();
			me->ui->ShowMsgOK(CSTR("Error in listening to the port"), CSTR("Error"), me);
			return;
		}
		me->svr = svr;
		svr->AddMulticastIP(Net::SocketUtil::GetIPAddr(CSTR("239.255.255.250")));
		me->txtPort->SetReadOnly(true);
		me->lbMulticastCurr->ClearItems();
	}
}

void __stdcall SSWR::AVIRead::AVIRUDPCaptureForm::OnLogSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRUDPCaptureForm> me = userObj.GetNN<SSWR::AVIRead::AVIRUDPCaptureForm>();
	NN<Text::String> s;
	if (me->lbLog->GetSelectedItemTextNew().SetTo(s))
	{
		me->txtLog->SetText(s->ToCString());
		s->Release();
	}
}

void __stdcall SSWR::AVIRead::AVIRUDPCaptureForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRUDPCaptureForm> me = userObj.GetNN<SSWR::AVIRead::AVIRUDPCaptureForm>();
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	if (me->packetsChg)
	{
		Data::DateTime dt;
		OSInt i;
		me->packetsChg = false;
		Sync::MutexUsage mutUsage(me->packetMut);
		me->lbData->ClearItems();
		i = me->packetCurr;
		while (true)
		{
			if (--i < 0)
			{
				i = PACKETCOUNT - 1;
			}
			if (me->packets[i].buff == 0)
			{
				break;
			}
			dt.SetTicks(me->packets[i].recvTime);
			dt.ToLocalTime();
			sptr = dt.ToString(sbuff, "HH:mm:ss.fff");
			me->lbData->AddItem(CSTRP(sbuff, sptr), (void*)i);

			if (i == me->packetCurr)
				break;
		}
		
		mutUsage.EndUse();
	}
}

void __stdcall SSWR::AVIRead::AVIRUDPCaptureForm::OnDataSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRUDPCaptureForm> me = userObj.GetNN<SSWR::AVIRead::AVIRUDPCaptureForm>();
	UOSInt i;
	i = me->lbData->GetSelectedIndex();
	if (i == INVALID_INDEX)
	{
		me->txtData->SetText(CSTR(""));
	}
	else
	{
		UTF8Char sbuff[32];
		UnsafeArray<UTF8Char> sptr;
		Text::StringBuilderUTF8 sb;
		UnsafeArray<UInt8> pbuff;
		Data::DateTime dt;
		i = (UOSInt)me->lbData->GetItem(i).p;
		Sync::MutexUsage mutUsage(me->packetMut);
		sb.AppendC(UTF8STRC("Recv Time: "));
		dt.SetTicks(me->packets[i].recvTime);
		dt.ToLocalTime();
		sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
		sb.AppendP(sbuff, sptr);
		sb.AppendC(UTF8STRC("\r\nUDP From: "));
		sptr = Net::SocketUtil::GetAddrName(sbuff, me->packets[i].addr).Or(sbuff);
		sb.AppendP(sbuff, sptr);
		sb.AppendC(UTF8STRC(":"));
		sb.AppendU32(me->packets[i].port);
		sb.AppendC(UTF8STRC("\r\nData:\r\n"));
		if (me->packets[i].buff.SetTo(pbuff))
		{
			sb.AppendHexBuff(pbuff, me->packets[i].buffSize, ' ', Text::LineBreakType::CRLF);
		}
		mutUsage.EndUse();
		me->txtData->SetText(sb.ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRUDPCaptureForm::OnPortsDblClk(AnyType userObj, UOSInt index)
{
	NN<SSWR::AVIRead::AVIRUDPCaptureForm> me = userObj.GetNN<SSWR::AVIRead::AVIRUDPCaptureForm>();
	if (me->svr.NotNull())
		return;
	UTF8Char sbuff[16];
	UnsafeArray<UTF8Char> sptr;
	UInt16 port = (UInt16)me->lvPorts->GetItem(index).GetUOSInt();
	if (port != 0)
	{
		sptr = Text::StrUInt16(sbuff, port);
		me->txtPort->SetText(CSTRP(sbuff, sptr));
	}
}

void __stdcall SSWR::AVIRead::AVIRUDPCaptureForm::OnUDPPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData)
{
	NN<SSWR::AVIRead::AVIRUDPCaptureForm> me = userData.GetNN<SSWR::AVIRead::AVIRUDPCaptureForm>();
	UnsafeArray<UInt8> pbuff;
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	Sync::MutexUsage mutUsage(me->packetMut);
	if (me->packets[me->packetCurr].buff.SetTo(pbuff))
	{
		MemFreeArr(pbuff);
	}
	me->packets[me->packetCurr].buff = pbuff = MemAllocArr(UInt8, data.GetSize());
	me->packets[me->packetCurr].buffSize = (UInt32)data.GetSize();
	MemCopyNO(pbuff.Ptr(), data.Arr().Ptr(), data.GetSize());
	me->packets[me->packetCurr].addr = addr.Ptr()[0];
	me->packets[me->packetCurr].port = port;
	me->packets[me->packetCurr].recvTime = dt.ToTicks();
	me->packetCurr = (me->packetCurr + 1) % PACKETCOUNT;
	mutUsage.EndUse();
	me->packetsChg = true;
}

void __stdcall SSWR::AVIRead::AVIRUDPCaptureForm::OnMulticastClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRUDPCaptureForm> me = userObj.GetNN<SSWR::AVIRead::AVIRUDPCaptureForm>();
	NN<Net::UDPServer> svr;
	if (me->svr.SetTo(svr))
	{
		Text::StringBuilderUTF8 sb;
		me->txtMulticastCurr->GetText(sb);
		UInt32 ip = Net::SocketUtil::GetIPAddr(sb.ToCString());
		if (ip != 0)
		{
			svr->AddMulticastIP(ip);
			me->lbMulticastCurr->AddItem(sb.ToCString(), 0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRUDPCaptureForm::OnMulticastDoubleClk(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRUDPCaptureForm> me = userObj.GetNN<SSWR::AVIRead::AVIRUDPCaptureForm>();
	const UTF8Char *ip = (const UTF8Char*)me->lbMulticastCommon->GetSelectedItem().p;
	if (ip)
	{
		me->txtMulticastCurr->SetText({ip, Text::StrCharCnt(ip)});
	}
}


SSWR::AVIRead::AVIRUDPCaptureForm::AVIRUDPCaptureForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("UDP Capture"));
	this->SetFont(nullptr, 8.25, false);

	this->core = core;
	this->svr = 0;
	this->packetsChg = false;
	this->packetCurr = 0;
	this->packets = MemAlloc(PacketInfo, PACKETCOUNT);
	UOSInt i;
	i = PACKETCOUNT;
	while (i-- > 0)
	{
		this->packets[i].buff = 0;
		this->packets[i].buffSize = 0;
	}
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlControl = ui->NewPanel(*this);
	this->pnlControl->SetRect(0, 0, 100, 32, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblPort = ui->NewLabel(this->pnlControl, CSTR("UDP Port"));
	this->lblPort->SetRect(4, 4, 100, 23, false);
	this->txtPort = ui->NewTextBox(this->pnlControl, CSTR(""));
	this->txtPort->SetRect(104, 4, 60, 23, false);
	this->chkReuseAddr = ui->NewCheckBox(this->pnlControl, CSTR("Reuse Addr"), true);
	this->chkReuseAddr->SetRect(164, 4, 80, 23, false);
	this->btnStart = ui->NewButton(this->pnlControl, CSTR("Start"));
	this->btnStart->SetRect(244, 4, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpData = this->tcMain->AddTabPage(CSTR("Data"));
	this->lbData = ui->NewListBox(this->tpData, false);
	this->lbData->SetRect(0, 0, 200, 100, false);
	this->lbData->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbData->HandleSelectionChange(OnDataSelChg, this);
	this->hspData = ui->NewHSplitter(this->tpData, 3, false);
	this->txtData = ui->NewTextBox(this->tpData, CSTR(""), true);
	this->txtData->SetReadOnly(true);
	this->txtData->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpLog = this->tcMain->AddTabPage(CSTR("Log"));
	this->txtLog = ui->NewTextBox(this->tpLog, CSTR(""));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lbLog = ui->NewListBox(this->tpLog, false);
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSelChg, this);

	this->tpPorts = this->tcMain->AddTabPage(CSTR("Common Ports"));
	this->lvPorts = ui->NewListView(this->tpPorts, UI::ListViewStyle::Table, 2);
	this->lvPorts->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvPorts->SetFullRowSelect(true);
	this->lvPorts->SetShowGrid(true);
	this->lvPorts->HandleDblClk(OnPortsDblClk, this);
	this->lvPorts->AddColumn(CSTR("Name"), 150);
	this->lvPorts->AddColumn(CSTR("Port"), 100);
	i = this->lvPorts->AddItem(CSTR("DNS"), (void*)53);
	this->lvPorts->SetSubItem(i, 1, CSTR("53"));
	i = this->lvPorts->AddItem(CSTR("DHCP Server"), (void*)67);
	this->lvPorts->SetSubItem(i, 1, CSTR("67"));
	i = this->lvPorts->AddItem(CSTR("DHCP Client"), (void*)68);
	this->lvPorts->SetSubItem(i, 1, CSTR("68"));
	i = this->lvPorts->AddItem(CSTR("NTP"), (void*)123);
	this->lvPorts->SetSubItem(i, 1, CSTR("123"));
	i = this->lvPorts->AddItem(CSTR("NetBIOS Name Service"), (void*)137);
	this->lvPorts->SetSubItem(i, 1, CSTR("137"));
	i = this->lvPorts->AddItem(CSTR("NetBIOS Datagram Service"), (void*)138);
	this->lvPorts->SetSubItem(i, 1, CSTR("138"));
	i = this->lvPorts->AddItem(CSTR("NetBIOS Session Service"), (void*)139);
	this->lvPorts->SetSubItem(i, 1, CSTR("139"));
	i = this->lvPorts->AddItem(CSTR("ISAKMP"), (void*)500);
	this->lvPorts->SetSubItem(i, 1, CSTR("500"));
	i = this->lvPorts->AddItem(CSTR("DHCPv6 Server"), (void*)546);
	this->lvPorts->SetSubItem(i, 1, CSTR("546"));
	i = this->lvPorts->AddItem(CSTR("DHCPv6 Client"), (void*)547);
	this->lvPorts->SetSubItem(i, 1, CSTR("547"));
	i = this->lvPorts->AddItem(CSTR("SSDP"), (void*)1900);
	this->lvPorts->SetSubItem(i, 1, CSTR("1900"));
	i = this->lvPorts->AddItem(CSTR("WS-Discovery"), (void*)3702);
	this->lvPorts->SetSubItem(i, 1, CSTR("3702"));

	this->tpMulticast = this->tcMain->AddTabPage(CSTR("Multicast"));
	this->pnlMulticastCommon = ui->NewPanel(this->tpMulticast);
	this->pnlMulticastCommon->SetRect(0, 0, 100, 23, false);
	this->pnlMulticastCommon->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lblMulticastCommon = ui->NewLabel(this->pnlMulticastCommon, CSTR("Common IP"));
	this->lblMulticastCommon->SetRect(0, 0, 100, 23, false);
	this->lblMulticastCommon->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lbMulticastCommon = ui->NewListBox(this->pnlMulticastCommon, false);
	this->lbMulticastCommon->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbMulticastCommon->HandleDoubleClicked(OnMulticastDoubleClk, this);
	this->hspMulticast = ui->NewHSplitter(this->tpMulticast, 3, false);
	this->pnlMulticast = ui->NewPanel(this->tpMulticast);
	this->pnlMulticast->SetDockType(UI::GUIControl::DOCK_FILL);
	this->pnlMulticastCtrl = ui->NewPanel(this->pnlMulticast);
	this->pnlMulticastCtrl->SetRect(0, 0, 100, 23, false);
	this->pnlMulticastCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblMulticastCurr = ui->NewLabel(this->pnlMulticastCtrl, CSTR("Multicast IP"));
	this->lblMulticastCurr->SetRect(0, 0, 100, 23, false);
	this->txtMulticastCurr = ui->NewTextBox(this->pnlMulticastCtrl, CSTR(""));
	this->txtMulticastCurr->SetRect(100, 0, 100, 23, false);
	this->btnMulticastAdd = ui->NewButton(this->pnlMulticastCtrl, CSTR("&Add"));
	this->btnMulticastAdd->SetRect(200, 0, 75, 23, false);
	this->btnMulticastAdd->HandleButtonClick(OnMulticastClicked, this);
	this->lbMulticastCurr = ui->NewListBox(this->pnlMulticast, false);
	this->lbMulticastCurr->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbMulticastCommon->AddItem(CSTR("239.255.255.250"), (void*)"239.255.255.250");

	NEW_CLASSNN(this->logger, UI::ListBoxLogger(*this, this->lbLog, 500, true));
	this->log.AddLogHandler(this->logger, IO::LogHandler::LogLevel::Raw);
	
	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRUDPCaptureForm::~AVIRUDPCaptureForm()
{
	OSInt i;
	UnsafeArray<UInt8> pbuff;
	this->svr.Delete();
	i = PACKETCOUNT;
	while (i-- > 0)
	{
		if (this->packets[i].buff.SetTo(pbuff))
		{
			MemFreeArr(pbuff);
		}
	}
	MemFreeArr(this->packets);
	this->log.RemoveLogHandler(this->logger);
	this->logger.Delete();
}

void SSWR::AVIRead::AVIRUDPCaptureForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
