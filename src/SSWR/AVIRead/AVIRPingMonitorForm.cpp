#include "Stdafx.h"
#include "Manage/HiResClock.h"
#include "Net/ConnectionInfo.h"
#include "SSWR/AVIRead/AVIRPingMonitorForm.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRPingMonitorForm::OnPingPacket(void *userData, UInt32 srcIP, UInt32 destIP, UInt8 ttl, UOSInt packetSize)
{
	SSWR::AVIRead::AVIRPingMonitorForm *me = (SSWR::AVIRead::AVIRPingMonitorForm*)userData;
	Text::StringBuilderUTF8 sb;
	UInt32 sortableIP = Net::SocketUtil::IPv4ToSortable(srcIP);
	IPInfo *ipInfo;
	UTF8Char sbuff[256];
	me->ipMut->Lock();
	ipInfo = me->ipMap->Get(sortableIP);
	if (ipInfo == 0)
	{
		Net::WhoisRecord *rec;
		ipInfo = MemAlloc(IPInfo, 1);
		ipInfo->ip = srcIP;
		ipInfo->count = 0;
		rec = me->whois->RequestIP(srcIP);
		if (rec->GetNetworkName(sbuff))
		{
			ipInfo->name = Text::StrCopyNew(sbuff);
		}
		else
		{
			ipInfo->name = Text::StrCopyNew((const UTF8Char*)"Unknown");
		}
		if (rec->GetCountryCode(sbuff))
		{
			ipInfo->country = Text::StrCopyNew(sbuff);
		}
		else
		{
			ipInfo->country = Text::StrCopyNew((const UTF8Char*)"Unk");
		}
		me->ipMap->Put(sortableIP, ipInfo);
		me->ipListUpdated = true;
	}
	ipInfo->count++;
	if (me->currIP == ipInfo)
	{
		me->ipContUpdated = true;
	}
	me->ipMut->Unlock();

	sb.Append((const UTF8Char *)"Received from ");
	Net::SocketUtil::GetIPv4Name(sbuff, srcIP);
	sb.Append(sbuff);
	sb.Append((const UTF8Char *)" to ");
	Net::SocketUtil::GetIPv4Name(sbuff, destIP);
	sb.Append(sbuff);
	sb.Append((const UTF8Char *)", size = ");
	sb.AppendOSInt(packetSize);
	sb.Append((const UTF8Char *)", ttl = ");
	sb.AppendU16(ttl);
	me->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_COMMAND);
}

void __stdcall SSWR::AVIRead::AVIRPingMonitorForm::OnRAWData(void *userData, const UInt8 *rawData, UOSInt packetSize)
{
	SSWR::AVIRead::AVIRPingMonitorForm *me = (SSWR::AVIRead::AVIRPingMonitorForm*)userData;
	me->analyzer->PacketIPv4(rawData, packetSize, 0, 0);
}

void __stdcall SSWR::AVIRead::AVIRPingMonitorForm::OnInfoClicked(void *userObj)
{
	SSWR::AVIRead::AVIRPingMonitorForm *me = (SSWR::AVIRead::AVIRPingMonitorForm*)userObj;
	if (me->listener)
	{
		DEL_CLASS(me->listener);
		DEL_CLASS(me->webHdlr);
		me->listener = 0;
		me->webHdlr = 0;
		me->txtInfo->SetReadOnly(false);
		return;
	}
	UInt16 port;
	Text::StringBuilderUTF8 sb;
	me->txtInfo->GetText(&sb);
	if (!sb.ToUInt16(&port))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Info port is not valid", (const UTF8Char*)"Ping Monitor", me);
		return;
	}
	NEW_CLASS(me->webHdlr, Net::EthernetWebHandler(me->analyzer));
	NEW_CLASS(me->listener, Net::WebServer::WebListener(me->sockf, me->webHdlr, port, 60, 3, (const UTF8Char*)"PingMonitor/1.0", false, true));
	if (me->listener->IsError())
	{
		DEL_CLASS(me->listener);
		DEL_CLASS(me->webHdlr);
		me->listener = 0;
		me->webHdlr = 0;
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in listening to info port", (const UTF8Char*)"Ping Monitor", me);
		return;
	}
	me->txtInfo->SetReadOnly(true);
}

void __stdcall SSWR::AVIRead::AVIRPingMonitorForm::OnStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRPingMonitorForm *me = (SSWR::AVIRead::AVIRPingMonitorForm*)userObj;
	if (me->socMon)
	{
		DEL_CLASS(me->socMon);
		me->socMon = 0;
		me->cboIP->SetEnabled(true);
		return;
	}

	UInt32 ip = (UInt32)(OSInt)me->cboIP->GetSelectedItem();
	if (ip)
	{
		UInt32 *soc = me->sockf->CreateICMPIPv4Socket(ip);
		if (soc)
		{
			NEW_CLASS(me->socMon, Net::SocketMonitor(me->sockf, soc, OnRAWData, me, 3));
			me->cboIP->SetEnabled(false);
		}
		else
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in listening to ping", (const UTF8Char*)"Error", me);
			return;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRPingMonitorForm::OnLogSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRPingMonitorForm *me = (SSWR::AVIRead::AVIRPingMonitorForm*)userObj;
	const UTF8Char *csptr = me->lbLog->GetSelectedItemTextNew();
	if (csptr)
	{
		me->txtLog->SetText(csptr);
		me->lbLog->DelTextNew(csptr);
	}
}

void __stdcall SSWR::AVIRead::AVIRPingMonitorForm::OnIPSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRPingMonitorForm *me = (SSWR::AVIRead::AVIRPingMonitorForm*)userObj;
	me->currIP = (IPInfo*)me->lbIP->GetSelectedItem();
	me->ipContUpdated = false;
	if (me->currIP)
	{
		UTF8Char sbuff[32];
		Text::StrInt64(sbuff, me->currIP->count);
		me->txtIPCount->SetText(sbuff);
		me->txtIPName->SetText(me->currIP->name);
		me->txtIPCountry->SetText(me->currIP->country);

		Text::StringBuilderUTF8 sb;
		Net::WhoisRecord *rec = me->whois->RequestIP(me->currIP->ip);
		if (rec)
		{
			OSInt i = 0;
			OSInt j = rec->GetCount();
			while (i < j)
			{
				sb.Append(rec->GetItem(i));
				sb.Append((const UTF8Char*)"\r\n");
				i++;
			}
		}
		me->txtIPWhois->SetText(sb.ToString());
	}
	else
	{
		me->txtIPCount->SetText((const UTF8Char*)"");
		me->txtIPName->SetText((const UTF8Char*)"");
		me->txtIPCountry->SetText((const UTF8Char*)"");
		me->txtIPWhois->SetText((const UTF8Char*)"");
	}
}

void __stdcall SSWR::AVIRead::AVIRPingMonitorForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRPingMonitorForm *me = (SSWR::AVIRead::AVIRPingMonitorForm*)userObj;
	UTF8Char sbuff[32];
	if (me->ipContUpdated)
	{
		me->ipContUpdated = false;
		Text::StrInt64(sbuff, me->currIP->count);
		me->txtIPCount->SetText(sbuff);
	}
	if (me->ipListUpdated)
	{
		Data::ArrayList<IPInfo*> *ipList;
		IPInfo *ipInfo;
		OSInt i;
		OSInt j;
		me->ipListUpdated = false;
		me->ipMut->Lock();
		me->lbIP->ClearItems();
		ipList = me->ipMap->GetValues();
		i = 0;
		j = ipList->GetCount();
		while (i < j)
		{
			ipInfo = ipList->GetItem(i);
			Net::SocketUtil::GetIPv4Name(sbuff, ipInfo->ip);
			me->lbIP->AddItem(sbuff, ipInfo);
			if (ipInfo == me->currIP)
			{
				me->lbIP->SetSelectedIndex(i);
			}
			i++;
		}
		me->ipMut->Unlock();
	}
}

SSWR::AVIRead::AVIRPingMonitorForm::AVIRPingMonitorForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"Ping Monitor");

	this->core = core;
	this->sockf = core->GetSocketFactory();
	NEW_CLASS(this->log, IO::LogTool());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->listener = 0;
	this->webHdlr = 0;
	NEW_CLASS(this->ipMut, Sync::Mutex());
	NEW_CLASS(this->ipMap, Data::Integer32Map<IPInfo*>());
	NEW_CLASS(this->whois, Net::WhoisHandler(this->sockf));
	NEW_CLASS(this->analyzer, Net::EthernetAnalyzer(0, Net::EthernetAnalyzer::AT_ICMP, (const UTF8Char*)"PingMonitor"));
	this->ipListUpdated = false;
	this->ipContUpdated = false;
	this->currIP = 0;
	this->analyzer->HandlePingv4Request(OnPingPacket, this);

	NEW_CLASS(this->pnlControl, UI::GUIPanel(ui, this));
	this->pnlControl->SetRect(0, 0, 100, 55, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblInfo, UI::GUILabel(ui, this->pnlControl, (const UTF8Char*)"Info Port"));
	this->lblInfo->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtInfo, UI::GUITextBox(ui, this->pnlControl, (const UTF8Char*)"8089"));
	this->txtInfo->SetRect(104, 4, 80, 23, false);
	NEW_CLASS(this->btnInfo, UI::GUIButton(ui, this->pnlControl, (const UTF8Char*)"Start"));
	this->btnInfo->SetRect(184, 4, 75, 23, false);
	this->btnInfo->HandleButtonClick(OnInfoClicked, this);
	NEW_CLASS(this->lblIP, UI::GUILabel(ui, this->pnlControl, (const UTF8Char*)"IP"));
	this->lblIP->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->cboIP, UI::GUIComboBox(ui, this->pnlControl, false));
	this->cboIP->SetRect(104, 28, 150, 23, false);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this->pnlControl, (const UTF8Char*)"Start"));
	this->btnStart->SetRect(254, 28, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	
	this->tpIP = this->tcMain->AddTabPage((const UTF8Char*)"IP");
	NEW_CLASS(this->lbIP, UI::GUIListBox(ui, this->tpIP, false));
	this->lbIP->SetRect(0, 0, 150, 23, false);
	this->lbIP->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbIP->HandleSelectionChange(OnIPSelChg, this);
	NEW_CLASS(this->hspIP, UI::GUIHSplitter(ui, this->tpIP, 3, false));
	NEW_CLASS(this->tcIP, UI::GUITabControl(ui, this->tpIP));
	this->tcIP->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpIPInfo = this->tcIP->AddTabPage((const UTF8Char*)"Info");
	NEW_CLASS(this->lblIPCount, UI::GUILabel(ui, this->tpIPInfo, (const UTF8Char*)"Count"));
	this->lblIPCount->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtIPCount, UI::GUITextBox(ui, this->tpIPInfo, (const UTF8Char*)"0"));
	this->txtIPCount->SetRect(104, 4, 100, 23, false);
	this->txtIPCount->SetReadOnly(true);
	NEW_CLASS(this->lblIPName, UI::GUILabel(ui, this->tpIPInfo, (const UTF8Char*)"Name"));
	this->lblIPName->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtIPName, UI::GUITextBox(ui, this->tpIPInfo, (const UTF8Char*)""));
	this->txtIPName->SetRect(104, 28, 200, 23, false);
	this->txtIPName->SetReadOnly(true);
	NEW_CLASS(this->lblIPCountry, UI::GUILabel(ui, this->tpIPInfo, (const UTF8Char*)"Country"));
	this->lblIPCountry->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtIPCountry, UI::GUITextBox(ui, this->tpIPInfo, (const UTF8Char*)""));
	this->txtIPCountry->SetRect(104, 52, 100, 23, false);
	this->txtIPCountry->SetReadOnly(true);
	this->tpIPWhois = this->tcIP->AddTabPage((const UTF8Char*)"Whois");
	NEW_CLASS(this->txtIPWhois, UI::GUITextBox(ui, this->tpIPWhois, (const UTF8Char*)"", true));
	this->txtIPWhois->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtIPWhois->SetReadOnly(true);

	this->tpLog = this->tcMain->AddTabPage((const UTF8Char*)"Log");
	NEW_CLASS(this->txtLog, UI::GUITextBox(ui, this->tpLog, (const UTF8Char*)""));
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lbLog, UI::GUIListBox(ui, this->tpLog, false));
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSelChg, this);

	NEW_CLASS(this->logger, UI::ListBoxLogger(this, this->lbLog, 500, true));
	this->log->AddLogHandler(this->logger, IO::ILogHandler::LOG_LEVEL_RAW);

	Data::ArrayList<Net::ConnectionInfo*> connInfoList;
	Net::ConnectionInfo *connInfo;
	UTF8Char sbuff[32];
	OSInt i;
	OSInt j;
	OSInt k;
	UInt32 ip;
	this->sockf->GetConnInfoList(&connInfoList);
	i = 0;
	j = connInfoList.GetCount();
	while (i < j)
	{
		connInfo = connInfoList.GetItem(i);
		if (connInfo->GetConnectionStatus() == Net::ConnectionInfo::CS_UP)
		{
			k = 0;
			while (true)
			{
				ip = connInfo->GetIPAddress(k);
				if (ip == 0)
					break;
				Net::SocketUtil::GetIPv4Name(sbuff, ip);
				this->cboIP->AddItem(sbuff, (void*)(OSInt)ip);
				k++;
			}
		}
		DEL_CLASS(connInfo);
		i++;
	}
	if (this->cboIP->GetCount() > 0)
	{
		this->cboIP->SetSelectedIndex(0);
	}
	this->socMon = 0;
	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRPingMonitorForm::~AVIRPingMonitorForm()
{
	SDEL_CLASS(this->socMon);
	if (this->listener)
	{
		DEL_CLASS(this->listener);
		DEL_CLASS(this->webHdlr);
		this->listener = 0;
		this->webHdlr = 0;
	}
	DEL_CLASS(this->log);
	DEL_CLASS(this->logger);

	Data::ArrayList<IPInfo*> *ipList;
	IPInfo *ipInfo;
	OSInt i;
	ipList = this->ipMap->GetValues();
	i = ipList->GetCount();
	while (i-- > 0)
	{
		ipInfo = ipList->GetItem(i);
		SDEL_TEXT(ipInfo->name);
		SDEL_TEXT(ipInfo->country);
		MemFree(ipInfo);
	}

	DEL_CLASS(this->ipMap);
	DEL_CLASS(this->ipMut);
	DEL_CLASS(this->whois);
	DEL_CLASS(this->analyzer);
}

void SSWR::AVIRead::AVIRPingMonitorForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
