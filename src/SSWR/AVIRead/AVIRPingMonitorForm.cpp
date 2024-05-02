#include "Stdafx.h"
#include "Manage/HiResClock.h"
#include "Net/ConnectionInfo.h"
#include "SSWR/AVIRead/AVIRPingMonitorForm.h"
#include "Sync/MutexUsage.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRPingMonitorForm::OnPingPacket(AnyType userData, UInt32 srcIP, UInt32 destIP, UInt8 ttl, UOSInt packetSize)
{
	NN<SSWR::AVIRead::AVIRPingMonitorForm> me = userData.GetNN<SSWR::AVIRead::AVIRPingMonitorForm>();
	Text::StringBuilderUTF8 sb;
	UInt32 sortableIP = Net::SocketUtil::IPv4ToSortable(srcIP);
	NN<IPInfo> ipInfo;
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Sync::MutexUsage mutUsage(me->ipMut);
	if (!me->ipMap.Get(sortableIP).SetTo(ipInfo))
	{
		Net::WhoisRecord *rec;
		ipInfo = MemAllocNN(IPInfo);
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
	if (me->currIP == ipInfo.Ptr())
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

void __stdcall SSWR::AVIRead::AVIRPingMonitorForm::OnRAWData(AnyType userData, const UInt8 *rawData, UOSInt packetSize)
{
	NN<SSWR::AVIRead::AVIRPingMonitorForm> me = userData.GetNN<SSWR::AVIRead::AVIRPingMonitorForm>();
	me->analyzer.PacketIPv4(rawData, packetSize, 0, 0);
}

void __stdcall SSWR::AVIRead::AVIRPingMonitorForm::OnInfoClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRPingMonitorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRPingMonitorForm>();
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
		me->ui->ShowMsgOK(CSTR("Info port is not valid"), CSTR("Ping Monitor"), me);
		return;
	}
	NN<Net::EthernetWebHandler> webHdlr;
	NEW_CLASSNN(webHdlr, Net::EthernetWebHandler(&me->analyzer));
	NEW_CLASS(me->listener, Net::WebServer::WebListener(me->sockf, 0, webHdlr, port, 60, 1, 3, CSTR("PingMonitor/1.0"), false, Net::WebServer::KeepAlive::Default, true));
	if (me->listener->IsError())
	{
		DEL_CLASS(me->listener);
		webHdlr.Delete();
		me->listener = 0;
		me->webHdlr = 0;
		me->ui->ShowMsgOK(CSTR("Error in listening to info port"), CSTR("Ping Monitor"), me);
		return;
	}
	me->webHdlr = webHdlr.Ptr();
	me->txtInfo->SetReadOnly(true);
}

void __stdcall SSWR::AVIRead::AVIRPingMonitorForm::OnStartClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRPingMonitorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRPingMonitorForm>();
	if (me->socMon)
	{
		DEL_CLASS(me->socMon);
		me->socMon = 0;
		me->cboIP->SetEnabled(true);
		return;
	}

	UInt32 ip = (UInt32)me->cboIP->GetSelectedItem().GetOSInt();
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
			me->ui->ShowMsgOK(CSTR("Error in listening to ping"), CSTR("Error"), me);
			return;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRPingMonitorForm::OnLogSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRPingMonitorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRPingMonitorForm>();
	NN<Text::String> s;
	if (me->lbLog->GetSelectedItemTextNew().SetTo(s))
	{
		me->txtLog->SetText(s->ToCString());
		s->Release();
	}
}

void __stdcall SSWR::AVIRead::AVIRPingMonitorForm::OnIPSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRPingMonitorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRPingMonitorForm>();
	me->currIP = (IPInfo*)me->lbIP->GetSelectedItem().p;
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
			sb.AppendJoin(rec->Iterator(), CSTR("\r\n"));
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

void __stdcall SSWR::AVIRead::AVIRPingMonitorForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRPingMonitorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRPingMonitorForm>();
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
		NN<IPInfo> ipInfo;
		UOSInt i;
		UOSInt j;
		me->ipListUpdated = false;
		Sync::MutexUsage mutUsage(me->ipMut);
		me->lbIP->ClearItems();
		i = 0;
		j = me->ipMap.GetCount();
		while (i < j)
		{
			ipInfo = me->ipMap.GetItemNoCheck(i);
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, ipInfo->ip);
			me->lbIP->AddItem(CSTRP(sbuff, sptr), ipInfo);
			if (ipInfo.Ptr() == me->currIP)
			{
				me->lbIP->SetSelectedIndex(i);
			}
			i++;
		}
		mutUsage.EndUse();
	}
}

SSWR::AVIRead::AVIRPingMonitorForm::AVIRPingMonitorForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui), whois(core->GetSocketFactory(), 15000), analyzer(0, Net::EthernetAnalyzer::AT_ICMP, CSTR("PingMonitor"))
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

	this->pnlControl = ui->NewPanel(*this);
	this->pnlControl->SetRect(0, 0, 100, 55, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblInfo = ui->NewLabel(this->pnlControl, CSTR("Info Port"));
	this->lblInfo->SetRect(4, 4, 100, 23, false);
	this->txtInfo = ui->NewTextBox(this->pnlControl, CSTR("8089"));
	this->txtInfo->SetRect(104, 4, 80, 23, false);
	this->btnInfo = ui->NewButton(this->pnlControl, CSTR("Start"));
	this->btnInfo->SetRect(184, 4, 75, 23, false);
	this->btnInfo->HandleButtonClick(OnInfoClicked, this);
	this->lblIP = ui->NewLabel(this->pnlControl, CSTR("IP"));
	this->lblIP->SetRect(4, 28, 100, 23, false);
	this->cboIP = ui->NewComboBox(this->pnlControl, false);
	this->cboIP->SetRect(104, 28, 150, 23, false);
	this->btnStart = ui->NewButton(this->pnlControl, CSTR("Start"));
	this->btnStart->SetRect(254, 28, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	
	this->tpIP = this->tcMain->AddTabPage(CSTR("IP"));
	this->lbIP = ui->NewListBox(this->tpIP, false);
	this->lbIP->SetRect(0, 0, 150, 23, false);
	this->lbIP->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbIP->HandleSelectionChange(OnIPSelChg, this);
	this->hspIP = ui->NewHSplitter(this->tpIP, 3, false);
	this->tcIP = ui->NewTabControl(this->tpIP);
	this->tcIP->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpIPInfo = this->tcIP->AddTabPage(CSTR("Info"));
	this->lblIPCount = ui->NewLabel(this->tpIPInfo, CSTR("Count"));
	this->lblIPCount->SetRect(4, 4, 100, 23, false);
	this->txtIPCount = ui->NewTextBox(this->tpIPInfo, CSTR("0"));
	this->txtIPCount->SetRect(104, 4, 100, 23, false);
	this->txtIPCount->SetReadOnly(true);
	this->lblIPName = ui->NewLabel(this->tpIPInfo, CSTR("Name"));
	this->lblIPName->SetRect(4, 28, 100, 23, false);
	this->txtIPName = ui->NewTextBox(this->tpIPInfo, CSTR(""));
	this->txtIPName->SetRect(104, 28, 200, 23, false);
	this->txtIPName->SetReadOnly(true);
	this->lblIPCountry = ui->NewLabel(this->tpIPInfo, CSTR("Country"));
	this->lblIPCountry->SetRect(4, 52, 100, 23, false);
	this->txtIPCountry = ui->NewTextBox(this->tpIPInfo, CSTR(""));
	this->txtIPCountry->SetRect(104, 52, 100, 23, false);
	this->txtIPCountry->SetReadOnly(true);
	this->tpIPWhois = this->tcIP->AddTabPage(CSTR("Whois"));
	this->txtIPWhois = ui->NewTextBox(this->tpIPWhois, CSTR(""), true);
	this->txtIPWhois->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtIPWhois->SetReadOnly(true);

	this->tpLog = this->tcMain->AddTabPage(CSTR("Log"));
	this->txtLog = ui->NewTextBox(this->tpLog, CSTR(""));
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lbLog = ui->NewListBox(this->tpLog, false);
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSelChg, this);

	NEW_CLASSNN(this->logger, UI::ListBoxLogger(*this, this->lbLog, 500, true));
	this->log.AddLogHandler(this->logger, IO::LogHandler::LogLevel::Raw);

	Data::ArrayListNN<Net::ConnectionInfo> connInfoList;
	NN<Net::ConnectionInfo> connInfo;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
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
		connInfo.Delete();
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
	this->logger.Delete();

	NN<IPInfo> ipInfo;
	UOSInt i;
	i = this->ipMap.GetCount();
	while (i-- > 0)
	{
		ipInfo = this->ipMap.GetItemNoCheck(i);
		ipInfo->name->Release();
		ipInfo->country->Release();
		MemFreeNN(ipInfo);
	}
}

void SSWR::AVIRead::AVIRPingMonitorForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
