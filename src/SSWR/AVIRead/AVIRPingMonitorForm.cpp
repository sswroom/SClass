#include "Stdafx.h"
#include "Manage/HiResClock.h"
#include "Net/ConnectionInfo.h"
#include "SSWR/AVIRead/AVIRPingMonitorForm.h"
#include "Sync/MutexUsage.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRPingMonitorForm::OnPingPacket(void *userData, UInt32 srcIP, UInt32 destIP, UInt8 ttl, UOSInt packetSize)
{
	SSWR::AVIRead::AVIRPingMonitorForm *me = (SSWR::AVIRead::AVIRPingMonitorForm*)userData;
	Text::StringBuilderUTF8 sb;
	UInt32 sortableIP = Net::SocketUtil::IPv4ToSortable(srcIP);
	IPInfo *ipInfo;
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Sync::MutexUsage mutUsage(me->ipMut);
	ipInfo = me->ipMap.Get(sortableIP);
	if (ipInfo == 0)
	{
		Net::WhoisRecord *rec;
		ipInfo = MemAlloc(IPInfo, 1);
		ipInfo->ip = srcIP;
		ipInfo->count = 0;
		rec = me->whois.RequestIP(srcIP);
		if ((sptr = rec->GetNetworkName(sbuff)) != 0)
		{
			ipInfo->name = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
		}
		else
		{
			ipInfo->name = Text::String::New(UTF8STRC("Unknown"));
		}
		if ((sptr = rec->GetCountryCode(sbuff)) != 0)
		{
			ipInfo->country = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
		}
		else
		{
			ipInfo->country = Text::String::New(UTF8STRC("Unk"));
		}
		me->ipMap.Put(sortableIP, ipInfo);
		me->ipListUpdated = true;
	}
	ipInfo->count++;
	if (me->currIP == ipInfo)
	{
		me->ipContUpdated = true;
	}
	mutUsage.EndUse();

	sb.AppendC(UTF8STRC("Received from "));
	sptr = Net::SocketUtil::GetIPv4Name(sbuff, srcIP);
	sb.AppendP(sbuff, sptr);
	sb.AppendC(UTF8STRC(" to "));
	sptr = Net::SocketUtil::GetIPv4Name(sbuff, destIP);
	sb.AppendP(sbuff, sptr);
	sb.AppendC(UTF8STRC(", size = "));
	sb.AppendUOSInt(packetSize);
	sb.AppendC(UTF8STRC(", ttl = "));
	sb.AppendU16(ttl);
	me->log.LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Command);
}

void __stdcall SSWR::AVIRead::AVIRPingMonitorForm::OnRAWData(void *userData, const UInt8 *rawData, UOSInt packetSize)
{
	SSWR::AVIRead::AVIRPingMonitorForm *me = (SSWR::AVIRead::AVIRPingMonitorForm*)userData;
	me->analyzer.PacketIPv4(rawData, packetSize, 0, 0);
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
	me->txtInfo->GetText(sb);
	if (!sb.ToUInt16(port))
	{
		UI::MessageDialog::ShowDialog(CSTR("Info port is not valid"), CSTR("Ping Monitor"), me);
		return;
	}
	NotNullPtr<Net::EthernetWebHandler> webHdlr;
	NEW_CLASSNN(webHdlr, Net::EthernetWebHandler(&me->analyzer));
	NEW_CLASS(me->listener, Net::WebServer::WebListener(me->sockf, 0, webHdlr, port, 60, 3, CSTR("PingMonitor/1.0"), false, Net::WebServer::KeepAlive::Default, true));
	if (me->listener->IsError())
	{
		DEL_CLASS(me->listener);
		webHdlr.Delete();
		me->listener = 0;
		me->webHdlr = 0;
		UI::MessageDialog::ShowDialog(CSTR("Error in listening to info port"), CSTR("Ping Monitor"), me);
		return;
	}
	me->webHdlr = webHdlr.Ptr();
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
		Socket *soc = me->sockf->CreateICMPIPv4Socket(ip);
		if (soc)
		{
			NEW_CLASS(me->socMon, Net::SocketMonitor(me->sockf, soc, OnRAWData, me, 3));
			me->cboIP->SetEnabled(false);
		}
		else
		{
			UI::MessageDialog::ShowDialog(CSTR("Error in listening to ping"), CSTR("Error"), me);
			return;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRPingMonitorForm::OnLogSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRPingMonitorForm *me = (SSWR::AVIRead::AVIRPingMonitorForm*)userObj;
	Text::String *s = me->lbLog->GetSelectedItemTextNew();
	if (s)
	{
		me->txtLog->SetText(s->ToCString());
		s->Release();
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
		UTF8Char *sptr;
		sptr = Text::StrInt64(sbuff, me->currIP->count);
		me->txtIPCount->SetText(CSTRP(sbuff, sptr));
		me->txtIPName->SetText(me->currIP->name->ToCString());
		me->txtIPCountry->SetText(me->currIP->country->ToCString());

		Text::StringBuilderUTF8 sb;
		Net::WhoisRecord *rec = me->whois.RequestIP(me->currIP->ip);
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
		me->txtIPWhois->SetText(sb.ToCString());
	}
	else
	{
		me->txtIPCount->SetText(CSTR(""));
		me->txtIPName->SetText(CSTR(""));
		me->txtIPCountry->SetText(CSTR(""));
		me->txtIPWhois->SetText(CSTR(""));
	}
}

void __stdcall SSWR::AVIRead::AVIRPingMonitorForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRPingMonitorForm *me = (SSWR::AVIRead::AVIRPingMonitorForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	if (me->ipContUpdated)
	{
		me->ipContUpdated = false;
		sptr = Text::StrInt64(sbuff, me->currIP->count);
		me->txtIPCount->SetText(CSTRP(sbuff, sptr));
	}
	if (me->ipListUpdated)
	{
		IPInfo *ipInfo;
		UOSInt i;
		UOSInt j;
		me->ipListUpdated = false;
		Sync::MutexUsage mutUsage(me->ipMut);
		me->lbIP->ClearItems();
		i = 0;
		j = me->ipMap.GetCount();
		while (i < j)
		{
			ipInfo = me->ipMap.GetItem(i);
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, ipInfo->ip);
			me->lbIP->AddItem(CSTRP(sbuff, sptr), ipInfo);
			if (ipInfo == me->currIP)
			{
				me->lbIP->SetSelectedIndex(i);
			}
			i++;
		}
		mutUsage.EndUse();
	}
}

SSWR::AVIRead::AVIRPingMonitorForm::AVIRPingMonitorForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui), whois(core->GetSocketFactory(), 15000), analyzer(0, Net::EthernetAnalyzer::AT_ICMP, CSTR("PingMonitor"))
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Ping Monitor"));

	this->core = core;
	this->sockf = core->GetSocketFactory();
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->listener = 0;
	this->webHdlr = 0;
	this->ipListUpdated = false;
	this->ipContUpdated = false;
	this->currIP = 0;
	this->analyzer.HandlePingv4Request(OnPingPacket, this);

	NEW_CLASS(this->pnlControl, UI::GUIPanel(ui, this));
	this->pnlControl->SetRect(0, 0, 100, 55, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblInfo, UI::GUILabel(ui, this->pnlControl, CSTR("Info Port")));
	this->lblInfo->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtInfo, UI::GUITextBox(ui, this->pnlControl, CSTR("8089")));
	this->txtInfo->SetRect(104, 4, 80, 23, false);
	NEW_CLASS(this->btnInfo, UI::GUIButton(ui, this->pnlControl, CSTR("Start")));
	this->btnInfo->SetRect(184, 4, 75, 23, false);
	this->btnInfo->HandleButtonClick(OnInfoClicked, this);
	NEW_CLASS(this->lblIP, UI::GUILabel(ui, this->pnlControl, CSTR("IP")));
	this->lblIP->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->cboIP, UI::GUIComboBox(ui, this->pnlControl, false));
	this->cboIP->SetRect(104, 28, 150, 23, false);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this->pnlControl, CSTR("Start")));
	this->btnStart->SetRect(254, 28, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	
	this->tpIP = this->tcMain->AddTabPage(CSTR("IP"));
	NEW_CLASS(this->lbIP, UI::GUIListBox(ui, this->tpIP, false));
	this->lbIP->SetRect(0, 0, 150, 23, false);
	this->lbIP->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbIP->HandleSelectionChange(OnIPSelChg, this);
	NEW_CLASS(this->hspIP, UI::GUIHSplitter(ui, this->tpIP, 3, false));
	NEW_CLASS(this->tcIP, UI::GUITabControl(ui, this->tpIP));
	this->tcIP->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpIPInfo = this->tcIP->AddTabPage(CSTR("Info"));
	NEW_CLASS(this->lblIPCount, UI::GUILabel(ui, this->tpIPInfo, CSTR("Count")));
	this->lblIPCount->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtIPCount, UI::GUITextBox(ui, this->tpIPInfo, CSTR("0")));
	this->txtIPCount->SetRect(104, 4, 100, 23, false);
	this->txtIPCount->SetReadOnly(true);
	NEW_CLASS(this->lblIPName, UI::GUILabel(ui, this->tpIPInfo, CSTR("Name")));
	this->lblIPName->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtIPName, UI::GUITextBox(ui, this->tpIPInfo, CSTR("")));
	this->txtIPName->SetRect(104, 28, 200, 23, false);
	this->txtIPName->SetReadOnly(true);
	NEW_CLASS(this->lblIPCountry, UI::GUILabel(ui, this->tpIPInfo, CSTR("Country")));
	this->lblIPCountry->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtIPCountry, UI::GUITextBox(ui, this->tpIPInfo, CSTR("")));
	this->txtIPCountry->SetRect(104, 52, 100, 23, false);
	this->txtIPCountry->SetReadOnly(true);
	this->tpIPWhois = this->tcIP->AddTabPage(CSTR("Whois"));
	NEW_CLASS(this->txtIPWhois, UI::GUITextBox(ui, this->tpIPWhois, CSTR(""), true));
	this->txtIPWhois->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtIPWhois->SetReadOnly(true);

	this->tpLog = this->tcMain->AddTabPage(CSTR("Log"));
	NEW_CLASS(this->txtLog, UI::GUITextBox(ui, this->tpLog, CSTR("")));
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lbLog, UI::GUIListBox(ui, this->tpLog, false));
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSelChg, this);

	NEW_CLASS(this->logger, UI::ListBoxLogger(this, this->lbLog, 500, true));
	this->log.AddLogHandler(this->logger, IO::LogHandler::LogLevel::Raw);

	Data::ArrayList<Net::ConnectionInfo*> connInfoList;
	Net::ConnectionInfo *connInfo;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
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
		if (connInfo->GetConnectionStatus() == Net::ConnectionInfo::CS_UP)
		{
			k = 0;
			while (true)
			{
				ip = connInfo->GetIPAddress(k);
				if (ip == 0)
					break;
				sptr = Net::SocketUtil::GetIPv4Name(sbuff, ip);
				this->cboIP->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)ip);
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
	this->log.RemoveLogHandler(this->logger);
	DEL_CLASS(this->logger);

	IPInfo *ipInfo;
	UOSInt i;
	i = this->ipMap.GetCount();
	while (i-- > 0)
	{
		ipInfo = this->ipMap.GetItem(i);
		ipInfo->name->Release();
		ipInfo->country->Release();
		MemFree(ipInfo);
	}
}

void SSWR::AVIRead::AVIRPingMonitorForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
