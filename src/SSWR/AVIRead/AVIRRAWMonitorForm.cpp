#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Manage/HiResClock.h"
#include "Net/ConnectionInfo.h"
#include "Net/MACInfo.h"
#include "Net/PacketAnalyzerEthernet.h"
#include "SSWR/AVIRead/AVIRRAWMonitorForm.h"
#include "Sync/MutexUsage.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRRAWMonitorForm::OnPingPacket(void *userData, UInt32 srcIP, UInt32 destIP, UInt8 ttl, UOSInt packetSize)
{
	SSWR::AVIRead::AVIRRAWMonitorForm *me = (SSWR::AVIRead::AVIRRAWMonitorForm*)userData;
	Text::StringBuilderUTF8 sb;
	UInt32 sortableIP = Net::SocketUtil::IPv4ToSortable(srcIP);
	PingIPInfo *pingIPInfo;
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Sync::MutexUsage mutUsage(&me->pingIPMut);
	pingIPInfo = me->pingIPMap.Get(sortableIP);
	if (pingIPInfo == 0)
	{
		Net::WhoisRecord *rec;
		pingIPInfo = MemAlloc(PingIPInfo, 1);
		pingIPInfo->ip = srcIP;
		pingIPInfo->count = 0;
		rec = me->whois.RequestIP(srcIP);
		if ((sptr = rec->GetNetworkName(sbuff)) != 0)
		{
			pingIPInfo->name = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
		}
		else
		{
			pingIPInfo->name = Text::String::New(UTF8STRC("Unknown"));
		}
		if ((sptr = rec->GetCountryCode(sbuff)) != 0)
		{
			pingIPInfo->country = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
		}
		else
		{
			pingIPInfo->country = Text::String::New(UTF8STRC("Unk"));
		}
		me->pingIPMap.Put(sortableIP, pingIPInfo);
		me->pingIPListUpdated = true;
	}
	pingIPInfo->count++;
	if (me->currPingIP == pingIPInfo)
	{
		me->pingIPContUpdated = true;
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
	me->log.LogMessage(sb.ToCString(), IO::ILogHandler::LogLevel::Command);
}

void __stdcall SSWR::AVIRead::AVIRRAWMonitorForm::OnRAWData(void *userData, const UInt8 *rawData, UOSInt packetSize)
{
	SSWR::AVIRead::AVIRRAWMonitorForm *me = (SSWR::AVIRead::AVIRRAWMonitorForm*)userData;
	me->analyzer->PacketEthernet(rawData, packetSize);
}

void __stdcall SSWR::AVIRead::AVIRRAWMonitorForm::OnIPv4Data(void *userData, const UInt8 *rawData, UOSInt packetSize)
{
	SSWR::AVIRead::AVIRRAWMonitorForm *me = (SSWR::AVIRead::AVIRRAWMonitorForm*)userData;
	me->analyzer->PacketIPv4(rawData, packetSize, 0, 0);
}

void __stdcall SSWR::AVIRead::AVIRRAWMonitorForm::OnInfoClicked(void *userObj)
{
	SSWR::AVIRead::AVIRRAWMonitorForm *me = (SSWR::AVIRead::AVIRRAWMonitorForm*)userObj;
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
		UI::MessageDialog::ShowDialog(CSTR("Info port is not valid"), CSTR("RAW Monitor"), me);
		return;
	}
	NEW_CLASS(me->webHdlr, Net::EthernetWebHandler(me->analyzer));
	NEW_CLASS(me->listener, Net::WebServer::WebListener(me->sockf, 0, me->webHdlr, port, 60, 3, CSTR("RAWMonitor/1.0"), false, true, true));
	if (me->listener->IsError())
	{
		DEL_CLASS(me->listener);
		DEL_CLASS(me->webHdlr);
		me->listener = 0;
		me->webHdlr = 0;
		UI::MessageDialog::ShowDialog(CSTR("Error in listening to info port"), CSTR("RAW Monitor"), me);
		return;
	}
	me->txtInfo->SetReadOnly(true);
}

void __stdcall SSWR::AVIRead::AVIRRAWMonitorForm::OnStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRRAWMonitorForm *me = (SSWR::AVIRead::AVIRRAWMonitorForm*)userObj;
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
		Socket *soc;
		
		if ((soc = me->sockf->CreateRAWSocket()) != 0)
		{
			NEW_CLASS(me->socMon, Net::SocketMonitor(me->sockf, soc, OnRAWData, me, 3));
			me->cboIP->SetEnabled(false);
		}
		else if ((soc = me->sockf->CreateRAWIPv4Socket(ip)) != 0)
		{
			NEW_CLASS(me->socMon, Net::SocketMonitor(me->sockf, soc, OnIPv4Data, me, 3));
			me->cboIP->SetEnabled(false);
		}
		else
		{
			UI::MessageDialog::ShowDialog(CSTR("Error in listening to socket"), CSTR("Error"), me);
			return;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRRAWMonitorForm::OnIPSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRRAWMonitorForm *me = (SSWR::AVIRead::AVIRRAWMonitorForm*)userObj;
	UInt32 ip = (UInt32)(OSInt)me->cboIP->GetSelectedItem();
	if (ip)
	{
		me->adapterIP = ip;
		me->adapterChanged = true;
	}
}

void __stdcall SSWR::AVIRead::AVIRRAWMonitorForm::OnIPTranSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRRAWMonitorForm *me = (SSWR::AVIRead::AVIRRAWMonitorForm*)userObj;
	IPTranInfo *ipTran = (IPTranInfo*)me->lbIPTran->GetSelectedItem();
	if (ipTran)
	{
		Text::StringBuilderUTF8 sb;
		Net::WhoisRecord *rec = me->whois.RequestIP(ipTran->ip);
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
		me->txtIPTranWhois->SetText(sb.ToCString());
		me->dataUpdated = true;
	}
	else
	{
		me->txtIPTranWhois->SetText(CSTR(""));
	}
}

void __stdcall SSWR::AVIRead::AVIRRAWMonitorForm::OnLogSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRRAWMonitorForm *me = (SSWR::AVIRead::AVIRRAWMonitorForm*)userObj;
	Text::String *s = me->lbLog->GetSelectedItemTextNew();
	if (s)
	{
		me->txtLog->SetText(s->ToCString());
		s->Release();
	}
}

void __stdcall SSWR::AVIRead::AVIRRAWMonitorForm::OnPingIPSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRRAWMonitorForm *me = (SSWR::AVIRead::AVIRRAWMonitorForm*)userObj;
	me->currPingIP = (PingIPInfo*)me->lbPingIP->GetSelectedItem();
	me->pingIPContUpdated = false;
	if (me->currPingIP)
	{
		UTF8Char sbuff[32];
		UTF8Char *sptr;
		sptr = Text::StrInt64(sbuff, me->currPingIP->count);
		me->txtPingIPCount->SetText(CSTRP(sbuff, sptr));
		me->txtPingIPName->SetText(me->currPingIP->name->ToCString());
		me->txtPingIPCountry->SetText(me->currPingIP->country->ToCString());

		Text::StringBuilderUTF8 sb;
		Net::WhoisRecord *rec = me->whois.RequestIP(me->currPingIP->ip);
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
		me->txtPingIPWhois->SetText(sb.ToCString());
	}
	else
	{
		me->txtPingIPCount->SetText(CSTR(""));
		me->txtPingIPName->SetText(CSTR(""));
		me->txtPingIPCountry->SetText(CSTR(""));
		me->txtPingIPWhois->SetText(CSTR(""));
	}
}

void __stdcall SSWR::AVIRead::AVIRRAWMonitorForm::OnDNSReqv4SelChg(void *userObj)
{
	SSWR::AVIRead::AVIRRAWMonitorForm *me = (SSWR::AVIRead::AVIRRAWMonitorForm*)userObj;
	Text::String *name = (Text::String*)me->lbDNSReqv4->GetSelectedItem();
	if (name)
	{
		UTF8Char sbuff[64];
		UTF8Char *sptr;
		Data::DateTime reqTime;
		UInt32 ttl;
		Data::ArrayList<Net::DNSClient::RequestAnswer*> ansList;
		Net::DNSClient::RequestAnswer *ans;
		if (me->analyzer->DNSReqv4GetInfo(name->ToCString(), &ansList, &reqTime, &ttl))
		{
			UOSInt i;
			UOSInt j;
			me->txtDNSReqv4Name->SetText(name->ToCString());
			reqTime.ToLocalTime();
			sptr = reqTime.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
			me->txtDNSReqv4ReqTime->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrUInt32(sbuff, ttl);
			me->txtDNSReqv4TTL->SetText(CSTRP(sbuff, sptr));
			me->lvDNSReqv4->ClearItems();
			i = 0;
			j = ansList.GetCount();
			while (i < j)
			{
				ans = ansList.GetItem(i);
				me->lvDNSReqv4->AddItem(ans->name, ans);
				sptr = Text::StrInt32(sbuff, ans->recType);
				me->lvDNSReqv4->SetSubItem(i, 1, CSTRP(sbuff, sptr));
				sptr = Text::StrInt32(sbuff, ans->recClass);
				me->lvDNSReqv4->SetSubItem(i, 2, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt32(sbuff, ans->ttl);
				me->lvDNSReqv4->SetSubItem(i, 3, CSTRP(sbuff, sptr));
				me->lvDNSReqv4->SetSubItem(i, 4, ans->rd);
				i++;
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRRAWMonitorForm::OnDNSReqv6SelChg(void *userObj)
{
	SSWR::AVIRead::AVIRRAWMonitorForm *me = (SSWR::AVIRead::AVIRRAWMonitorForm*)userObj;
	Text::String *name = (Text::String*)me->lbDNSReqv6->GetSelectedItem();
	if (name)
	{
		UTF8Char sbuff[64];
		UTF8Char *sptr;
		Data::DateTime reqTime;
		UInt32 ttl;
		Data::ArrayList<Net::DNSClient::RequestAnswer*> ansList;
		Net::DNSClient::RequestAnswer *ans;
		if (me->analyzer->DNSReqv6GetInfo(name->ToCString(), &ansList, &reqTime, &ttl))
		{
			UOSInt i;
			UOSInt j;
			me->txtDNSReqv6Name->SetText(name->ToCString());
			reqTime.ToLocalTime();
			sptr = reqTime.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
			me->txtDNSReqv6ReqTime->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrUInt32(sbuff, ttl);
			me->txtDNSReqv6TTL->SetText(CSTRP(sbuff, sptr));
			me->lvDNSReqv6->ClearItems();
			i = 0;
			j = ansList.GetCount();
			while (i < j)
			{
				ans = ansList.GetItem(i);
				me->lvDNSReqv6->AddItem(ans->name, ans);
				sptr = Text::StrInt32(sbuff, ans->recType);
				me->lvDNSReqv6->SetSubItem(i, 1, CSTRP(sbuff, sptr));
				sptr = Text::StrInt32(sbuff, ans->recClass);
				me->lvDNSReqv6->SetSubItem(i, 2, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt32(sbuff, ans->ttl);
				me->lvDNSReqv6->SetSubItem(i, 3, CSTRP(sbuff, sptr));
				me->lvDNSReqv6->SetSubItem(i, 4, ans->rd);
				i++;
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRRAWMonitorForm::OnDNSReqOthSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRRAWMonitorForm *me = (SSWR::AVIRead::AVIRRAWMonitorForm*)userObj;
	Text::String *name = (Text::String*)me->lbDNSReqOth->GetSelectedItem();
	if (name)
	{
		UTF8Char sbuff[64];
		UTF8Char *sptr;
		Data::DateTime reqTime;
		UInt32 ttl;
		Data::ArrayList<Net::DNSClient::RequestAnswer*> ansList;
		Net::DNSClient::RequestAnswer *ans;
		if (me->analyzer->DNSReqOthGetInfo(name->ToCString(), &ansList, &reqTime, &ttl))
		{
			UOSInt i;
			UOSInt j;
			me->txtDNSReqOthName->SetText(name->ToCString());
			reqTime.ToLocalTime();
			sptr = reqTime.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
			me->txtDNSReqOthReqTime->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrUInt32(sbuff, ttl);
			me->txtDNSReqOthTTL->SetText(CSTRP(sbuff, sptr));
			me->lvDNSReqOth->ClearItems();
			i = 0;
			j = ansList.GetCount();
			while (i < j)
			{
				ans = ansList.GetItem(i);
				me->lvDNSReqOth->AddItem(ans->name, ans);
				sptr = Text::StrInt32(sbuff, ans->recType);
				me->lvDNSReqOth->SetSubItem(i, 1, CSTRP(sbuff, sptr));
				sptr = Text::StrInt32(sbuff, ans->recClass);
				me->lvDNSReqOth->SetSubItem(i, 2, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt32(sbuff, ans->ttl);
				me->lvDNSReqOth->SetSubItem(i, 3, CSTRP(sbuff, sptr));
				me->lvDNSReqOth->SetSubItem(i, 4, ans->rd);
				i++;
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRRAWMonitorForm::OnDNSTargetSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRRAWMonitorForm *me = (SSWR::AVIRead::AVIRRAWMonitorForm*)userObj;
	Net::EthernetAnalyzer::DNSTargetInfo *target = (Net::EthernetAnalyzer::DNSTargetInfo*)me->lbDNSTarget->GetSelectedItem();
	me->lbDNSTargetDomain->ClearItems();
	if (target)
	{
		UOSInt i;
		UOSInt j;
		Sync::MutexUsage mutUsage(&target->mut);
		i = 0;
		j = target->addrList.GetCount();
		while (i < j)
		{
			me->lbDNSTargetDomain->AddItem(target->addrList.GetItem(i), 0);
			i++;
		}
		mutUsage.EndUse();
		Net::WhoisRecord *rec = me->whois.RequestIP(target->ip);
		if (rec)
		{
			Text::StringBuilderUTF8 sb;
			i = 0;
			j = rec->GetCount();
			while (i < j)
			{
				sb.Append(rec->GetItem(i));
				sb.AppendC(UTF8STRC("\r\n"));
				i++;
			}
			me->txtDNSTargetWhois->SetText(sb.ToCString());
		}
		else
		{
			me->txtDNSTargetWhois->SetText(CSTR(""));
		}		
	}
	else
	{
		me->txtDNSTargetWhois->SetText(CSTR(""));
	}
}

void __stdcall SSWR::AVIRead::AVIRRAWMonitorForm::OnMDNSSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRRAWMonitorForm *me = (SSWR::AVIRead::AVIRRAWMonitorForm*)userObj;
	Net::DNSClient::RequestAnswer *ans = (Net::DNSClient::RequestAnswer*)me->lbMDNS->GetSelectedItem();
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	if (ans)
	{
		me->txtMDNSName->SetText(ans->name->ToCString());
		sptr = Text::StrUInt16(sbuff, ans->recType);
		Text::CString typeId = Net::DNSClient::TypeGetID(ans->recType);
		if (typeId.v)
		{
			sptr = Text::StrConcatC(sptr, UTF8STRC(" ("));
			sptr = typeId.ConcatTo(sptr);
			sptr = Text::StrConcatC(sptr, UTF8STRC(")"));
		}
		me->txtMDNSType->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrUInt16(sbuff, ans->recClass & 0x7fff);
		if (ans->recClass & 0x8000)
		{
			sptr = Text::StrConcatC(sptr, UTF8STRC(", cache flush"));
		}
		me->txtMDNSClass->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrUInt32(sbuff, ans->ttl);
		me->txtMDNSTTL->SetText(CSTRP(sbuff, sptr));
		if (ans->rd)
		{
			me->txtMDNSResult->SetText(ans->rd->ToCString());
		}
		else
		{
			me->txtMDNSResult->SetText(CSTR(""));
		}
	}
	else
	{
		me->txtMDNSName->SetText(CSTR(""));
		me->txtMDNSType->SetText(CSTR(""));
		me->txtMDNSClass->SetText(CSTR(""));
		me->txtMDNSTTL->SetText(CSTR(""));
		me->txtMDNSResult->SetText(CSTR(""));
	}
}

void __stdcall SSWR::AVIRead::AVIRRAWMonitorForm::OnDNSClientSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRRAWMonitorForm *me = (SSWR::AVIRead::AVIRRAWMonitorForm*)userObj;
	Net::EthernetAnalyzer::DNSClientInfo *cli = (Net::EthernetAnalyzer::DNSClientInfo*)me->lbDNSClient->GetSelectedItem();
	UOSInt i;
	UOSInt j;
	Net::EthernetAnalyzer::DNSCliHourInfo *hourInfo;
	UTF8Char sbuff[64];
	UTF8Char *sptr;	
	me->lvDNSClient->ClearItems();
	if (cli)
	{
		Sync::MutexUsage mutUsage(&cli->mut);
		i = 0;
		j = cli->hourInfos.GetCount();
		while (i < j)
		{
			hourInfo = cli->hourInfos.GetItem(i);
			sptr = Text::StrInt32(sbuff, hourInfo->year);
			*sptr++ = '-';
			sptr = Text::StrInt32(sptr, hourInfo->month);
			*sptr++ = '-';
			sptr = Text::StrInt32(sptr, hourInfo->day);
			*sptr++ = ' ';
			sptr = Text::StrInt32(sptr, hourInfo->hour);
			sptr = Text::StrConcatC(sptr, UTF8STRC(":00"));
			me->lvDNSClient->AddItem(CSTRP(sbuff, sptr), 0);
			sptr = Text::StrUInt64(sbuff, hourInfo->reqCount);
			me->lvDNSClient->SetSubItem(i, 1, CSTRP(sbuff, sptr));
			i++;
		}
		mutUsage.EndUse();
	}
}

void __stdcall SSWR::AVIRead::AVIRRAWMonitorForm::OnIPLogSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRRAWMonitorForm *me = (SSWR::AVIRead::AVIRRAWMonitorForm*)userObj;
	Net::EthernetAnalyzer::IPLogInfo *ipLog = (Net::EthernetAnalyzer::IPLogInfo*)me->lbIPLog->GetSelectedItem();
	UOSInt i;
	UOSInt j;
	me->lbIPLogVal->ClearItems();
	if (ipLog)
	{
		Sync::MutexUsage mutUsage(&ipLog->mut);
		i = 0;
		j = ipLog->logList.GetCount();
		while (i < j)
		{
			me->lbIPLogVal->AddItem(ipLog->logList.GetItem(i), 0);
			i++;
		}
		mutUsage.EndUse();
	}
}

void __stdcall SSWR::AVIRead::AVIRRAWMonitorForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRRAWMonitorForm *me = (SSWR::AVIRead::AVIRRAWMonitorForm*)userObj;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	if (me->pingIPContUpdated)
	{
		me->pingIPContUpdated = false;
		sptr = Text::StrInt64(sbuff, me->currPingIP->count);
		me->txtPingIPCount->SetText(CSTRP(sbuff, sptr));
	}
	if (me->pingIPListUpdated)
	{
		PingIPInfo *pingIPInfo;
		UOSInt i;
		UOSInt j;
		me->pingIPListUpdated = false;
		Sync::MutexUsage mutUsage(&me->pingIPMut);
		me->lbPingIP->ClearItems();
		i = 0;
		j = me->pingIPMap.GetCount();
		while (i < j)
		{
			pingIPInfo = me->pingIPMap.GetItem(i);
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, pingIPInfo->ip);
			me->lbPingIP->AddItem(CSTRP(sbuff, sptr), pingIPInfo);
			if (pingIPInfo == me->currPingIP)
			{
				me->lbPingIP->SetSelectedIndex(i);
			}
			i++;
		}
		mutUsage.EndUse();
	}
	if (me->analyzer->DNSReqv4GetCount() != me->lbDNSReqv4->GetCount())
	{
		Data::ArrayList<Text::String *> nameList;
		Text::String *selName = (Text::String*)me->lbDNSReqv4->GetSelectedItem();
		Text::String *s;
		UOSInt i;
		UOSInt j;
		me->lbDNSReqv4->ClearItems();
		me->analyzer->DNSReqv4GetList(&nameList);
		i = 0;
		j = nameList.GetCount();
		while (i < j)
		{
			s = nameList.GetItem(i);
			me->lbDNSReqv4->AddItem(s, (void*)s);
			if (s == selName)
			{
				me->lbDNSReqv4->SetSelectedIndex(i);
			}
			i++;
		}
	}
	if (me->analyzer->DNSReqv6GetCount() != me->lbDNSReqv6->GetCount())
	{
		Data::ArrayList<Text::String *> nameList;
		Text::String *selName = (Text::String*)me->lbDNSReqv6->GetSelectedItem();
		Text::String *s;
		UOSInt i;
		UOSInt j;
		me->lbDNSReqv6->ClearItems();
		me->analyzer->DNSReqv6GetList(&nameList);
		i = 0;
		j = nameList.GetCount();
		while (i < j)
		{
			s = nameList.GetItem(i);
			me->lbDNSReqv6->AddItem(s, (void*)s);
			if (s == selName)
			{
				me->lbDNSReqv6->SetSelectedIndex(i);
			}
			i++;
		}
	}
	if (me->analyzer->DNSReqOthGetCount() != me->lbDNSReqOth->GetCount())
	{
		Data::ArrayList<Text::String *> nameList;
		Text::String *selName = (Text::String*)me->lbDNSReqOth->GetSelectedItem();
		Text::String *s;
		UOSInt i;
		UOSInt j;
		me->lbDNSReqOth->ClearItems();
		me->analyzer->DNSReqOthGetList(&nameList);
		i = 0;
		j = nameList.GetCount();
		while (i < j)
		{
			s = nameList.GetItem(i);
			me->lbDNSReqOth->AddItem(s, (void*)s);
			if (s == selName)
			{
				me->lbDNSReqOth->SetSelectedIndex(i);
			}
			i++;
		}
	}
	if (me->analyzer->DNSTargetGetCount() != me->lbDNSTarget->GetCount())
	{
		Data::ArrayList<Net::EthernetAnalyzer::DNSTargetInfo *> targetList;
		Net::EthernetAnalyzer::DNSTargetInfo *target;
		Net::EthernetAnalyzer::DNSTargetInfo *currSel = (Net::EthernetAnalyzer::DNSTargetInfo*)me->lbDNSTarget->GetSelectedItem();
		UOSInt i;
		UOSInt j;
		me->analyzer->DNSTargetGetList(&targetList);
		me->lbDNSTarget->ClearItems();
		i = 0;
		j = targetList.GetCount();
		while (i < j)
		{
			target = targetList.GetItem(i);
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, target->ip);
			me->lbDNSTarget->AddItem(CSTRP(sbuff, sptr), target);
			if (target == currSel)
			{
				me->lbDNSTarget->SetSelectedIndex(i);
			}
			i++;
		}
	}
	if (me->analyzer->MDNSGetCount() != me->lbMDNS->GetCount())
	{
		Data::ArrayList<Net::DNSClient::RequestAnswer *> mdnsList;
		Net::DNSClient::RequestAnswer *ans;
		Net::DNSClient::RequestAnswer *currSel = (Net::DNSClient::RequestAnswer*)me->lbMDNS->GetSelectedItem();
		UOSInt i;
		UOSInt j;
		me->analyzer->MDNSGetList(&mdnsList);
		me->lbMDNS->ClearItems();
		i = 0;
		j = mdnsList.GetCount();
		while (i < j)
		{
			ans = mdnsList.GetItem(i);
			me->lbMDNS->AddItem(ans->name, ans);
			if (ans == currSel)
			{
				me->lbMDNS->SetSelectedIndex(i);
			}
			i++;
		}
	}
	if (me->analyzer->DNSCliGetCount() != me->lbDNSClient->GetCount())
	{
		Net::EthernetAnalyzer::DNSClientInfo *cli;
		Net::EthernetAnalyzer::DNSClientInfo *currSel = (Net::EthernetAnalyzer::DNSClientInfo*)me->lbDNSClient->GetSelectedItem();
		Data::ArrayList<Net::EthernetAnalyzer::DNSClientInfo*> cliList;
		UOSInt i;
		UOSInt j;
		Sync::MutexUsage mutUsage;
		me->analyzer->UseDNSCli(&mutUsage);
		cliList.AddAll(me->analyzer->DNSCliGetList());
		mutUsage.EndUse();
		me->lbDNSClient->ClearItems();
		i = 0;
		j = cliList.GetCount();
		while (i < j)
		{
			cli = cliList.GetItem(i);
			sptr = Net::SocketUtil::GetAddrName(sbuff, &cli->addr);
			me->lbDNSClient->AddItem(CSTRP(sbuff, sptr), cli);
			if (cli == currSel)
			{
				me->lbDNSClient->SetSelectedIndex(i);
			}
			i++;
		}
	}
	if (me->analyzer->IPLogGetCount() != me->lbIPLog->GetCount())
	{
		Net::EthernetAnalyzer::IPLogInfo *ipLog;
		Net::EthernetAnalyzer::IPLogInfo *currSel = (Net::EthernetAnalyzer::IPLogInfo*)me->lbIPLog->GetSelectedItem();
		Data::ArrayList<Net::EthernetAnalyzer::IPLogInfo*> ipLogList;
		UOSInt i;
		UOSInt j;
		Sync::MutexUsage mutUsage;
		me->analyzer->UseIPLog(&mutUsage);
		ipLogList.AddAll(me->analyzer->IPLogGetList());
		mutUsage.EndUse();
		me->lbIPLog->ClearItems();
		i = 0;
		j = ipLogList.GetCount();
		while (i < j)
		{
			ipLog = ipLogList.GetItem(i);
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, ipLog->ip);
			me->lbIPLog->AddItem(CSTRP(sbuff, sptr), ipLog);
			if (ipLog == currSel)
			{
				me->lbIPLog->SetSelectedIndex(i);
			}
			i++;
		}
	}
	if (me->adapterChanged || me->analyzer->IPTranGetCount() != me->ipTranCnt)
	{
		Bool listChg = false;
		UOSInt i;
		UOSInt j;
		Data::ArrayList<Net::EthernetAnalyzer::IPTranStatus*> ipTranList;
		Net::EthernetAnalyzer::IPTranStatus *status;
		IPTranInfo *ipTran;
		Sync::MutexUsage mutUsage;
		me->adapterChanged = false;
		me->analyzer->UseIPTran(&mutUsage);
		ipTranList.AddAll(me->analyzer->IPTranGetList());
		mutUsage.EndUse();
		me->ipTranCnt = ipTranList.GetCount();
		i = 0;
		j = ipTranList.GetCount();
		while (i < j)
		{
			status = ipTranList.GetItem(i);
			if (status->srcIP == me->adapterIP)
			{
				ipTran = me->ipTranMap.Get(Net::SocketUtil::IPv4ToSortable(status->destIP));
				if (ipTran)
				{
					ipTran->sendStatus = status;
				}
				else
				{
					listChg = true;
					ipTran = MemAlloc(IPTranInfo, 1);
					ipTran->ip = status->destIP;
					ipTran->recvStatus = 0;
					ipTran->sendStatus = status;
					me->ipTranMap.Put(Net::SocketUtil::IPv4ToSortable(ipTran->ip), ipTran);
				}
			}
			else if (status->destIP == me->adapterIP)
			{
				ipTran = me->ipTranMap.Get(Net::SocketUtil::IPv4ToSortable(status->srcIP));
				if (ipTran)
				{
					ipTran->recvStatus = status;
				}
				else
				{
					listChg = true;
					ipTran = MemAlloc(IPTranInfo, 1);
					ipTran->ip = status->srcIP;
					ipTran->recvStatus = status;
					ipTran->sendStatus = 0;
					me->ipTranMap.Put(Net::SocketUtil::IPv4ToSortable(ipTran->ip), ipTran);
				}
			}
			i++;
		}

		if (listChg)
		{
			IPTranInfo *currSel = (IPTranInfo*)me->lbIPTran->GetSelectedItem();
			me->lbIPTran->ClearItems();
			i = 0;
			j = me->ipTranMap.GetCount();
			while (i < j)
			{
				ipTran = me->ipTranMap.GetItem(i);
				sptr = Net::SocketUtil::GetIPv4Name(sbuff, ipTran->ip);
				me->lbIPTran->AddItem(CSTRP(sbuff, sptr), ipTran);
				if (currSel == ipTran)
				{
					me->lbIPTran->SetSelectedIndex(i);
				}
				i++;
			}
		}
	}
	if (me->analyzer->TCP4SYNIsDiff(me->tcp4synLastIndex))
	{
		Data::ArrayList<Net::EthernetAnalyzer::TCP4SYNInfo> synList;
		Net::EthernetAnalyzer::TCP4SYNInfo syn;
		UOSInt i = 0;
		UOSInt j = me->analyzer->TCP4SYNGetList(&synList, &me->tcp4synLastIndex);
		me->lvTCP4SYN->ClearItems();
		while (i < j)
		{
			syn = synList.GetItem(i);
			sptr = syn.reqTime.ToString(sbuff);
			me->lvTCP4SYN->AddItem(CSTRP(sbuff, sptr), 0);
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, syn.srcAddr);
			me->lvTCP4SYN->SetSubItem(i, 1, CSTRP(sbuff, sptr));
			sptr = Text::StrUInt16(sbuff, syn.srcPort);
			me->lvTCP4SYN->SetSubItem(i, 2, CSTRP(sbuff, sptr));
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, syn.destAddr);
			me->lvTCP4SYN->SetSubItem(i, 3, CSTRP(sbuff, sptr));
			sptr = Text::StrUInt16(sbuff, syn.destPort);
			me->lvTCP4SYN->SetSubItem(i, 4, CSTRP(sbuff, sptr));
			i++;
		}
	}

	if (me->socMon || me->dataUpdated)
	{
		IPTranInfo *currSel = (IPTranInfo*)me->lbIPTran->GetSelectedItem();
		UOSInt i;
		UOSInt j;
		me->dataUpdated = false;
		if (currSel)
		{
			if (currSel->recvStatus)
			{
				sptr = Text::StrUInt64(sbuff, currSel->recvStatus->tcpCnt);
				me->lvIPTranInfo->SetSubItem(0, 1, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt64(sbuff, currSel->recvStatus->tcpSize);
				me->lvIPTranInfo->SetSubItem(2, 1, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt64(sbuff, currSel->recvStatus->udpCnt);
				me->lvIPTranInfo->SetSubItem(4, 1, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt64(sbuff, currSel->recvStatus->udpSize);
				me->lvIPTranInfo->SetSubItem(6, 1, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt64(sbuff, currSel->recvStatus->icmpCnt);
				me->lvIPTranInfo->SetSubItem(8, 1, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt64(sbuff, currSel->recvStatus->icmpSize);
				me->lvIPTranInfo->SetSubItem(10, 1, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt64(sbuff, currSel->recvStatus->otherCnt);
				me->lvIPTranInfo->SetSubItem(12, 1, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt64(sbuff, currSel->recvStatus->otherSize);
				me->lvIPTranInfo->SetSubItem(14, 1, CSTRP(sbuff, sptr));
			}
			else
			{
				me->lvIPTranInfo->SetSubItem(0, 1, CSTR("0"));
				me->lvIPTranInfo->SetSubItem(2, 1, CSTR("0"));
				me->lvIPTranInfo->SetSubItem(4, 1, CSTR("0"));
				me->lvIPTranInfo->SetSubItem(6, 1, CSTR("0"));
				me->lvIPTranInfo->SetSubItem(8, 1, CSTR("0"));
				me->lvIPTranInfo->SetSubItem(10, 1, CSTR("0"));
				me->lvIPTranInfo->SetSubItem(12, 1, CSTR("0"));
				me->lvIPTranInfo->SetSubItem(14, 1, CSTR("0"));
			}
			
			if (currSel->sendStatus)
			{
				sptr = Text::StrUInt64(sbuff, currSel->sendStatus->tcpCnt);
				me->lvIPTranInfo->SetSubItem(1, 1, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt64(sbuff, currSel->sendStatus->tcpSize);
				me->lvIPTranInfo->SetSubItem(3, 1, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt64(sbuff, currSel->sendStatus->udpCnt);
				me->lvIPTranInfo->SetSubItem(5, 1, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt64(sbuff, currSel->sendStatus->udpSize);
				me->lvIPTranInfo->SetSubItem(7, 1, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt64(sbuff, currSel->sendStatus->icmpCnt);
				me->lvIPTranInfo->SetSubItem(9, 1, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt64(sbuff, currSel->sendStatus->icmpSize);
				me->lvIPTranInfo->SetSubItem(11, 1, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt64(sbuff, currSel->sendStatus->otherCnt);
				me->lvIPTranInfo->SetSubItem(13, 1, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt64(sbuff, currSel->sendStatus->otherSize);
				me->lvIPTranInfo->SetSubItem(15, 1, CSTRP(sbuff, sptr));
			}
			else
			{
				me->lvIPTranInfo->SetSubItem(1, 1, CSTR("0"));
				me->lvIPTranInfo->SetSubItem(3, 1, CSTR("0"));
				me->lvIPTranInfo->SetSubItem(5, 1, CSTR("0"));
				me->lvIPTranInfo->SetSubItem(7, 1, CSTR("0"));
				me->lvIPTranInfo->SetSubItem(9, 1, CSTR("0"));
				me->lvIPTranInfo->SetSubItem(11, 1, CSTR("0"));
				me->lvIPTranInfo->SetSubItem(13, 1, CSTR("0"));
				me->lvIPTranInfo->SetSubItem(15, 1, CSTR("0"));
			}
		}

		const Data::ReadingList<Net::EthernetAnalyzer::MACStatus *> *macList;
		Net::EthernetAnalyzer::MACStatus *mac;
		const Net::MACInfo::MACEntry *entry;
		UInt8 macBuff[8];
		Sync::MutexUsage mutUsage;
		me->analyzer->UseMAC(&mutUsage);
		macList = me->analyzer->MACGetList();
		j = macList->GetCount();
		if (j != me->lvDevice->GetCount())
		{
			me->lvDevice->ClearItems();
			i = 0;
			while (i < j)
			{
				mac = macList->GetItem(i);
				WriteMUInt64(macBuff, mac->macAddr);
				sptr = Text::StrHexBytes(sbuff, &macBuff[2], 6, ':');
				me->lvDevice->AddItem(CSTRP(sbuff, sptr), mac);
				entry = Net::MACInfo::GetMACInfo(mac->macAddr);
				me->lvDevice->SetSubItem(i, 1, {entry->name, entry->nameLen});
				if (mac->name)
				{
					me->lvDevice->SetSubItem(i, 8, mac->name);
				}
				else
				{
					me->lvDevice->SetSubItem(i, 8, CSTR("Unknown"));
				}
				i++;
			}
		}
		i = 0;
		while (i < j)
		{
			mac = macList->GetItem(i);
			sptr = Text::StrUInt64(sbuff, mac->ipv4SrcCnt);
			me->lvDevice->SetSubItem(i, 2, CSTRP(sbuff, sptr));
			sptr = Text::StrUInt64(sbuff, mac->ipv4DestCnt);
			me->lvDevice->SetSubItem(i, 3, CSTRP(sbuff, sptr));
			sptr = Text::StrUInt64(sbuff, mac->ipv6SrcCnt);
			me->lvDevice->SetSubItem(i, 4, CSTRP(sbuff, sptr));
			sptr = Text::StrUInt64(sbuff, mac->ipv6DestCnt);
			me->lvDevice->SetSubItem(i, 5, CSTRP(sbuff, sptr));
			sptr = Text::StrUInt64(sbuff, mac->othSrcCnt);
			me->lvDevice->SetSubItem(i, 6, CSTRP(sbuff, sptr));
			sptr = Text::StrUInt64(sbuff, mac->othDestCnt);
			me->lvDevice->SetSubItem(i, 7, CSTRP(sbuff, sptr));

			if (mac->ipv4Addr[0])
			{
				if (mac->ipv6Addr.addrType == Net::AddrType::IPv6)
				{
					sptr = Net::SocketUtil::GetAddrName(Text::StrConcatC(Net::SocketUtil::GetIPv4Name(sbuff, mac->ipv4Addr[0]), UTF8STRC(", ")), &mac->ipv6Addr);
					me->lvDevice->SetSubItem(i, 9, CSTRP(sbuff, sptr));
				}
				else
				{
					sptr = Net::SocketUtil::GetIPv4Name(sbuff, mac->ipv4Addr[0]);
					me->lvDevice->SetSubItem(i, 9, CSTRP(sbuff, sptr));
				}
			}
			else
			{
				if (mac->ipv6Addr.addrType == Net::AddrType::IPv6)
				{
					sptr = Net::SocketUtil::GetAddrName(sbuff, &mac->ipv6Addr);
					me->lvDevice->SetSubItem(i, 9, CSTRP(sbuff, sptr));
				}
				else
				{
					me->lvDevice->SetSubItem(i, 9, CSTR(""));
				}
			}
			i++;
		}
		mutUsage.EndUse();
	}

	{
		UInt8 mac[8];
		UOSInt i;
		UOSInt j;
		UOSInt k;
		UTF8Char *sptr;
		Net::EthernetAnalyzer::DHCPInfo *dhcp;
		const Net::MACInfo::MACEntry *macInfo;
		Sync::MutexUsage mutUsage;
		me->analyzer->UseDHCP(&mutUsage);
		const Data::ReadingList<Net::EthernetAnalyzer::DHCPInfo*> *dhcpList = me->analyzer->DHCPGetList();
		if (dhcpList->GetCount() != me->lvDHCP->GetCount())
		{
			Net::EthernetAnalyzer::DHCPInfo *currSel = (Net::EthernetAnalyzer::DHCPInfo*)me->lvDHCP->GetSelectedItem();
			me->lvDHCP->ClearItems();
			i = 0;
			j = dhcpList->GetCount();
			while (i < j)
			{
				dhcp = dhcpList->GetItem(i);
				WriteMUInt64(mac, dhcp->iMAC);
				sptr = Text::StrHexBytes(sbuff, &mac[2], 6, ':');
				me->lvDHCP->AddItem(CSTRP(sbuff, sptr), dhcp);
				macInfo = Net::MACInfo::GetMACInfo(dhcp->iMAC);
				me->lvDHCP->SetSubItem(i, 1, {macInfo->name, macInfo->nameLen});
				if (dhcp == currSel)
				{
					me->lvDHCP->SetSelectedIndex(i);
				}
				dhcp->updated = true;
				i++;
			}
		}

		i = 0;
		j = dhcpList->GetCount();
		while (i < j)
		{
			dhcp = dhcpList->GetItem(i);
			if (dhcp->updated)
			{
				Data::DateTime dt;
				Sync::MutexUsage mutUsage(&dhcp->mut);
				dhcp->updated = false;
				sptr = Net::SocketUtil::GetIPv4Name(sbuff, dhcp->ipAddr);
				me->lvDHCP->SetSubItem(i, 2, CSTRP(sbuff, sptr));
				sptr = Net::SocketUtil::GetIPv4Name(sbuff, dhcp->subnetMask);
				me->lvDHCP->SetSubItem(i, 3, CSTRP(sbuff, sptr));
				sptr = Net::SocketUtil::GetIPv4Name(sbuff, dhcp->gwAddr);
				me->lvDHCP->SetSubItem(i, 4, CSTRP(sbuff, sptr));
				sbuff[0] = 0;
				sptr = sbuff;
				k = 0;
				while (k < 4)
				{
					if (dhcp->dns[k] == 0)
						break;
					if (k > 0)
					{
						sptr = Text::StrConcatC(sptr, UTF8STRC(", "));
					}
					sptr = Net::SocketUtil::GetIPv4Name(sptr, dhcp->dns[k]);
					k++;
				}
				me->lvDHCP->SetSubItem(i, 5, CSTRP(sbuff, sptr));
				sptr = Net::SocketUtil::GetIPv4Name(sbuff, dhcp->dhcpServer);
				me->lvDHCP->SetSubItem(i, 6, CSTRP(sbuff, sptr));
				sptr = Net::SocketUtil::GetIPv4Name(sbuff, dhcp->router);
				me->lvDHCP->SetSubItem(i, 7, CSTRP(sbuff, sptr));
				dt.SetTicks(dhcp->ipAddrTime);
				dt.ToLocalTime();
				sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
				me->lvDHCP->SetSubItem(i, 8, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt32(sbuff, dhcp->ipAddrLease);
				me->lvDHCP->SetSubItem(i, 9, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt32(sbuff, dhcp->renewTime);
				me->lvDHCP->SetSubItem(i, 10, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt32(sbuff, dhcp->rebindTime);
				me->lvDHCP->SetSubItem(i, 11, CSTRP(sbuff, sptr));
				if (dhcp->hostName)
					me->lvDHCP->SetSubItem(i, 12, dhcp->hostName);
				if (dhcp->vendorClass)
					me->lvDHCP->SetSubItem(i, 13, dhcp->vendorClass);
				mutUsage.EndUse();
			}
			i++;
		}
		mutUsage.EndUse();
	}
}

void __stdcall SSWR::AVIRead::AVIRRAWMonitorForm::OnDeviceSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRRAWMonitorForm *me = (SSWR::AVIRead::AVIRRAWMonitorForm*)userObj;
	Net::EthernetAnalyzer::MACStatus *mac = (Net::EthernetAnalyzer::MACStatus *)me->lvDevice->GetSelectedItem();
	Data::DateTime dt;
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	if (mac)
	{
		UOSInt cnt;
		UOSInt i;
		Sync::MutexUsage mutUsage;
		me->analyzer->UseMAC(&mutUsage);
		cnt = (UOSInt)(mac->ipv4SrcCnt + mac->ipv6SrcCnt + mac->othSrcCnt);
		if (cnt <= 16)
		{
			i = 0;
			while (i < cnt)
			{
				dt.SetTicks(mac->packetTime[i]);
				dt.ToLocalTime();
				sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
				sb.AppendP(sbuff, sptr);
				sb.AppendC(UTF8STRC("\r\n"));
				sb.AppendC(UTF8STRC("Dest MAC: "));
				WriteMUInt64(sbuff, mac->packetDestMAC[i]);
				Net::PacketAnalyzerEthernet::PacketEthernetDataGetDetail(mac->packetEtherType[i], mac->packetData[i], mac->packetSize[i], &sb);
				sb.AppendC(UTF8STRC("\r\n"));
				sb.AppendC(UTF8STRC("\r\n"));
				i++;
			}
		}
		else
		{
			i = 0;
			while (i < 16)
			{
				dt.SetTicks(mac->packetTime[(cnt + i) & 15]);
				dt.ToLocalTime();
				sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
				sb.AppendP(sbuff, sptr);
				sb.AppendC(UTF8STRC("\r\n"));
				sb.AppendC(UTF8STRC("Dest MAC: "));
				WriteMUInt64(sbuff, mac->packetDestMAC[(cnt + i) & 15]);
				sb.AppendHexBuff(&sbuff[2], 6, ':', Text::LineBreakType::None);
				Net::PacketAnalyzerEthernet::PacketEthernetDataGetDetail(mac->packetEtherType[(cnt + i) & 15], mac->packetData[(cnt + i) & 15], mac->packetSize[(cnt + i) & 15], &sb);
				sb.AppendC(UTF8STRC("\r\n"));
				sb.AppendC(UTF8STRC("\r\n"));
				i++;
			}
		}
	}
	me->txtDevice->SetText(sb.ToCString());
}

SSWR::AVIRead::AVIRRAWMonitorForm::AVIRRAWMonitorForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Net::EthernetAnalyzer *analyzer) : UI::GUIForm(parent, 1024, 768, ui), whois(core->GetSocketFactory())
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("RAW Monitor"));

	this->core = core;
	this->sockf = core->GetSocketFactory();
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->listener = 0;
	this->webHdlr = 0;
	this->adapterIP = 0;
	this->adapterChanged = false;
	this->dataUpdated = true;
	if (analyzer)
	{
		this->analyzer = analyzer;
	}
	else
	{
		NEW_CLASS(this->analyzer, Net::EthernetAnalyzer(0, Net::EthernetAnalyzer::AT_ALL, CSTR("RAWMonitor")));
	}
	this->ipTranCnt = 0;
	this->pingIPListUpdated = false;
	this->pingIPContUpdated = false;
	this->currPingIP = 0;
	this->analyzer->HandlePingv4Request(OnPingPacket, this);
	this->tcp4synLastIndex = 0;

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
	this->cboIP->HandleSelectionChange(OnIPSelChg, this);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this->pnlControl, CSTR("Start")));
	this->btnStart->SetRect(254, 28, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpDevice = this->tcMain->AddTabPage(CSTR("Device"));
	NEW_CLASS(this->txtDevice, UI::GUITextBox(ui, this->tpDevice, CSTR(""), true));
	this->txtDevice->SetReadOnly(true);
	this->txtDevice->SetRect(0, 0, 100, 300, false);
	this->txtDevice->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->vspDevice, UI::GUIVSplitter(ui, this->tpDevice, 3, true));
	NEW_CLASS(this->lvDevice, UI::GUIListView(ui, this->tpDevice, UI::GUIListView::LVSTYLE_TABLE, 10));
	this->lvDevice->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDevice->SetFullRowSelect(true);
	this->lvDevice->SetShowGrid(true);
	this->lvDevice->AddColumn(CSTR("MAC"), 115);
	this->lvDevice->AddColumn(CSTR("Vendor"), 200);
	this->lvDevice->AddColumn(CSTR("IPv4 Src"), 50);
	this->lvDevice->AddColumn(CSTR("IPv4 Dest"), 50);
	this->lvDevice->AddColumn(CSTR("IPv6 Src"), 50);
	this->lvDevice->AddColumn(CSTR("IPv6 Dest"), 50);
	this->lvDevice->AddColumn(CSTR("Other Src"), 50);
	this->lvDevice->AddColumn(CSTR("Other Dest"), 50);
	this->lvDevice->AddColumn(CSTR("Name"), 120);
	this->lvDevice->AddColumn(CSTR("IP List"), 280);
	this->lvDevice->HandleSelChg(OnDeviceSelChg, this);

	this->tpIPTran = this->tcMain->AddTabPage(CSTR("IP Tran"));
	NEW_CLASS(this->lbIPTran, UI::GUIListBox(ui, this->tpIPTran, false));
	this->lbIPTran->SetRect(0, 0, 150, 23, false);
	this->lbIPTran->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbIPTran->HandleSelectionChange(OnIPTranSelChg, this);
	NEW_CLASS(this->hspIPTran, UI::GUIHSplitter(ui, this->tpIPTran, 3, false));
	NEW_CLASS(this->tcIPTran, UI::GUITabControl(ui, this->tpIPTran));
	this->tcIPTran->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpIPTranInfo = this->tcIPTran->AddTabPage(CSTR("Info"));
	NEW_CLASS(this->lvIPTranInfo, UI::GUIListView(ui, this->tpIPTranInfo, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvIPTranInfo->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvIPTranInfo->SetFullRowSelect(true);
	this->lvIPTranInfo->SetShowGrid(true);
	this->lvIPTranInfo->AddColumn(CSTR("Name"), 200);
	this->lvIPTranInfo->AddColumn(CSTR("Value"), 100);
	this->lvIPTranInfo->AddItem(CSTR("Recv TCP Cnt"), 0);
	this->lvIPTranInfo->AddItem(CSTR("Send TCP Cnt"), 0);
	this->lvIPTranInfo->AddItem(CSTR("Recv TCP Size"), 0);
	this->lvIPTranInfo->AddItem(CSTR("Send TCP Size"), 0);
	this->lvIPTranInfo->AddItem(CSTR("Recv UDP Cnt"), 0);
	this->lvIPTranInfo->AddItem(CSTR("Send UDP Cnt"), 0);
	this->lvIPTranInfo->AddItem(CSTR("Recv UDP Size"), 0);
	this->lvIPTranInfo->AddItem(CSTR("Send UDP Size"), 0);
	this->lvIPTranInfo->AddItem(CSTR("Recv ICMP Cnt"), 0);
	this->lvIPTranInfo->AddItem(CSTR("Send ICMP Cnt"), 0);
	this->lvIPTranInfo->AddItem(CSTR("Recv ICMP Size"), 0);
	this->lvIPTranInfo->AddItem(CSTR("Send ICMP Size"), 0);
	this->lvIPTranInfo->AddItem(CSTR("Recv Other Cnt"), 0);
	this->lvIPTranInfo->AddItem(CSTR("Send Other Cnt"), 0);
	this->lvIPTranInfo->AddItem(CSTR("Recv Other Size"), 0);
	this->lvIPTranInfo->AddItem(CSTR("Send Other Size"), 0);
	this->tpIPTranWhois = this->tcIPTran->AddTabPage(CSTR("Whois"));
	NEW_CLASS(this->txtIPTranWhois, UI::GUITextBox(ui, this->tpIPTranWhois, CSTR(""), true));
	this->txtIPTranWhois->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtIPTranWhois->SetReadOnly(true);

	this->tpDNSReqv4 = this->tcMain->AddTabPage(CSTR("DNS Req v4"));
	NEW_CLASS(this->lbDNSReqv4, UI::GUIListBox(ui, this->tpDNSReqv4, false));
	this->lbDNSReqv4->SetRect(0, 0, 250, 23, false);
	this->lbDNSReqv4->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbDNSReqv4->HandleSelectionChange(OnDNSReqv4SelChg, this);
	NEW_CLASS(this->hspDNSReqv4, UI::GUIHSplitter(ui, this->tpDNSReqv4, 3, false));
	NEW_CLASS(this->pnlDNSReqv4, UI::GUIPanel(ui, this->tpDNSReqv4));
	this->pnlDNSReqv4->SetRect(0, 0, 100, 79, false);
	this->pnlDNSReqv4->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblDNSReqv4Name, UI::GUILabel(ui, this->pnlDNSReqv4, CSTR("Req Name")));
	this->lblDNSReqv4Name->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtDNSReqv4Name, UI::GUITextBox(ui, this->pnlDNSReqv4, CSTR("")));
	this->txtDNSReqv4Name->SetRect(104, 4, 250, 23, false);
	this->txtDNSReqv4Name->SetReadOnly(true);
	NEW_CLASS(this->lblDNSReqv4ReqTime, UI::GUILabel(ui, this->pnlDNSReqv4, CSTR("Req Time")));
	this->lblDNSReqv4ReqTime->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtDNSReqv4ReqTime, UI::GUITextBox(ui, this->pnlDNSReqv4, CSTR("")));
	this->txtDNSReqv4ReqTime->SetRect(104, 28, 150, 23, false);
	this->txtDNSReqv4ReqTime->SetReadOnly(true);
	NEW_CLASS(this->lblDNSReqv4TTL, UI::GUILabel(ui, this->pnlDNSReqv4, CSTR("TTL")));
	this->lblDNSReqv4TTL->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtDNSReqv4TTL, UI::GUITextBox(ui, this->pnlDNSReqv4, CSTR("")));
	this->txtDNSReqv4TTL->SetRect(104, 52, 80, 23, false);
	this->txtDNSReqv4TTL->SetReadOnly(true);
	NEW_CLASS(this->lvDNSReqv4, UI::GUIListView(ui, this->tpDNSReqv4, UI::GUIListView::LVSTYLE_TABLE, 5));
	this->lvDNSReqv4->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDNSReqv4->SetFullRowSelect(true);
	this->lvDNSReqv4->SetShowGrid(true);
	this->lvDNSReqv4->AddColumn(CSTR("Name"), 250);
	this->lvDNSReqv4->AddColumn(CSTR("Type"), 40);
	this->lvDNSReqv4->AddColumn(CSTR("Class"), 40);
	this->lvDNSReqv4->AddColumn(CSTR("TTL"), 60);
	this->lvDNSReqv4->AddColumn(CSTR("RD"), 250);

	this->tpDNSReqv6 = this->tcMain->AddTabPage(CSTR("DNS Req v6"));
	NEW_CLASS(this->lbDNSReqv6, UI::GUIListBox(ui, this->tpDNSReqv6, false));
	this->lbDNSReqv6->SetRect(0, 0, 250, 23, false);
	this->lbDNSReqv6->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbDNSReqv6->HandleSelectionChange(OnDNSReqv6SelChg, this);
	NEW_CLASS(this->hspDNSReqv6, UI::GUIHSplitter(ui, this->tpDNSReqv6, 3, false));
	NEW_CLASS(this->pnlDNSReqv6, UI::GUIPanel(ui, this->tpDNSReqv6));
	this->pnlDNSReqv6->SetRect(0, 0, 100, 79, false);
	this->pnlDNSReqv6->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblDNSReqv6Name, UI::GUILabel(ui, this->pnlDNSReqv6, CSTR("Req Name")));
	this->lblDNSReqv6Name->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtDNSReqv6Name, UI::GUITextBox(ui, this->pnlDNSReqv6, CSTR("")));
	this->txtDNSReqv6Name->SetRect(104, 4, 250, 23, false);
	this->txtDNSReqv6Name->SetReadOnly(true);
	NEW_CLASS(this->lblDNSReqv6ReqTime, UI::GUILabel(ui, this->pnlDNSReqv6, CSTR("Req Time")));
	this->lblDNSReqv6ReqTime->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtDNSReqv6ReqTime, UI::GUITextBox(ui, this->pnlDNSReqv6, CSTR("")));
	this->txtDNSReqv6ReqTime->SetRect(104, 28, 150, 23, false);
	this->txtDNSReqv6ReqTime->SetReadOnly(true);
	NEW_CLASS(this->lblDNSReqv6TTL, UI::GUILabel(ui, this->pnlDNSReqv6, CSTR("TTL")));
	this->lblDNSReqv6TTL->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtDNSReqv6TTL, UI::GUITextBox(ui, this->pnlDNSReqv6, CSTR("")));
	this->txtDNSReqv6TTL->SetRect(104, 52, 80, 23, false);
	this->txtDNSReqv6TTL->SetReadOnly(true);
	NEW_CLASS(this->lvDNSReqv6, UI::GUIListView(ui, this->tpDNSReqv6, UI::GUIListView::LVSTYLE_TABLE, 5));
	this->lvDNSReqv6->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDNSReqv6->SetFullRowSelect(true);
	this->lvDNSReqv6->SetShowGrid(true);
	this->lvDNSReqv6->AddColumn(CSTR("Name"), 250);
	this->lvDNSReqv6->AddColumn(CSTR("Type"), 40);
	this->lvDNSReqv6->AddColumn(CSTR("Class"), 40);
	this->lvDNSReqv6->AddColumn(CSTR("TTL"), 60);
	this->lvDNSReqv6->AddColumn(CSTR("RD"), 250);

	this->tpDNSReqOth = this->tcMain->AddTabPage(CSTR("DNS Req Other"));
	NEW_CLASS(this->lbDNSReqOth, UI::GUIListBox(ui, this->tpDNSReqOth, false));
	this->lbDNSReqOth->SetRect(0, 0, 250, 23, false);
	this->lbDNSReqOth->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbDNSReqOth->HandleSelectionChange(OnDNSReqOthSelChg, this);
	NEW_CLASS(this->hspDNSReqOth, UI::GUIHSplitter(ui, this->tpDNSReqOth, 3, false));
	NEW_CLASS(this->pnlDNSReqOth, UI::GUIPanel(ui, this->tpDNSReqOth));
	this->pnlDNSReqOth->SetRect(0, 0, 100, 79, false);
	this->pnlDNSReqOth->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblDNSReqOthName, UI::GUILabel(ui, this->pnlDNSReqOth, CSTR("Req Name")));
	this->lblDNSReqOthName->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtDNSReqOthName, UI::GUITextBox(ui, this->pnlDNSReqOth, CSTR("")));
	this->txtDNSReqOthName->SetRect(104, 4, 250, 23, false);
	this->txtDNSReqOthName->SetReadOnly(true);
	NEW_CLASS(this->lblDNSReqOthReqTime, UI::GUILabel(ui, this->pnlDNSReqOth, CSTR("Req Time")));
	this->lblDNSReqOthReqTime->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtDNSReqOthReqTime, UI::GUITextBox(ui, this->pnlDNSReqOth, CSTR("")));
	this->txtDNSReqOthReqTime->SetRect(104, 28, 150, 23, false);
	this->txtDNSReqOthReqTime->SetReadOnly(true);
	NEW_CLASS(this->lblDNSReqOthTTL, UI::GUILabel(ui, this->pnlDNSReqOth, CSTR("TTL")));
	this->lblDNSReqOthTTL->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtDNSReqOthTTL, UI::GUITextBox(ui, this->pnlDNSReqOth, CSTR("")));
	this->txtDNSReqOthTTL->SetRect(104, 52, 80, 23, false);
	this->txtDNSReqOthTTL->SetReadOnly(true);
	NEW_CLASS(this->lvDNSReqOth, UI::GUIListView(ui, this->tpDNSReqOth, UI::GUIListView::LVSTYLE_TABLE, 5));
	this->lvDNSReqOth->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDNSReqOth->SetFullRowSelect(true);
	this->lvDNSReqOth->SetShowGrid(true);
	this->lvDNSReqOth->AddColumn(CSTR("Name"), 250);
	this->lvDNSReqOth->AddColumn(CSTR("Type"), 40);
	this->lvDNSReqOth->AddColumn(CSTR("Class"), 40);
	this->lvDNSReqOth->AddColumn(CSTR("TTL"), 60);
	this->lvDNSReqOth->AddColumn(CSTR("RD"), 250);

	this->tpDNSTarget = this->tcMain->AddTabPage(CSTR("DNS Target"));
	NEW_CLASS(this->lbDNSTarget, UI::GUIListBox(ui, this->tpDNSTarget, false));
	this->lbDNSTarget->SetRect(0, 0, 150, 23, false);
	this->lbDNSTarget->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbDNSTarget->HandleSelectionChange(OnDNSTargetSelChg, this);
	NEW_CLASS(this->hspDNSTarget, UI::GUIHSplitter(ui, this->tpDNSTarget, 3, false));
	NEW_CLASS(this->tcDNSTarget, UI::GUITabControl(ui, this->tpDNSTarget));
	this->tcDNSTarget->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpDNSTargetDomain = this->tcDNSTarget->AddTabPage(CSTR("Domains"));
	NEW_CLASS(this->lbDNSTargetDomain, UI::GUIListBox(ui, this->tpDNSTargetDomain, false));
	this->lbDNSTargetDomain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpDNSTargetWhois = this->tcDNSTarget->AddTabPage(CSTR("Whois"));
	NEW_CLASS(this->txtDNSTargetWhois, UI::GUITextBox(ui, this->tpDNSTargetWhois, CSTR(""), true));
	this->txtDNSTargetWhois->SetReadOnly(true);
	this->txtDNSTargetWhois->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpMDNS = this->tcMain->AddTabPage(CSTR("MDNS"));
	NEW_CLASS(this->lbMDNS, UI::GUIListBox(ui, this->tpMDNS, false));
	this->lbMDNS->SetRect(0, 0, 200, 23, false);
	this->lbMDNS->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbMDNS->HandleSelectionChange(OnMDNSSelChg, this);
	NEW_CLASS(this->hspMDNS, UI::GUIHSplitter(ui, this->tpMDNS, 3, false));
	NEW_CLASS(this->pnlMDNS, UI::GUIPanel(ui, this->tpMDNS));
	this->pnlMDNS->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->lblMDNSName, UI::GUILabel(ui, this->pnlMDNS, CSTR("Name")));
	this->lblMDNSName->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtMDNSName, UI::GUITextBox(ui, this->pnlMDNS, CSTR("")));
	this->txtMDNSName->SetRect(104, 4, 400, 23, false);
	this->txtMDNSName->SetReadOnly(true);
	NEW_CLASS(this->lblMDNSType, UI::GUILabel(ui, this->pnlMDNS, CSTR("Type")));
	this->lblMDNSType->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtMDNSType, UI::GUITextBox(ui, this->pnlMDNS, CSTR("")));
	this->txtMDNSType->SetRect(104, 28, 200, 23, false);
	this->txtMDNSType->SetReadOnly(true);
	NEW_CLASS(this->lblMDNSClass, UI::GUILabel(ui, this->pnlMDNS, CSTR("Class")));
	this->lblMDNSClass->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtMDNSClass, UI::GUITextBox(ui, this->pnlMDNS, CSTR("")));
	this->txtMDNSClass->SetRect(104, 52, 200, 23, false);
	this->txtMDNSClass->SetReadOnly(true);
	NEW_CLASS(this->lblMDNSTTL, UI::GUILabel(ui, this->pnlMDNS, CSTR("TTL")));
	this->lblMDNSTTL->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtMDNSTTL, UI::GUITextBox(ui, this->pnlMDNS, CSTR("")));
	this->txtMDNSTTL->SetRect(104, 76, 100, 23, false);
	this->txtMDNSTTL->SetReadOnly(true);
	NEW_CLASS(this->lblMDNSResult, UI::GUILabel(ui, this->pnlMDNS, CSTR("Result")));
	this->lblMDNSResult->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtMDNSResult, UI::GUITextBox(ui, this->pnlMDNS, CSTR("")));
	this->txtMDNSResult->SetRect(104, 100, 400, 23, false);
	this->txtMDNSResult->SetReadOnly(true);

	this->tpDNSClient = this->tcMain->AddTabPage(CSTR("DNS Client"));
	NEW_CLASS(this->lbDNSClient, UI::GUIListBox(ui, this->tpDNSClient, false));
	this->lbDNSClient->SetRect(0, 0, 150, 23, false);
	this->lbDNSClient->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbDNSClient->HandleSelectionChange(OnDNSClientSelChg, this);
	NEW_CLASS(this->hspDNSClient, UI::GUIHSplitter(ui, this->tpDNSClient, 3, false));
	NEW_CLASS(this->lvDNSClient, UI::GUIListView(ui, this->tpDNSClient, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvDNSClient->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDNSClient->SetFullRowSelect(true);
	this->lvDNSClient->SetShowGrid(true);
	this->lvDNSClient->AddColumn(CSTR("Time"), 120);
	this->lvDNSClient->AddColumn(CSTR("Count"), 100);

	this->tpDHCP = this->tcMain->AddTabPage(CSTR("DHCP"));
	NEW_CLASS(this->lvDHCP, UI::GUIListView(ui, this->tpDHCP, UI::GUIListView::LVSTYLE_TABLE, 14));
	this->lvDHCP->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDHCP->SetFullRowSelect(true);
	this->lvDHCP->SetShowGrid(true);
	this->lvDHCP->AddColumn(CSTR("MAC"), 105);
	this->lvDHCP->AddColumn(CSTR("Vendor"), 200);
	this->lvDHCP->AddColumn(CSTR("IP Addr"), 100);
	this->lvDHCP->AddColumn(CSTR("Subnet"), 100);
	this->lvDHCP->AddColumn(CSTR("Gateway"), 100);
	this->lvDHCP->AddColumn(CSTR("DNS"), 200);
	this->lvDHCP->AddColumn(CSTR("DHCP Server"), 100);
	this->lvDHCP->AddColumn(CSTR("Router"), 100);
	this->lvDHCP->AddColumn(CSTR("IP Time"), 140);
	this->lvDHCP->AddColumn(CSTR("Lease Time"), 50);
	this->lvDHCP->AddColumn(CSTR("Renew Time"), 50);
	this->lvDHCP->AddColumn(CSTR("Rebind Time"), 50);
	this->lvDHCP->AddColumn(CSTR("Host Name"), 100);
	this->lvDHCP->AddColumn(CSTR("Vendor Class"), 100);

	this->tpIPLog = this->tcMain->AddTabPage(CSTR("IP Log"));
	NEW_CLASS(this->lbIPLog, UI::GUIListBox(ui, this->tpIPLog, false));
	this->lbIPLog->SetRect(0, 0, 150, 23, false);
	this->lbIPLog->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbIPLog->HandleSelectionChange(OnIPLogSelChg, this);
	NEW_CLASS(this->hspIPLog, UI::GUIHSplitter(ui, this->tpIPLog, 3, false));
	NEW_CLASS(this->lbIPLogVal, UI::GUIListBox(ui, this->tpIPLog, false));
	this->lbIPLogVal->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpPingIP = this->tcMain->AddTabPage(CSTR("Ping IP"));
	NEW_CLASS(this->lbPingIP, UI::GUIListBox(ui, this->tpPingIP, false));
	this->lbPingIP->SetRect(0, 0, 150, 23, false);
	this->lbPingIP->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbPingIP->HandleSelectionChange(OnPingIPSelChg, this);
	NEW_CLASS(this->hspPingIP, UI::GUIHSplitter(ui, this->tpPingIP, 3, false));
	NEW_CLASS(this->tcPingIP, UI::GUITabControl(ui, this->tpPingIP));
	this->tcPingIP->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpPingIPInfo = this->tcPingIP->AddTabPage(CSTR("Info"));
	NEW_CLASS(this->lblPingIPCount, UI::GUILabel(ui, this->tpPingIPInfo, CSTR("Count")));
	this->lblPingIPCount->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtPingIPCount, UI::GUITextBox(ui, this->tpPingIPInfo, CSTR("0")));
	this->txtPingIPCount->SetRect(104, 4, 100, 23, false);
	this->txtPingIPCount->SetReadOnly(true);
	NEW_CLASS(this->lblPingIPName, UI::GUILabel(ui, this->tpPingIPInfo, CSTR("Name")));
	this->lblPingIPName->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtPingIPName, UI::GUITextBox(ui, this->tpPingIPInfo, CSTR("")));
	this->txtPingIPName->SetRect(104, 28, 200, 23, false);
	this->txtPingIPName->SetReadOnly(true);
	NEW_CLASS(this->lblPingIPCountry, UI::GUILabel(ui, this->tpPingIPInfo, CSTR("Country")));
	this->lblPingIPCountry->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtPingIPCountry, UI::GUITextBox(ui, this->tpPingIPInfo, CSTR("")));
	this->txtPingIPCountry->SetRect(104, 52, 100, 23, false);
	this->txtPingIPCountry->SetReadOnly(true);
	this->tpPingIPWhois = this->tcPingIP->AddTabPage(CSTR("Whois"));
	NEW_CLASS(this->txtPingIPWhois, UI::GUITextBox(ui, this->tpPingIPWhois, CSTR(""), true));
	this->txtPingIPWhois->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtPingIPWhois->SetReadOnly(true);

	this->tpTCP4SYN = this->tcMain->AddTabPage(CSTR("TCPv4 SYN"));
	NEW_CLASS(this->lvTCP4SYN, UI::GUIListView(ui, this->tpTCP4SYN, UI::GUIListView::LVSTYLE_TABLE, 5));
	this->lvTCP4SYN->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvTCP4SYN->SetFullRowSelect(true);
	this->lvTCP4SYN->SetShowGrid(true);
	this->lvTCP4SYN->AddColumn(CSTR("Time"), 180);
	this->lvTCP4SYN->AddColumn(CSTR("Source IP"), 100);
	this->lvTCP4SYN->AddColumn(CSTR("Port"), 50);
	this->lvTCP4SYN->AddColumn(CSTR("Dest IP"), 100);
	this->lvTCP4SYN->AddColumn(CSTR("Port"), 50);

	this->tpLog = this->tcMain->AddTabPage(CSTR("Log"));
	NEW_CLASS(this->txtLog, UI::GUITextBox(ui, this->tpLog, CSTR("")));
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lbLog, UI::GUIListBox(ui, this->tpLog, false));
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSelChg, this);

	NEW_CLASS(this->logger, UI::ListBoxLogger(this, this->lbLog, 500, true));
	this->log.AddLogHandler(this->logger, IO::ILogHandler::LogLevel::Raw);

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
		DEL_CLASS(connInfo);
		i++;
	}
	if (this->cboIP->GetCount() > 0)
	{
		this->adapterIP = (UInt32)(OSInt)this->cboIP->GetItem(0);
		this->cboIP->SetSelectedIndex(0);
		this->adapterChanged = true;
	}
	this->socMon = 0;
	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRRAWMonitorForm::~AVIRRAWMonitorForm()
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

	PingIPInfo *pingIPInfo;
	UOSInt i;
	i = this->pingIPMap.GetCount();
	while (i-- > 0)
	{
		pingIPInfo = this->pingIPMap.GetItem(i);
		SDEL_STRING(pingIPInfo->name);
		SDEL_STRING(pingIPInfo->country);
		MemFree(pingIPInfo);
	}

	IPTranInfo *ipTran;
	i = this->ipTranMap.GetCount();
	while (i-- > 0)
	{
		ipTran = this->ipTranMap.GetItem(i);
		MemFree(ipTran);
	}
	DEL_CLASS(this->analyzer);
}

void SSWR::AVIRead::AVIRRAWMonitorForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
