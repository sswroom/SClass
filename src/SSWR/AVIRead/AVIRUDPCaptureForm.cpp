#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRUDPCaptureForm.h"
#include "Sync/MutexUsage.h"
#include "UI/MessageDialog.h"

#define PACKETCOUNT 128

void __stdcall SSWR::AVIRead::AVIRUDPCaptureForm::OnStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRUDPCaptureForm *me = (SSWR::AVIRead::AVIRUDPCaptureForm*)userObj;
	if (me->svr)
	{
		DEL_CLASS(me->svr);
		me->svr = 0;
		me->txtPort->SetReadOnly(false);
		me->lbMulticastCurr->ClearItems();
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		UInt16 port;
		me->txtPort->GetText(&sb);
		if (!sb.ToUInt16(&port))
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter valid port", (const UTF8Char*)"Error", me);
			return;
		}
		if (port <= 0 || port > 65535)
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter valid port", (const UTF8Char*)"Error", me);
			return;
		}
		NEW_CLASS(me->svr, Net::UDPServer(me->core->GetSocketFactory(), 0, port, 0, OnUDPPacket, me, me->log, (const UTF8Char*)"UDP: ", 4, me->chkReuseAddr->IsChecked()));
		if (me->svr->IsError())
		{
			DEL_CLASS(me->svr);
			me->svr = 0;
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in listening to the port", (const UTF8Char*)"Error", me);
			return;
		}
		me->svr->AddMulticastIP(Net::SocketUtil::GetIPAddr(UTF8STRC("239.255.255.250")));
		me->txtPort->SetReadOnly(true);
		me->lbMulticastCurr->ClearItems();
	}
}

void __stdcall SSWR::AVIRead::AVIRUDPCaptureForm::OnLogSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRUDPCaptureForm *me = (SSWR::AVIRead::AVIRUDPCaptureForm*)userObj;
	Text::String *s = me->lbLog->GetSelectedItemTextNew();
	if (s)
	{
		me->txtLog->SetText(s->v);
		s->Release();
	}
}

void __stdcall SSWR::AVIRead::AVIRUDPCaptureForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRUDPCaptureForm *me = (SSWR::AVIRead::AVIRUDPCaptureForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
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
			me->lbData->AddItem({sbuff, (UOSInt)(sptr - sbuff)}, (void*)i);

			if (i == me->packetCurr)
				break;
		}
		
		mutUsage.EndUse();
	}
}

void __stdcall SSWR::AVIRead::AVIRUDPCaptureForm::OnDataSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRUDPCaptureForm *me = (SSWR::AVIRead::AVIRUDPCaptureForm*)userObj;
	UOSInt i;
	i = me->lbData->GetSelectedIndex();
	if (i == INVALID_INDEX)
	{
		me->txtData->SetText((const UTF8Char*)"");
	}
	else
	{
		UTF8Char sbuff[32];
		UTF8Char *sptr;
		Text::StringBuilderUTF8 sb;
		Data::DateTime dt;
		i = (UOSInt)me->lbData->GetItem(i);
		Sync::MutexUsage mutUsage(me->packetMut);
		sb.AppendC(UTF8STRC("Recv Time: "));
		dt.SetTicks(me->packets[i].recvTime);
		dt.ToLocalTime();
		sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
		sb.AppendP(sbuff, sptr);
		sb.AppendC(UTF8STRC("\r\nUDP From: "));
		sptr = Net::SocketUtil::GetAddrName(sbuff, &me->packets[i].addr);
		sb.AppendP(sbuff, sptr);
		sb.AppendC(UTF8STRC(":"));
		sb.AppendU32(me->packets[i].port);
		sb.AppendC(UTF8STRC("\r\nData:\r\n"));
		sb.AppendHexBuff(me->packets[i].buff, me->packets[i].buffSize, ' ', Text::LineBreakType::CRLF);
		mutUsage.EndUse();
		me->txtData->SetText(sb.ToString());
	}
}

void __stdcall SSWR::AVIRead::AVIRUDPCaptureForm::OnPortsDblClk(void *userObj, UOSInt index)
{
	SSWR::AVIRead::AVIRUDPCaptureForm *me = (SSWR::AVIRead::AVIRUDPCaptureForm*)userObj;
	if (me->svr)
		return;
	UTF8Char sbuff[16];
	UInt16 port = (UInt16)(UOSInt)me->lvPorts->GetItem(index);
	if (port != 0)
	{
		Text::StrUInt16(sbuff, port);
		me->txtPort->SetText(sbuff);
	}
}

void __stdcall SSWR::AVIRead::AVIRUDPCaptureForm::OnUDPPacket(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData)
{
	SSWR::AVIRead::AVIRUDPCaptureForm *me = (SSWR::AVIRead::AVIRUDPCaptureForm*)userData;
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	Sync::MutexUsage mutUsage(me->packetMut);
	if (me->packets[me->packetCurr].buff)
	{
		MemFree(me->packets[me->packetCurr].buff);
	}
	me->packets[me->packetCurr].buff = MemAlloc(UInt8, dataSize);
	me->packets[me->packetCurr].buffSize = (UInt32)dataSize;
	MemCopyNO(me->packets[me->packetCurr].buff, buff, dataSize);
	me->packets[me->packetCurr].addr = *addr;
	me->packets[me->packetCurr].port = port;
	me->packets[me->packetCurr].recvTime = dt.ToTicks();
	me->packetCurr = (me->packetCurr + 1) % PACKETCOUNT;
	mutUsage.EndUse();
	me->packetsChg = true;
}

void __stdcall SSWR::AVIRead::AVIRUDPCaptureForm::OnMulticastClicked(void *userObj)
{
	SSWR::AVIRead::AVIRUDPCaptureForm *me = (SSWR::AVIRead::AVIRUDPCaptureForm*)userObj;
	if (me->svr)
	{
		Text::StringBuilderUTF8 sb;
		me->txtMulticastCurr->GetText(&sb);
		UInt32 ip = Net::SocketUtil::GetIPAddr(sb.ToString(), sb.GetLength());
		if (ip != 0)
		{
			me->svr->AddMulticastIP(ip);
			me->lbMulticastCurr->AddItem(sb.ToCString(), 0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRUDPCaptureForm::OnMulticastDoubleClk(void *userObj)
{
	SSWR::AVIRead::AVIRUDPCaptureForm *me = (SSWR::AVIRead::AVIRUDPCaptureForm*)userObj;
	const UTF8Char *ip = (const UTF8Char*)me->lbMulticastCommon->GetSelectedItem();
	if (ip)
	{
		me->txtMulticastCurr->SetText(ip);
	}
}


SSWR::AVIRead::AVIRUDPCaptureForm::AVIRUDPCaptureForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText((const UTF8Char*)"UDP Capture");
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->svr = 0;
	NEW_CLASS(this->log, IO::LogTool());
	NEW_CLASS(this->packetMut, Sync::Mutex());
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

	NEW_CLASS(this->pnlControl, UI::GUIPanel(ui, this));
	this->pnlControl->SetRect(0, 0, 100, 32, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this->pnlControl, (const UTF8Char*)"UDP Port"));
	this->lblPort->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this->pnlControl, CSTR("")));
	this->txtPort->SetRect(104, 4, 60, 23, false);
	NEW_CLASS(this->chkReuseAddr, UI::GUICheckBox(ui, this->pnlControl, (const UTF8Char*)"Reuse Addr", true));
	this->chkReuseAddr->SetRect(164, 4, 80, 23, false);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this->pnlControl, (const UTF8Char*)"Start"));
	this->btnStart->SetRect(244, 4, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpData = this->tcMain->AddTabPage((const UTF8Char*)"Data");
	NEW_CLASS(this->lbData, UI::GUIListBox(ui, this->tpData, false));
	this->lbData->SetRect(0, 0, 200, 100, false);
	this->lbData->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbData->HandleSelectionChange(OnDataSelChg, this);
	NEW_CLASS(this->hspData, UI::GUIHSplitter(ui, this->tpData, 3, false));
	NEW_CLASS(this->txtData, UI::GUITextBox(ui, this->tpData, CSTR(""), true));
	this->txtData->SetReadOnly(true);
	this->txtData->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpLog = this->tcMain->AddTabPage((const UTF8Char*)"Log");
	NEW_CLASS(this->txtLog, UI::GUITextBox(ui, this->tpLog, CSTR("")));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lbLog, UI::GUIListBox(ui, this->tpLog, false));
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSelChg, this);

	this->tpPorts = this->tcMain->AddTabPage((const UTF8Char*)"Common Ports");
	NEW_CLASS(this->lvPorts, UI::GUIListView(ui, this->tpPorts, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvPorts->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvPorts->SetFullRowSelect(true);
	this->lvPorts->SetShowGrid(true);
	this->lvPorts->HandleDblClk(OnPortsDblClk, this);
	this->lvPorts->AddColumn((const UTF8Char*)"Name", 150);
	this->lvPorts->AddColumn((const UTF8Char*)"Port", 100);
	i = this->lvPorts->AddItem((const UTF8Char*)"DNS", (void*)53);
	this->lvPorts->SetSubItem(i, 1, (const UTF8Char*)"53");
	i = this->lvPorts->AddItem((const UTF8Char*)"DHCP Server", (void*)67);
	this->lvPorts->SetSubItem(i, 1, (const UTF8Char*)"67");
	i = this->lvPorts->AddItem((const UTF8Char*)"DHCP Client", (void*)68);
	this->lvPorts->SetSubItem(i, 1, (const UTF8Char*)"68");
	i = this->lvPorts->AddItem((const UTF8Char*)"NTP", (void*)123);
	this->lvPorts->SetSubItem(i, 1, (const UTF8Char*)"123");
	i = this->lvPorts->AddItem((const UTF8Char*)"NetBIOS Name Service", (void*)137);
	this->lvPorts->SetSubItem(i, 1, (const UTF8Char*)"137");
	i = this->lvPorts->AddItem((const UTF8Char*)"NetBIOS Datagram Service", (void*)138);
	this->lvPorts->SetSubItem(i, 1, (const UTF8Char*)"138");
	i = this->lvPorts->AddItem((const UTF8Char*)"NetBIOS Session Service", (void*)139);
	this->lvPorts->SetSubItem(i, 1, (const UTF8Char*)"139");
	i = this->lvPorts->AddItem((const UTF8Char*)"ISAKMP", (void*)500);
	this->lvPorts->SetSubItem(i, 1, (const UTF8Char*)"500");
	i = this->lvPorts->AddItem((const UTF8Char*)"DHCPv6 Server", (void*)546);
	this->lvPorts->SetSubItem(i, 1, (const UTF8Char*)"546");
	i = this->lvPorts->AddItem((const UTF8Char*)"DHCPv6 Client", (void*)547);
	this->lvPorts->SetSubItem(i, 1, (const UTF8Char*)"547");
	i = this->lvPorts->AddItem((const UTF8Char*)"SSDP", (void*)1900);
	this->lvPorts->SetSubItem(i, 1, (const UTF8Char*)"1900");
	i = this->lvPorts->AddItem((const UTF8Char*)"WS-Discovery", (void*)3702);
	this->lvPorts->SetSubItem(i, 1, (const UTF8Char*)"3702");

	this->tpMulticast = this->tcMain->AddTabPage((const UTF8Char*)"Multicast");
	NEW_CLASS(this->pnlMulticastCommon, UI::GUIPanel(ui, this->tpMulticast));
	this->pnlMulticastCommon->SetRect(0, 0, 100, 23, false);
	this->pnlMulticastCommon->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->lblMulticastCommon, UI::GUILabel(ui, this->pnlMulticastCommon, (const UTF8Char*)"Common IP"));
	this->lblMulticastCommon->SetRect(0, 0, 100, 23, false);
	this->lblMulticastCommon->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lbMulticastCommon, UI::GUIListBox(ui, this->pnlMulticastCommon, false));
	this->lbMulticastCommon->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbMulticastCommon->HandleDoubleClicked(OnMulticastDoubleClk, this);
	NEW_CLASS(this->hspMulticast, UI::GUIHSplitter(ui, this->tpMulticast, 3, false));
	NEW_CLASS(this->pnlMulticast, UI::GUIPanel(ui, this->tpMulticast));
	this->pnlMulticast->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->pnlMulticastCtrl, UI::GUIPanel(ui, this->pnlMulticast));
	this->pnlMulticastCtrl->SetRect(0, 0, 100, 23, false);
	this->pnlMulticastCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblMulticastCurr, UI::GUILabel(ui, this->pnlMulticastCtrl, (const UTF8Char*)"Multicast IP"));
	this->lblMulticastCurr->SetRect(0, 0, 100, 23, false);
	NEW_CLASS(this->txtMulticastCurr, UI::GUITextBox(ui, this->pnlMulticastCtrl, CSTR("")));
	this->txtMulticastCurr->SetRect(100, 0, 100, 23, false);
	NEW_CLASS(this->btnMulticastAdd, UI::GUIButton(ui, this->pnlMulticastCtrl, (const UTF8Char*)"&Add"));
	this->btnMulticastAdd->SetRect(200, 0, 75, 23, false);
	this->btnMulticastAdd->HandleButtonClick(OnMulticastClicked, this);
	NEW_CLASS(this->lbMulticastCurr, UI::GUIListBox(ui, this->pnlMulticast, false));
	this->lbMulticastCurr->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbMulticastCommon->AddItem(CSTR("239.255.255.250"), (void*)(const UTF8Char*)"239.255.255.250");

	NEW_CLASS(this->logger, UI::ListBoxLogger(this, this->lbLog, 500, true));
	this->log->AddLogHandler(this->logger, IO::ILogHandler::LOG_LEVEL_RAW);
	
	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRUDPCaptureForm::~AVIRUDPCaptureForm()
{
	OSInt i;
	SDEL_CLASS(this->svr);
	i = PACKETCOUNT;
	while (i-- > 0)
	{
		if (this->packets[i].buff)
		{
			MemFree(this->packets[i].buff);
		}
	}
	MemFree(this->packets);
	DEL_CLASS(this->packetMut);
	DEL_CLASS(this->log);
	DEL_CLASS(this->logger);
}

void SSWR::AVIRead::AVIRUDPCaptureForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
