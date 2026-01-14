#include "Stdafx.h"
#include "Core/ByteTool_C.h"
#include "IO/Path.h"
#include "IO/PcapngWriter.h"
#include "IO/PcapWriter.h"
#include "Manage/HiResClock.h"
#include "Net/ConnectionInfo.h"
#include "Net/MACInfo.h"
#include "Net/PacketAnalyzerEthernet.h"
#include "SSWR/AVIRead/AVIRRAWMonitorForm.h"
#include "Sync/MutexUsage.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/Clipboard.h"

void __stdcall SSWR::AVIRead::AVIRRAWMonitorForm::OnPingPacket(AnyType userData, UInt32 srcIP, UInt32 destIP, UInt8 ttl, UOSInt packetSize)
{
	NN<SSWR::AVIRead::AVIRRAWMonitorForm> me = userData.GetNN<SSWR::AVIRead::AVIRRAWMonitorForm>();
	Text::StringBuilderUTF8 sb;
	UInt32 sortableIP = Net::SocketUtil::IPv4ToSortable(srcIP);
	NN<PingIPInfo> pingIPInfo;
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	Sync::MutexUsage mutUsage(me->pingIPMut);
	if (!me->pingIPMap.Get(sortableIP).SetTo(pingIPInfo))
	{
		NN<Net::WhoisRecord> rec;
		pingIPInfo = MemAllocNN(PingIPInfo);
		pingIPInfo->ip = srcIP;
		pingIPInfo->count = 0;
		rec = me->whois.RequestIP(srcIP);
		if (rec->GetNetworkName(sbuff).SetTo(sptr))
		{
			pingIPInfo->name = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
		}
		else
		{
			pingIPInfo->name = Text::String::New(UTF8STRC("Unknown"));
		}
		if (rec->GetCountryCode(sbuff).SetTo(sptr))
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
	if (me->currPingIP == pingIPInfo.Ptr())
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
	me->log.LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Command);
}

void __stdcall SSWR::AVIRead::AVIRRAWMonitorForm::OnRAWData(AnyType userData, UnsafeArray<const UInt8> rawData, UOSInt packetSize)
{
	NN<SSWR::AVIRead::AVIRRAWMonitorForm> me = userData.GetNN<SSWR::AVIRead::AVIRRAWMonitorForm>();
	{
		NN<IO::PacketLogWriter> plogWriter;
		Sync::MutexUsage mutUsage(me->plogMut);
		if (me->plogWriter.SetTo(plogWriter))
		{
			plogWriter->WritePacket(Data::ByteArrayR(rawData, packetSize));
		}
	}
	me->analyzer->PacketEthernet(rawData, packetSize);
}

void __stdcall SSWR::AVIRead::AVIRRAWMonitorForm::OnIPv4Data(AnyType userData, UnsafeArray<const UInt8> rawData, UOSInt packetSize)
{
	NN<SSWR::AVIRead::AVIRRAWMonitorForm> me = userData.GetNN<SSWR::AVIRead::AVIRRAWMonitorForm>();
	{
		NN<IO::PacketLogWriter> plogWriter;
		Sync::MutexUsage mutUsage(me->plogMut);
		if (me->plogWriter.SetTo(plogWriter))
		{
			plogWriter->WritePacket(Data::ByteArrayR(rawData, packetSize));
		}
	}
	me->analyzer->PacketIPv4_2(rawData, packetSize, 0, 0);
}

void __stdcall SSWR::AVIRead::AVIRRAWMonitorForm::OnInfoClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRRAWMonitorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRAWMonitorForm>();
	if (me->listener.NotNull())
	{
		me->listener.Delete();
		me->webHdlr.Delete();
		me->txtInfo->SetReadOnly(false);
		return;
	}
	UInt16 port;
	Text::StringBuilderUTF8 sb;
	me->txtInfo->GetText(sb);
	if (!sb.ToUInt16(port))
	{
		me->ui->ShowMsgOK(CSTR("Info port is not valid"), CSTR("RAW Monitor"), me);
		return;
	}
	NN<Net::EthernetWebHandler> webHdlr;
	NN<Net::WebServer::WebListener> listener;
	NEW_CLASSNN(webHdlr, Net::EthernetWebHandler(me->analyzer));
	NEW_CLASSNN(listener, Net::WebServer::WebListener(me->clif, nullptr, webHdlr, port, 60, 1, 3, CSTR("RAWMonitor/1.0"), false, Net::WebServer::KeepAlive::Default, true));
	if (listener->IsError())
	{
		listener.Delete();
		webHdlr.Delete();
		me->ui->ShowMsgOK(CSTR("Error in listening to info port"), CSTR("RAW Monitor"), me);
		return;
	}
	me->webHdlr = webHdlr;
	me->listener = listener;
	me->txtInfo->SetReadOnly(true);
}

void __stdcall SSWR::AVIRead::AVIRRAWMonitorForm::OnStartClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRRAWMonitorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRAWMonitorForm>();
	if (me->socMon.NotNull())
	{
		me->socMon.Delete();
		me->cboIP->SetEnabled(true);
		OnPLogClicked(me);
		return;
	}

	UInt32 ip = (UInt32)me->cboIP->GetSelectedItem().GetOSInt();
	if (ip)
	{
		NN<Socket> soc;
		
		if (me->clif->GetSocketFactory()->CreateRAWSocket().SetTo(soc))
		{
			me->linkType = IO::PacketAnalyse::LinkType::Ethernet;
			NEW_CLASSOPT(me->socMon, Net::SocketMonitor(me->clif->GetSocketFactory(), soc, OnRAWData, me, 3));
			me->cboIP->SetEnabled(false);
		}
		else if (me->clif->GetSocketFactory()->CreateRAWIPv4Socket(ip).SetTo(soc))
		{
			me->linkType = IO::PacketAnalyse::LinkType::Linux;
			NEW_CLASSOPT(me->socMon, Net::SocketMonitor(me->clif->GetSocketFactory(), soc, OnIPv4Data, me, 3));
			me->cboIP->SetEnabled(false);
		}
		else
		{
			me->ui->ShowMsgOK(CSTR("Error in listening to socket"), CSTR("Error"), me);
			return;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRRAWMonitorForm::OnPLogClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRRAWMonitorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRAWMonitorForm>();
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	if (me->plogWriter.NotNull())
	{
		Sync::MutexUsage mutUsage(me->plogMut);
		me->plogWriter.Delete();
		me->txtPLog->SetText(CSTR(""));
		me->btnPLog->SetText(CSTR("Begin Log"));
		return;
	}
	if (me->socMon.IsNull())
	{
		return;
	}
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("RAW"));
	sptr2 = sptr - 3;
	sptr = Text::StrInt64(sptr, Data::DateTimeUtil::GetCurrTimeMillis());
	NN<IO::PacketLogWriter> plogWriter;
	Sync::MutexUsage mutUsage(me->plogMut);
	if (me->cboPLog->GetSelectedIndex() == 1)
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC(".pcapng"));
		NEW_CLASSNN(plogWriter, IO::PcapngWriter(CSTRP(sbuff, sptr), me->linkType, CSTR("AVIRead")));
	}
	else
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC(".pcap"));
		NEW_CLASSNN(plogWriter, IO::PcapWriter(CSTRP(sbuff, sptr), me->linkType));
	}
	if (plogWriter->IsError())
	{
		plogWriter.Delete();
	}
	else
	{
		me->plogWriter = plogWriter;
		me->txtPLog->SetText(CSTRP(sptr2, sptr));
		me->btnPLog->SetText(CSTR("End Log"));
	}
}

void __stdcall SSWR::AVIRead::AVIRRAWMonitorForm::OnIPSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRRAWMonitorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRAWMonitorForm>();
	UInt32 ip = (UInt32)me->cboIP->GetSelectedItem().GetOSInt();
	if (ip)
	{
		me->adapterIP = ip;
		me->adapterChanged = true;
	}
}

void __stdcall SSWR::AVIRead::AVIRRAWMonitorForm::OnIPTranSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRRAWMonitorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRAWMonitorForm>();
	IPTranInfo *ipTran = (IPTranInfo*)me->lbIPTran->GetSelectedItem().p;
	if (ipTran)
	{
		Text::StringBuilderUTF8 sb;
		NN<Net::WhoisRecord> rec = me->whois.RequestIP(ipTran->ip);
		sb.AppendJoin(rec->Iterator(), CSTR("\r\n"));
		me->txtIPTranWhois->SetText(sb.ToCString());
		me->dataUpdated = true;
	}
	else
	{
		me->txtIPTranWhois->SetText(CSTR(""));
	}
}

void __stdcall SSWR::AVIRead::AVIRRAWMonitorForm::OnLogSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRRAWMonitorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRAWMonitorForm>();
	NN<Text::String> s;
	if (me->lbLog->GetSelectedItemTextNew().SetTo(s))
	{
		me->txtLog->SetText(s->ToCString());
		s->Release();
	}
}

void __stdcall SSWR::AVIRead::AVIRRAWMonitorForm::OnPingIPSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRRAWMonitorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRAWMonitorForm>();
	me->currPingIP = (PingIPInfo*)me->lbPingIP->GetSelectedItem().p;
	me->pingIPContUpdated = false;
	NN<PingIPInfo> currPingIP;
	if (me->currPingIP.SetTo(currPingIP))
	{
		UTF8Char sbuff[32];
		UnsafeArray<UTF8Char> sptr;
		sptr = Text::StrInt64(sbuff, currPingIP->count);
		me->txtPingIPCount->SetText(CSTRP(sbuff, sptr));
		me->txtPingIPName->SetText(currPingIP->name->ToCString());
		me->txtPingIPCountry->SetText(currPingIP->country->ToCString());

		Text::StringBuilderUTF8 sb;
		NN<Net::WhoisRecord> rec = me->whois.RequestIP(currPingIP->ip);
		sb.AppendJoin(rec->Iterator(), CSTR("\r\n"));
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

void __stdcall SSWR::AVIRead::AVIRRAWMonitorForm::OnDNSReqv4SelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRRAWMonitorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRAWMonitorForm>();
	Text::String *name = (Text::String*)me->lbDNSReqv4->GetSelectedItem().p;
	if (name)
	{
		UTF8Char sbuff[64];
		UnsafeArray<UTF8Char> sptr;
		Data::DateTime reqTime;
		UInt32 ttl;
		Data::ArrayListNN<Net::DNSClient::RequestAnswer> ansList;
		NN<Net::DNSClient::RequestAnswer> ans;
		if (me->analyzer->DNSReqv4GetInfo(name->ToCString(), ansList, reqTime, ttl))
		{
			UOSInt i;
			UOSInt j;
			me->txtDNSReqv4Name->SetText(name->ToCString());
			reqTime.ToLocalTime();
			sptr = reqTime.ToStringNoZone(sbuff);
			me->txtDNSReqv4ReqTime->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrUInt32(sbuff, ttl);
			me->txtDNSReqv4TTL->SetText(CSTRP(sbuff, sptr));
			me->lvDNSReqv4->ClearItems();
			i = 0;
			j = ansList.GetCount();
			while (i < j)
			{
				ans = ansList.GetItemNoCheck(i);
				me->lvDNSReqv4->AddItem(ans->name, ans);
				sptr = Text::StrInt32(sbuff, ans->recType);
				me->lvDNSReqv4->SetSubItem(i, 1, CSTRP(sbuff, sptr));
				sptr = Text::StrInt32(sbuff, ans->recClass);
				me->lvDNSReqv4->SetSubItem(i, 2, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt32(sbuff, ans->ttl);
				me->lvDNSReqv4->SetSubItem(i, 3, CSTRP(sbuff, sptr));
				me->lvDNSReqv4->SetSubItem(i, 4, Text::String::OrEmpty(ans->rd));
				i++;
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRRAWMonitorForm::OnDNSReqv6SelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRRAWMonitorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRAWMonitorForm>();
	Text::String *name = (Text::String*)me->lbDNSReqv6->GetSelectedItem().p;
	if (name)
	{
		UTF8Char sbuff[64];
		UnsafeArray<UTF8Char> sptr;
		Data::DateTime reqTime;
		UInt32 ttl;
		Data::ArrayListNN<Net::DNSClient::RequestAnswer> ansList;
		NN<Net::DNSClient::RequestAnswer> ans;
		if (me->analyzer->DNSReqv6GetInfo(name->ToCString(), ansList, reqTime, ttl))
		{
			UOSInt i;
			UOSInt j;
			me->txtDNSReqv6Name->SetText(name->ToCString());
			reqTime.ToLocalTime();
			sptr = reqTime.ToStringNoZone(sbuff);
			me->txtDNSReqv6ReqTime->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrUInt32(sbuff, ttl);
			me->txtDNSReqv6TTL->SetText(CSTRP(sbuff, sptr));
			me->lvDNSReqv6->ClearItems();
			i = 0;
			j = ansList.GetCount();
			while (i < j)
			{
				ans = ansList.GetItemNoCheck(i);
				me->lvDNSReqv6->AddItem(ans->name, ans);
				sptr = Text::StrInt32(sbuff, ans->recType);
				me->lvDNSReqv6->SetSubItem(i, 1, CSTRP(sbuff, sptr));
				sptr = Text::StrInt32(sbuff, ans->recClass);
				me->lvDNSReqv6->SetSubItem(i, 2, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt32(sbuff, ans->ttl);
				me->lvDNSReqv6->SetSubItem(i, 3, CSTRP(sbuff, sptr));
				me->lvDNSReqv6->SetSubItem(i, 4, Text::String::OrEmpty(ans->rd));
				i++;
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRRAWMonitorForm::OnDNSReqOthSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRRAWMonitorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRAWMonitorForm>();
	Text::String *name = (Text::String*)me->lbDNSReqOth->GetSelectedItem().p;
	if (name)
	{
		UTF8Char sbuff[64];
		UnsafeArray<UTF8Char> sptr;
		Data::DateTime reqTime;
		UInt32 ttl;
		Data::ArrayListNN<Net::DNSClient::RequestAnswer> ansList;
		NN<Net::DNSClient::RequestAnswer> ans;
		if (me->analyzer->DNSReqOthGetInfo(name->ToCString(), ansList, reqTime, ttl))
		{
			UOSInt i;
			UOSInt j;
			me->txtDNSReqOthName->SetText(name->ToCString());
			reqTime.ToLocalTime();
			sptr = reqTime.ToStringNoZone(sbuff);
			me->txtDNSReqOthReqTime->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrUInt32(sbuff, ttl);
			me->txtDNSReqOthTTL->SetText(CSTRP(sbuff, sptr));
			me->lvDNSReqOth->ClearItems();
			i = 0;
			j = ansList.GetCount();
			while (i < j)
			{
				ans = ansList.GetItemNoCheck(i);
				me->lvDNSReqOth->AddItem(ans->name, ans);
				sptr = Text::StrInt32(sbuff, ans->recType);
				me->lvDNSReqOth->SetSubItem(i, 1, CSTRP(sbuff, sptr));
				sptr = Text::StrInt32(sbuff, ans->recClass);
				me->lvDNSReqOth->SetSubItem(i, 2, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt32(sbuff, ans->ttl);
				me->lvDNSReqOth->SetSubItem(i, 3, CSTRP(sbuff, sptr));
				me->lvDNSReqOth->SetSubItem(i, 4, Text::String::OrEmpty(ans->rd));
				i++;
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRRAWMonitorForm::OnDNSTargetSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRRAWMonitorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRAWMonitorForm>();
	Net::EthernetAnalyzer::DNSTargetInfo *target = (Net::EthernetAnalyzer::DNSTargetInfo*)me->lbDNSTarget->GetSelectedItem().p;
	me->lbDNSTargetDomain->ClearItems();
	if (target)
	{
		UOSInt i;
		UOSInt j;
		NN<Text::String> s;
		Sync::MutexUsage mutUsage(target->mut);
		i = 0;
		j = target->addrList.GetCount();
		while (i < j)
		{
			if (target->addrList.GetItem(i).SetTo(s))
				me->lbDNSTargetDomain->AddItem(s, 0);
			i++;
		}
		mutUsage.EndUse();
		NN<Net::WhoisRecord> rec = me->whois.RequestIP(target->ip);
		Text::StringBuilderUTF8 sb;
		sb.AppendJoin(rec->Iterator(), CSTR("\r\n"));
		me->txtDNSTargetWhois->SetText(sb.ToCString());
	}
	else
	{
		me->txtDNSTargetWhois->SetText(CSTR(""));
	}
}

void __stdcall SSWR::AVIRead::AVIRRAWMonitorForm::OnMDNSSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRRAWMonitorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRAWMonitorForm>();
	Net::DNSClient::RequestAnswer *ans = (Net::DNSClient::RequestAnswer*)me->lbMDNS->GetSelectedItem().p;
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	if (ans)
	{
		me->txtMDNSName->SetText(ans->name->ToCString());
		sptr = Text::StrUInt16(sbuff, ans->recType);
		Text::CStringNN typeId;
		if (Net::DNSClient::TypeGetID(ans->recType).SetTo(typeId))
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

void __stdcall SSWR::AVIRead::AVIRRAWMonitorForm::OnDNSClientSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRRAWMonitorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRAWMonitorForm>();
	Net::EthernetAnalyzer::DNSClientInfo *cli = (Net::EthernetAnalyzer::DNSClientInfo*)me->lbDNSClient->GetSelectedItem().p;
	UOSInt i;
	UOSInt j;
	NN<Net::EthernetAnalyzer::DNSCliHourInfo> hourInfo;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;	
	me->lvDNSClient->ClearItems();
	if (cli)
	{
		Sync::MutexUsage mutUsage(cli->mut);
		i = 0;
		j = cli->hourInfos.GetCount();
		while (i < j)
		{
			hourInfo = cli->hourInfos.GetItemNoCheck(i);
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

void __stdcall SSWR::AVIRead::AVIRRAWMonitorForm::OnIPLogSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRRAWMonitorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRAWMonitorForm>();
	Net::EthernetAnalyzer::IPLogInfo *ipLog = (Net::EthernetAnalyzer::IPLogInfo*)me->lbIPLog->GetSelectedItem().p;
	UOSInt i;
	UOSInt j;
	me->lbIPLogVal->ClearItems();
	if (ipLog)
	{
		Sync::MutexUsage mutUsage(ipLog->mut);
		i = 0;
		j = ipLog->logList.GetCount();
		while (i < j)
		{
			me->lbIPLogVal->AddItem(Text::String::OrEmpty(ipLog->logList.GetItem(i)), 0);
			i++;
		}
		mutUsage.EndUse();
	}
}

void __stdcall SSWR::AVIRead::AVIRRAWMonitorForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRRAWMonitorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRAWMonitorForm>();
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	NN<Text::String> s;
	NN<PingIPInfo> currPingIP;
	if (me->pingIPContUpdated && me->currPingIP.SetTo(currPingIP))
	{
		me->pingIPContUpdated = false;
		sptr = Text::StrInt64(sbuff, currPingIP->count);
		me->txtPingIPCount->SetText(CSTRP(sbuff, sptr));
	}
	if (me->pingIPListUpdated)
	{
		NN<PingIPInfo> pingIPInfo;
		UOSInt i;
		UOSInt j;
		me->pingIPListUpdated = false;
		Sync::MutexUsage mutUsage(me->pingIPMut);
		me->lbPingIP->ClearItems();
		i = 0;
		j = me->pingIPMap.GetCount();
		while (i < j)
		{
			pingIPInfo = me->pingIPMap.GetItemNoCheck(i);
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, pingIPInfo->ip);
			me->lbPingIP->AddItem(CSTRP(sbuff, sptr), pingIPInfo);
			if (pingIPInfo.Ptr() == me->currPingIP.OrNull())
			{
				me->lbPingIP->SetSelectedIndex(i);
			}
			i++;
		}
		mutUsage.EndUse();
	}
	if (me->analyzer->DNSReqv4GetCount() != me->lbDNSReqv4->GetCount())
	{
		Data::ArrayListNN<Text::String> nameList;
		Text::String *selName = (Text::String*)me->lbDNSReqv4->GetSelectedItem().p;
		UOSInt i;
		UOSInt j;
		me->lbDNSReqv4->ClearItems();
		me->analyzer->DNSReqv4GetList(nameList);
		i = 0;
		j = nameList.GetCount();
		while (i < j)
		{
			s = nameList.GetItemNoCheck(i);
			me->lbDNSReqv4->AddItem(s, s);
			if (s.Ptr() == selName)
			{
				me->lbDNSReqv4->SetSelectedIndex(i);
			}
			i++;
		}
	}
	if (me->analyzer->DNSReqv6GetCount() != me->lbDNSReqv6->GetCount())
	{
		Data::ArrayListNN<Text::String> nameList;
		Text::String *selName = (Text::String*)me->lbDNSReqv6->GetSelectedItem().p;
		UOSInt i;
		UOSInt j;
		me->lbDNSReqv6->ClearItems();
		me->analyzer->DNSReqv6GetList(nameList);
		i = 0;
		j = nameList.GetCount();
		while (i < j)
		{
			s = nameList.GetItemNoCheck(i);
			me->lbDNSReqv6->AddItem(s, s);
			if (s.Ptr() == selName)
			{
				me->lbDNSReqv6->SetSelectedIndex(i);
			}
			i++;
		}
	}
	if (me->analyzer->DNSReqOthGetCount() != me->lbDNSReqOth->GetCount())
	{
		Data::ArrayListNN<Text::String> nameList;
		Text::String *selName = (Text::String*)me->lbDNSReqOth->GetSelectedItem().p;
		UOSInt i;
		UOSInt j;
		me->lbDNSReqOth->ClearItems();
		me->analyzer->DNSReqOthGetList(nameList);
		i = 0;
		j = nameList.GetCount();
		while (i < j)
		{
			s = nameList.GetItemNoCheck(i);
			me->lbDNSReqOth->AddItem(s, s);
			if (s.Ptr() == selName)
			{
				me->lbDNSReqOth->SetSelectedIndex(i);
			}
			i++;
		}
	}
	if (me->analyzer->DNSTargetGetCount() != me->lbDNSTarget->GetCount())
	{
		Data::ArrayListNN<Net::EthernetAnalyzer::DNSTargetInfo> targetList;
		NN<Net::EthernetAnalyzer::DNSTargetInfo> target;
		Net::EthernetAnalyzer::DNSTargetInfo *currSel = (Net::EthernetAnalyzer::DNSTargetInfo*)me->lbDNSTarget->GetSelectedItem().p;
		UOSInt i;
		UOSInt j;
		me->analyzer->DNSTargetGetList(targetList);
		me->lbDNSTarget->ClearItems();
		i = 0;
		j = targetList.GetCount();
		while (i < j)
		{
			target = targetList.GetItemNoCheck(i);
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, target->ip);
			me->lbDNSTarget->AddItem(CSTRP(sbuff, sptr), target);
			if (target.Ptr() == currSel)
			{
				me->lbDNSTarget->SetSelectedIndex(i);
			}
			i++;
		}
	}
	if (me->analyzer->MDNSGetCount() != me->lbMDNS->GetCount())
	{
		Data::ArrayListNN<Net::DNSClient::RequestAnswer> mdnsList;
		NN<Net::DNSClient::RequestAnswer> ans;
		Net::DNSClient::RequestAnswer *currSel = (Net::DNSClient::RequestAnswer*)me->lbMDNS->GetSelectedItem().p;
		UOSInt i;
		UOSInt j;
		me->analyzer->MDNSGetList(mdnsList);
		me->lbMDNS->ClearItems();
		i = 0;
		j = mdnsList.GetCount();
		while (i < j)
		{
			ans = mdnsList.GetItemNoCheck(i);
			me->lbMDNS->AddItem(ans->name, ans);
			if (ans.Ptr() == currSel)
			{
				me->lbMDNS->SetSelectedIndex(i);
			}
			i++;
		}
	}
	if (me->analyzer->DNSCliGetCount() != me->lbDNSClient->GetCount())
	{
		NN<Net::EthernetAnalyzer::DNSClientInfo> cli;
		Net::EthernetAnalyzer::DNSClientInfo *currSel = (Net::EthernetAnalyzer::DNSClientInfo*)me->lbDNSClient->GetSelectedItem().p;
		Data::ArrayListNN<Net::EthernetAnalyzer::DNSClientInfo> cliList;
		UOSInt i;
		UOSInt j;
		Sync::MutexUsage mutUsage;
		me->analyzer->UseDNSCli(mutUsage);
		cliList.AddAll(me->analyzer->DNSCliGetList());
		mutUsage.EndUse();
		me->lbDNSClient->ClearItems();
		i = 0;
		j = cliList.GetCount();
		while (i < j)
		{
			cli = cliList.GetItemNoCheck(i);
			sptr = Net::SocketUtil::GetAddrName(sbuff, cli->addr).Or(sbuff);
			me->lbDNSClient->AddItem(CSTRP(sbuff, sptr), cli);
			if (cli.Ptr() == currSel)
			{
				me->lbDNSClient->SetSelectedIndex(i);
			}
			i++;
		}
	}
	if (me->analyzer->IPLogGetCount() != me->lbIPLog->GetCount())
	{
		NN<Net::EthernetAnalyzer::IPLogInfo> ipLog;
		Net::EthernetAnalyzer::IPLogInfo *currSel = (Net::EthernetAnalyzer::IPLogInfo*)me->lbIPLog->GetSelectedItem().p;
		Data::ArrayListNN<Net::EthernetAnalyzer::IPLogInfo> ipLogList;
		UOSInt i;
		UOSInt j;
		Sync::MutexUsage mutUsage;
		me->analyzer->UseIPLog(mutUsage);
		ipLogList.AddAll(me->analyzer->IPLogGetList());
		mutUsage.EndUse();
		me->lbIPLog->ClearItems();
		i = 0;
		j = ipLogList.GetCount();
		while (i < j)
		{
			ipLog = ipLogList.GetItemNoCheck(i);
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, ipLog->ip);
			me->lbIPLog->AddItem(CSTRP(sbuff, sptr), ipLog);
			if (ipLog.Ptr() == currSel)
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
		Data::ArrayListNN<Net::EthernetAnalyzer::IPTranStatus> ipTranList;
		NN<Net::EthernetAnalyzer::IPTranStatus> status;
		NN<IPTranInfo> ipTran;
		Sync::MutexUsage mutUsage;
		me->adapterChanged = false;
		me->analyzer->UseIPTran(mutUsage);
		ipTranList.AddAll(me->analyzer->IPTranGetList());
		mutUsage.EndUse();
		me->ipTranCnt = ipTranList.GetCount();
		i = 0;
		j = ipTranList.GetCount();
		while (i < j)
		{
			status = ipTranList.GetItemNoCheck(i);
			if (status->srcIP == me->adapterIP)
			{
				if (me->ipTranMap.Get(Net::SocketUtil::IPv4ToSortable(status->destIP)).SetTo(ipTran))
				{
					ipTran->sendStatus = status;
				}
				else
				{
					listChg = true;
					ipTran = MemAllocNN(IPTranInfo);
					ipTran->ip = status->destIP;
					ipTran->recvStatus = nullptr;
					ipTran->sendStatus = status;
					me->ipTranMap.Put(Net::SocketUtil::IPv4ToSortable(ipTran->ip), ipTran);
				}
			}
			else if (status->destIP == me->adapterIP)
			{
				if (me->ipTranMap.Get(Net::SocketUtil::IPv4ToSortable(status->srcIP)).SetTo(ipTran))
				{
					ipTran->recvStatus = status;
				}
				else
				{
					listChg = true;
					ipTran = MemAllocNN(IPTranInfo);
					ipTran->ip = status->srcIP;
					ipTran->recvStatus = status;
					ipTran->sendStatus = nullptr;
					me->ipTranMap.Put(Net::SocketUtil::IPv4ToSortable(ipTran->ip), ipTran);
				}
			}
			i++;
		}

		if (listChg)
		{
			IPTranInfo *currSel = (IPTranInfo*)me->lbIPTran->GetSelectedItem().p;
			me->lbIPTran->ClearItems();
			i = 0;
			j = me->ipTranMap.GetCount();
			while (i < j)
			{
				ipTran = me->ipTranMap.GetItemNoCheck(i);
				sptr = Net::SocketUtil::GetIPv4Name(sbuff, ipTran->ip);
				me->lbIPTran->AddItem(CSTRP(sbuff, sptr), ipTran);
				if (currSel == ipTran.Ptr())
				{
					me->lbIPTran->SetSelectedIndex(i);
				}
				i++;
			}
		}
	}
	if (me->analyzer->TCP4SYNIsDiff(me->tcp4synLastIndex))
	{
		Data::ArrayListT<Net::EthernetAnalyzer::TCP4SYNInfo> synList;
		Net::EthernetAnalyzer::TCP4SYNInfo syn;
		UOSInt i = 0;
		UOSInt j = me->analyzer->TCP4SYNGetList(synList, me->tcp4synLastIndex);
		me->lvTCP4SYN->ClearItems();
		while (i < j)
		{
			syn = synList.GetItem(i);
			sptr = syn.reqTime.ToString(sbuff);
			me->lvTCP4SYN->AddItem(CSTRP(sbuff, sptr), 0);
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, syn.srcAddr);
			me->lvTCP4SYN->SetSubItem(i, 1, CSTRP(sbuff, sptr));
			if (me->analyzer->DNSTargetGetName(syn.srcAddr).SetTo(s))
				me->lvTCP4SYN->SetSubItem(i, 2, s);
			sptr = Text::StrUInt16(sbuff, syn.srcPort);
			me->lvTCP4SYN->SetSubItem(i, 3, CSTRP(sbuff, sptr));
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, syn.destAddr);
			me->lvTCP4SYN->SetSubItem(i, 4, CSTRP(sbuff, sptr));
			if (me->analyzer->DNSTargetGetName(syn.destAddr).SetTo(s))
				me->lvTCP4SYN->SetSubItem(i, 5, s);
			sptr = Text::StrUInt16(sbuff, syn.destPort);
			me->lvTCP4SYN->SetSubItem(i, 6, CSTRP(sbuff, sptr));
			i++;
		}
	}
	{
		Int64 dispTime = Data::DateTimeUtil::GetCurrTimeMillis() / 1000 - 1;
		Sync::MutexUsage mutUsage;
		NN<Data::FastMapNN<UInt32, Net::EthernetAnalyzer::BandwidthStat>> statMap = me->analyzer->BandwidthGetAll(mutUsage);
		NN<Net::EthernetAnalyzer::BandwidthStat> stat;
		UInt32 ip;
		UOSInt i = 0;
		UOSInt j = me->lvBandwidth->GetCount();
		while (i < j)
		{
			stat = me->lvBandwidth->GetItem(i).GetNN<Net::EthernetAnalyzer::BandwidthStat>();
			if (stat == statMap->GetItemNoCheck(i))
			{
/*				if ((stat->displayFlags & 1) == 0)
				{
#if IS_BYTEORDER_LE
					ip = BSWAPU32(stat->ip);
#else
					ip = stat->ip;
#endif
					if (me->analyzer->DNSTargetGetName(ip).SetTo(s))
					{
						stat->displayFlags |= 1;
						me->lvBandwidth->SetSubItem(i, 1, s);
					}
				}*/
				if (dispTime <= stat->displayTime)
				{
				}
				else if (stat->displayTime > stat->currStat.time)
				{
				}
				else if (dispTime > stat->currStat.time)
				{
					stat->displayTime = dispTime;
					me->lvBandwidth->SetSubItem(i, 2, CSTR("0"));
					me->lvBandwidth->SetSubItem(i, 3, CSTR("0"));
					me->lvBandwidth->SetSubItem(i, 4, CSTR("0"));
					me->lvBandwidth->SetSubItem(i, 5, CSTR("0"));
				}
				else if (dispTime == stat->lastStat.time)
				{
					stat->displayTime = dispTime;
					sptr = Text::StrUInt64(sbuff, stat->lastStat.recvBytes);
					me->lvBandwidth->SetSubItem(i, 2, CSTRP(sbuff, sptr));
					sptr = Text::StrUOSInt(sbuff, stat->lastStat.recvCnt);
					me->lvBandwidth->SetSubItem(i, 3, CSTRP(sbuff, sptr));
					sptr = Text::StrUInt64(sbuff, stat->lastStat.sendBytes);
					me->lvBandwidth->SetSubItem(i, 4, CSTRP(sbuff, sptr));
					sptr = Text::StrUOSInt(sbuff, stat->lastStat.sendCnt);
					me->lvBandwidth->SetSubItem(i, 5, CSTRP(sbuff, sptr));
				}
				else if (dispTime == stat->currStat.time)
				{
					stat->displayTime = dispTime;
					sptr = Text::StrUInt64(sbuff, stat->currStat.recvBytes);
					me->lvBandwidth->SetSubItem(i, 2, CSTRP(sbuff, sptr));
					sptr = Text::StrUOSInt(sbuff, stat->currStat.recvCnt);
					me->lvBandwidth->SetSubItem(i, 3, CSTRP(sbuff, sptr));
					sptr = Text::StrUInt64(sbuff, stat->currStat.sendBytes);
					me->lvBandwidth->SetSubItem(i, 4, CSTRP(sbuff, sptr));
					sptr = Text::StrUOSInt(sbuff, stat->currStat.sendCnt);
					me->lvBandwidth->SetSubItem(i, 5, CSTRP(sbuff, sptr));
				}
				else if (stat->displayTime == stat->lastStat.time)
				{
					stat->displayTime = dispTime;
					me->lvBandwidth->SetSubItem(i, 2, CSTR("0"));
					me->lvBandwidth->SetSubItem(i, 3, CSTR("0"));
					me->lvBandwidth->SetSubItem(i, 4, CSTR("0"));
					me->lvBandwidth->SetSubItem(i, 5, CSTR("0"));
				}
				else
				{
				}
			}
			else
			{
				stat = statMap->GetItemNoCheck(i);
#if IS_BYTEORDER_LE
				ip = BSWAPU32(stat->ip);
#else
				ip = stat->ip;
#endif
				sptr = Net::SocketUtil::GetIPv4Name(sbuff, ip);
				me->lvBandwidth->InsertItem(i, CSTRP(sbuff, sptr), stat);
				j++;
				stat->displayTime = stat->lastStat.time;
				if (me->analyzer->DNSTargetGetName(ip).SetTo(s))
				{
					stat->displayFlags |= 1;
					me->lvBandwidth->SetSubItem(i, 1, s);
				}
				sptr = Text::StrUInt64(sbuff, stat->lastStat.recvBytes);
				me->lvBandwidth->SetSubItem(i, 2, CSTRP(sbuff, sptr));
				sptr = Text::StrUOSInt(sbuff, stat->lastStat.recvCnt);
				me->lvBandwidth->SetSubItem(i, 3, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt64(sbuff, stat->lastStat.sendBytes);
				me->lvBandwidth->SetSubItem(i, 4, CSTRP(sbuff, sptr));
				sptr = Text::StrUOSInt(sbuff, stat->lastStat.sendCnt);
				me->lvBandwidth->SetSubItem(i, 5, CSTRP(sbuff, sptr));
			}
			i++;
		}
		i = j;
		j = statMap->GetCount();
		while (i < j)
		{
			stat = statMap->GetItemNoCheck(i);
#if IS_BYTEORDER_LE
			ip = BSWAPU32(stat->ip);
#else
			ip = stat->ip;
#endif
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, ip);
			me->lvBandwidth->InsertItem(i, CSTRP(sbuff, sptr), stat);
			stat->displayTime = stat->lastStat.time;
			if (me->analyzer->DNSTargetGetName(ip).SetTo(s))
			{
				stat->displayFlags |= 1;
				me->lvBandwidth->SetSubItem(i, 1, s);
			}
			sptr = Text::StrUInt64(sbuff, stat->lastStat.recvBytes);
			me->lvBandwidth->SetSubItem(i, 2, CSTRP(sbuff, sptr));
			sptr = Text::StrUOSInt(sbuff, stat->lastStat.recvCnt);
			me->lvBandwidth->SetSubItem(i, 3, CSTRP(sbuff, sptr));
			sptr = Text::StrUInt64(sbuff, stat->lastStat.sendBytes);
			me->lvBandwidth->SetSubItem(i, 4, CSTRP(sbuff, sptr));
			sptr = Text::StrUOSInt(sbuff, stat->lastStat.sendCnt);
			me->lvBandwidth->SetSubItem(i, 5, CSTRP(sbuff, sptr));
			i++;
		}
	}

	if (me->socMon.NotNull() || me->dataUpdated)
	{
		NN<IPTranInfo> currSel;
		UOSInt i;
		UOSInt j;
		me->dataUpdated = false;
		if (me->lbIPTran->GetSelectedItem().GetOpt<IPTranInfo>().SetTo(currSel))
		{
			NN<Net::EthernetAnalyzer::IPTranStatus> status;
			if (currSel->recvStatus.SetTo(status))
			{
				sptr = Text::StrUInt64(sbuff, status->tcpCnt);
				me->lvIPTranInfo->SetSubItem(0, 1, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt64(sbuff, status->tcpSize);
				me->lvIPTranInfo->SetSubItem(2, 1, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt64(sbuff, status->udpCnt);
				me->lvIPTranInfo->SetSubItem(4, 1, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt64(sbuff, status->udpSize);
				me->lvIPTranInfo->SetSubItem(6, 1, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt64(sbuff, status->icmpCnt);
				me->lvIPTranInfo->SetSubItem(8, 1, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt64(sbuff, status->icmpSize);
				me->lvIPTranInfo->SetSubItem(10, 1, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt64(sbuff, status->otherCnt);
				me->lvIPTranInfo->SetSubItem(12, 1, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt64(sbuff, status->otherSize);
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
			
			if (currSel->sendStatus.SetTo(status))
			{
				sptr = Text::StrUInt64(sbuff, status->tcpCnt);
				me->lvIPTranInfo->SetSubItem(1, 1, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt64(sbuff, status->tcpSize);
				me->lvIPTranInfo->SetSubItem(3, 1, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt64(sbuff, status->udpCnt);
				me->lvIPTranInfo->SetSubItem(5, 1, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt64(sbuff, status->udpSize);
				me->lvIPTranInfo->SetSubItem(7, 1, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt64(sbuff, status->icmpCnt);
				me->lvIPTranInfo->SetSubItem(9, 1, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt64(sbuff, status->icmpSize);
				me->lvIPTranInfo->SetSubItem(11, 1, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt64(sbuff, status->otherCnt);
				me->lvIPTranInfo->SetSubItem(13, 1, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt64(sbuff, status->otherSize);
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

		NN<const Data::ReadingListNN<Net::EthernetAnalyzer::MACStatus>> macList;
		NN<Net::EthernetAnalyzer::MACStatus> mac;
		NN<const Net::MACInfo::MACEntry> entry;
		UInt8 macBuff[8];
		Net::MACInfo::AddressType addrType;
		Sync::MutexUsage mutUsage;
		me->analyzer->UseMAC(mutUsage);
		macList = me->analyzer->MACGetList();
		j = macList->GetCount();
		if (j != me->lvDevice->GetCount())
		{
			me->lvDevice->ClearItems();
			i = 0;
			while (i < j)
			{
				mac = macList->GetItemNoCheck(i);
				WriteMUInt64(macBuff, mac->mac64Addr);
				sptr = Text::StrHexBytes(sbuff, &macBuff[0], 6, ':');
				me->lvDevice->AddItem(CSTRP(sbuff, sptr), mac);
				addrType = Net::MACInfo::GetAddressType(macBuff);
				if (addrType == Net::MACInfo::AddressType::UniversalMulticast)
				{
					me->lvDevice->SetSubItem(i, 1, CSTR("Universal Multicast"));
				}
				else if (addrType == Net::MACInfo::AddressType::LocalMulticast)
				{
					me->lvDevice->SetSubItem(i, 1, CSTR("Local Multicast"));
				}
				else if (addrType == Net::MACInfo::AddressType::LocalUnicast)
				{
					me->lvDevice->SetSubItem(i, 1, CSTR("Local Unicast"));
				}
				else
				{
					entry = Net::MACInfo::GetMAC64Info(mac->mac64Addr);
					me->lvDevice->SetSubItem(i, 1, {entry->name, entry->nameLen});
				}
				if (mac->name.SetTo(s))
				{
					me->lvDevice->SetSubItem(i, 8, s->ToCString());
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
			mac = macList->GetItemNoCheck(i);
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
					sptr = Text::StrConcatC(Net::SocketUtil::GetIPv4Name(sbuff, mac->ipv4Addr[0]), UTF8STRC(", "));
					sptr = Net::SocketUtil::GetAddrName(sptr, mac->ipv6Addr).Or(sptr);
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
					sptr = Net::SocketUtil::GetAddrName(sbuff, mac->ipv6Addr).Or(sbuff);
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
		UnsafeArray<UTF8Char> sptr;
		NN<Net::EthernetAnalyzer::DHCPInfo> dhcp;
		NN<const Net::MACInfo::MACEntry> macInfo;
		Sync::MutexUsage mutUsage;
		me->analyzer->UseDHCP(mutUsage);
		NN<const Data::ReadingListNN<Net::EthernetAnalyzer::DHCPInfo>> dhcpList = me->analyzer->DHCPGetList();
		if (dhcpList->GetCount() != me->lvDHCP->GetCount())
		{
			Optional<Net::EthernetAnalyzer::DHCPInfo> currSel = me->lvDHCP->GetSelectedItem().GetOpt<Net::EthernetAnalyzer::DHCPInfo>();
			me->lvDHCP->ClearItems();
			i = 0;
			j = dhcpList->GetCount();
			while (i < j)
			{
				dhcp = dhcpList->GetItemNoCheck(i);
				WriteMUInt64(mac, dhcp->iMAC64);
				sptr = Text::StrHexBytes(sbuff, &mac[0], 6, ':');
				me->lvDHCP->AddItem(CSTRP(sbuff, sptr), dhcp);
				macInfo = Net::MACInfo::GetMAC64Info(dhcp->iMAC64);
				me->lvDHCP->SetSubItem(i, 1, {macInfo->name, macInfo->nameLen});
				if (dhcp.Ptr() == currSel.OrNull())
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
			dhcp = dhcpList->GetItemNoCheck(i);
			if (dhcp->updated)
			{
				Data::DateTime dt;
				Sync::MutexUsage mutUsage(dhcp->mut);
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
				sptr = dhcp->ipAddrTime.ToLocalTime().ToStringNoZone(sbuff);
				me->lvDHCP->SetSubItem(i, 8, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt32(sbuff, dhcp->ipAddrLease);
				me->lvDHCP->SetSubItem(i, 9, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt32(sbuff, dhcp->renewTime);
				me->lvDHCP->SetSubItem(i, 10, CSTRP(sbuff, sptr));
				sptr = Text::StrUInt32(sbuff, dhcp->rebindTime);
				me->lvDHCP->SetSubItem(i, 11, CSTRP(sbuff, sptr));
				if (dhcp->hostName.SetTo(s))
					me->lvDHCP->SetSubItem(i, 12, s);
				if (dhcp->vendorClass.SetTo(s))
					me->lvDHCP->SetSubItem(i, 13, s);
				mutUsage.EndUse();
			}
			i++;
		}
		mutUsage.EndUse();
	}
}

void __stdcall SSWR::AVIRead::AVIRRAWMonitorForm::OnDeviceSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRRAWMonitorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRAWMonitorForm>();
	NN<Net::EthernetAnalyzer::MACStatus> mac;
	Data::DateTime dt;
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UInt8> packetData;
	if (me->lvDevice->GetSelectedItem().GetOpt<Net::EthernetAnalyzer::MACStatus>().SetTo(mac))
	{
		UOSInt cnt;
		UOSInt i;
		Sync::MutexUsage mutUsage;
		me->analyzer->UseMAC(mutUsage);
		cnt = (UOSInt)(mac->ipv4SrcCnt + mac->ipv6SrcCnt + mac->othSrcCnt);
		if (cnt <= 16)
		{
			i = 0;
			while (i < cnt)
			{
				dt.SetInstant(mac->packetTime[i]);
				dt.ToLocalTime();
				sptr = dt.ToStringNoZone(sbuff);
				sb.AppendP(sbuff, sptr);
				sb.AppendC(UTF8STRC("\r\n"));
				sb.AppendC(UTF8STRC("Dest MAC: "));
				WriteMUInt64(sbuff, mac->packetDestMAC[i]);
				if (mac->packetData[i].SetTo(packetData))
				{
					Net::PacketAnalyzerEthernet::PacketEthernetDataGetDetail(mac->packetEtherType[i], packetData, mac->packetSize[i], sb);
				}
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
				dt.SetInstant(mac->packetTime[(cnt + i) & 15]);
				dt.ToLocalTime();
				sptr = dt.ToStringNoZone(sbuff);
				sb.AppendP(sbuff, sptr);
				sb.AppendC(UTF8STRC("\r\n"));
				sb.AppendC(UTF8STRC("Dest MAC: "));
				WriteMUInt64(sbuff, mac->packetDestMAC[(cnt + i) & 15]);
				sb.AppendHexBuff(&sbuff[2], 6, ':', Text::LineBreakType::None);
				if (mac->packetData[(cnt + i) & 15].SetTo(packetData))
				{
					Net::PacketAnalyzerEthernet::PacketEthernetDataGetDetail(mac->packetEtherType[(cnt + i) & 15], packetData, mac->packetSize[(cnt + i) & 15], sb);
				}
				sb.AppendC(UTF8STRC("\r\n"));
				sb.AppendC(UTF8STRC("\r\n"));
				i++;
			}
		}
	}
	me->txtDevice->SetText(sb.ToCString());
}

void __stdcall SSWR::AVIRead::AVIRRAWMonitorForm::OnDeviceDblClk(AnyType userObj, UOSInt itemIndex)
{
	NN<SSWR::AVIRead::AVIRRAWMonitorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRAWMonitorForm>();
	NN<Text::String> s;
	if (me->lvDevice->GetItemTextNew(itemIndex).SetTo(s))
	{
		UI::Clipboard::SetString(me->GetHandle(), s->ToCString());
		s->Release();
	}
}

SSWR::AVIRead::AVIRRAWMonitorForm::AVIRRAWMonitorForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Optional<Net::EthernetAnalyzer> analyzer) : UI::GUIForm(parent, 1024, 768, ui), whois(core->GetSocketFactory(), 15000)
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(CSTR("RAW Monitor"));

	this->core = core;
	this->clif = core->GetTCPClientFactory();
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->listener = nullptr;
	this->webHdlr = nullptr;
	this->adapterIP = 0;
	this->adapterChanged = false;
	this->dataUpdated = true;
	this->plogWriter = nullptr;
	this->linkType = IO::PacketAnalyse::LinkType::Ethernet;
	if (!analyzer.SetTo(this->analyzer))
	{
		NEW_CLASSNN(this->analyzer, Net::EthernetAnalyzer(nullptr, Net::EthernetAnalyzer::AT_ALL, CSTR("RAWMonitor")));
	}
	this->ipTranCnt = 0;
	this->pingIPListUpdated = false;
	this->pingIPContUpdated = false;
	this->currPingIP = nullptr;
	this->analyzer->HandlePingv4Request(OnPingPacket, this);
	this->tcp4synLastIndex = 0;

	this->pnlControl = ui->NewPanel(*this);
	this->pnlControl->SetRect(0, 0, 100, 79, false);
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
	this->cboIP->HandleSelectionChange(OnIPSelChg, this);
	this->btnStart = ui->NewButton(this->pnlControl, CSTR("Start"));
	this->btnStart->SetRect(254, 28, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	this->lblPLog = ui->NewLabel(this->pnlControl, CSTR("Packet Log"));
	this->lblPLog->SetRect(4, 52, 100, 23, false);
	this->cboPLog = ui->NewComboBox(this->pnlControl, false);
	this->cboPLog->SetRect(104, 52, 150, 23, false);
	this->cboPLog->AddItem(CSTR("Pcap"), nullptr);	
	this->cboPLog->AddItem(CSTR("Pcapng"), nullptr);	
	this->cboPLog->SetSelectedIndex(1);
	this->btnPLog = ui->NewButton(this->pnlControl, CSTR("Begin Log"));
	this->btnPLog->SetRect(254, 52, 75, 23, false);
	this->btnPLog->HandleButtonClick(OnPLogClicked, this);
	this->txtPLog = ui->NewTextBox(this->pnlControl, CSTR(""));
	this->txtPLog->SetRect(334, 52, 200, 23, false);
	this->txtPLog->SetReadOnly(true);
	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpDevice = this->tcMain->AddTabPage(CSTR("Device"));
	this->txtDevice = ui->NewTextBox(this->tpDevice, CSTR(""), true);
	this->txtDevice->SetReadOnly(true);
	this->txtDevice->SetRect(0, 0, 100, 300, false);
	this->txtDevice->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->vspDevice = ui->NewVSplitter(this->tpDevice, 3, true);
	this->lvDevice = ui->NewListView(this->tpDevice, UI::ListViewStyle::Table, 10);
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
	this->lvDevice->HandleDblClk(OnDeviceDblClk, this);

	this->tpIPTran = this->tcMain->AddTabPage(CSTR("IP Tran"));
	this->lbIPTran = ui->NewListBox(this->tpIPTran, false);
	this->lbIPTran->SetRect(0, 0, 150, 23, false);
	this->lbIPTran->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbIPTran->HandleSelectionChange(OnIPTranSelChg, this);
	this->hspIPTran = ui->NewHSplitter(this->tpIPTran, 3, false);
	this->tcIPTran = ui->NewTabControl(this->tpIPTran);
	this->tcIPTran->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpIPTranInfo = this->tcIPTran->AddTabPage(CSTR("Info"));
	this->lvIPTranInfo = ui->NewListView(this->tpIPTranInfo, UI::ListViewStyle::Table, 2);
	this->lvIPTranInfo->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvIPTranInfo->SetFullRowSelect(true);
	this->lvIPTranInfo->SetShowGrid(true);
	this->lvIPTranInfo->AddColumn(CSTR("Name"), 200);
	this->lvIPTranInfo->AddColumn(CSTR("Value"), 100);
	this->lvIPTranInfo->AddItem(CSTR("Recv TCP Cnt"), nullptr);
	this->lvIPTranInfo->AddItem(CSTR("Send TCP Cnt"), nullptr);
	this->lvIPTranInfo->AddItem(CSTR("Recv TCP Size"), nullptr);
	this->lvIPTranInfo->AddItem(CSTR("Send TCP Size"), nullptr);
	this->lvIPTranInfo->AddItem(CSTR("Recv UDP Cnt"), nullptr);
	this->lvIPTranInfo->AddItem(CSTR("Send UDP Cnt"), nullptr);
	this->lvIPTranInfo->AddItem(CSTR("Recv UDP Size"), nullptr);
	this->lvIPTranInfo->AddItem(CSTR("Send UDP Size"), nullptr);
	this->lvIPTranInfo->AddItem(CSTR("Recv ICMP Cnt"), nullptr);
	this->lvIPTranInfo->AddItem(CSTR("Send ICMP Cnt"), nullptr);
	this->lvIPTranInfo->AddItem(CSTR("Recv ICMP Size"), nullptr);
	this->lvIPTranInfo->AddItem(CSTR("Send ICMP Size"), nullptr);
	this->lvIPTranInfo->AddItem(CSTR("Recv Other Cnt"), nullptr);
	this->lvIPTranInfo->AddItem(CSTR("Send Other Cnt"), nullptr);
	this->lvIPTranInfo->AddItem(CSTR("Recv Other Size"), nullptr);
	this->lvIPTranInfo->AddItem(CSTR("Send Other Size"), nullptr);
	this->tpIPTranWhois = this->tcIPTran->AddTabPage(CSTR("Whois"));
	this->txtIPTranWhois = ui->NewTextBox(this->tpIPTranWhois, CSTR(""), true);
	this->txtIPTranWhois->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtIPTranWhois->SetReadOnly(true);

	this->tpDNSReqv4 = this->tcMain->AddTabPage(CSTR("DNS Req v4"));
	this->lbDNSReqv4 = ui->NewListBox(this->tpDNSReqv4, false);
	this->lbDNSReqv4->SetRect(0, 0, 250, 23, false);
	this->lbDNSReqv4->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbDNSReqv4->HandleSelectionChange(OnDNSReqv4SelChg, this);
	this->hspDNSReqv4 = ui->NewHSplitter(this->tpDNSReqv4, 3, false);
	this->pnlDNSReqv4 = ui->NewPanel(this->tpDNSReqv4);
	this->pnlDNSReqv4->SetRect(0, 0, 100, 79, false);
	this->pnlDNSReqv4->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblDNSReqv4Name = ui->NewLabel(this->pnlDNSReqv4, CSTR("Req Name"));
	this->lblDNSReqv4Name->SetRect(4, 4, 100, 23, false);
	this->txtDNSReqv4Name = ui->NewTextBox(this->pnlDNSReqv4, CSTR(""));
	this->txtDNSReqv4Name->SetRect(104, 4, 250, 23, false);
	this->txtDNSReqv4Name->SetReadOnly(true);
	this->lblDNSReqv4ReqTime = ui->NewLabel(this->pnlDNSReqv4, CSTR("Req Time"));
	this->lblDNSReqv4ReqTime->SetRect(4, 28, 100, 23, false);
	this->txtDNSReqv4ReqTime = ui->NewTextBox(this->pnlDNSReqv4, CSTR(""));
	this->txtDNSReqv4ReqTime->SetRect(104, 28, 150, 23, false);
	this->txtDNSReqv4ReqTime->SetReadOnly(true);
	this->lblDNSReqv4TTL = ui->NewLabel(this->pnlDNSReqv4, CSTR("TTL"));
	this->lblDNSReqv4TTL->SetRect(4, 52, 100, 23, false);
	this->txtDNSReqv4TTL = ui->NewTextBox(this->pnlDNSReqv4, CSTR(""));
	this->txtDNSReqv4TTL->SetRect(104, 52, 80, 23, false);
	this->txtDNSReqv4TTL->SetReadOnly(true);
	this->lvDNSReqv4 = ui->NewListView(this->tpDNSReqv4, UI::ListViewStyle::Table, 5);
	this->lvDNSReqv4->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDNSReqv4->SetFullRowSelect(true);
	this->lvDNSReqv4->SetShowGrid(true);
	this->lvDNSReqv4->AddColumn(CSTR("Name"), 250);
	this->lvDNSReqv4->AddColumn(CSTR("Type"), 40);
	this->lvDNSReqv4->AddColumn(CSTR("Class"), 40);
	this->lvDNSReqv4->AddColumn(CSTR("TTL"), 60);
	this->lvDNSReqv4->AddColumn(CSTR("RD"), 250);

	this->tpDNSReqv6 = this->tcMain->AddTabPage(CSTR("DNS Req v6"));
	this->lbDNSReqv6 = ui->NewListBox(this->tpDNSReqv6, false);
	this->lbDNSReqv6->SetRect(0, 0, 250, 23, false);
	this->lbDNSReqv6->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbDNSReqv6->HandleSelectionChange(OnDNSReqv6SelChg, this);
	this->hspDNSReqv6 = ui->NewHSplitter(this->tpDNSReqv6, 3, false);
	this->pnlDNSReqv6 = ui->NewPanel(this->tpDNSReqv6);
	this->pnlDNSReqv6->SetRect(0, 0, 100, 79, false);
	this->pnlDNSReqv6->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblDNSReqv6Name = ui->NewLabel(this->pnlDNSReqv6, CSTR("Req Name"));
	this->lblDNSReqv6Name->SetRect(4, 4, 100, 23, false);
	this->txtDNSReqv6Name = ui->NewTextBox(this->pnlDNSReqv6, CSTR(""));
	this->txtDNSReqv6Name->SetRect(104, 4, 250, 23, false);
	this->txtDNSReqv6Name->SetReadOnly(true);
	this->lblDNSReqv6ReqTime = ui->NewLabel(this->pnlDNSReqv6, CSTR("Req Time"));
	this->lblDNSReqv6ReqTime->SetRect(4, 28, 100, 23, false);
	this->txtDNSReqv6ReqTime = ui->NewTextBox(this->pnlDNSReqv6, CSTR(""));
	this->txtDNSReqv6ReqTime->SetRect(104, 28, 150, 23, false);
	this->txtDNSReqv6ReqTime->SetReadOnly(true);
	this->lblDNSReqv6TTL = ui->NewLabel(this->pnlDNSReqv6, CSTR("TTL"));
	this->lblDNSReqv6TTL->SetRect(4, 52, 100, 23, false);
	this->txtDNSReqv6TTL = ui->NewTextBox(this->pnlDNSReqv6, CSTR(""));
	this->txtDNSReqv6TTL->SetRect(104, 52, 80, 23, false);
	this->txtDNSReqv6TTL->SetReadOnly(true);
	this->lvDNSReqv6 = ui->NewListView(this->tpDNSReqv6, UI::ListViewStyle::Table, 5);
	this->lvDNSReqv6->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDNSReqv6->SetFullRowSelect(true);
	this->lvDNSReqv6->SetShowGrid(true);
	this->lvDNSReqv6->AddColumn(CSTR("Name"), 250);
	this->lvDNSReqv6->AddColumn(CSTR("Type"), 40);
	this->lvDNSReqv6->AddColumn(CSTR("Class"), 40);
	this->lvDNSReqv6->AddColumn(CSTR("TTL"), 60);
	this->lvDNSReqv6->AddColumn(CSTR("RD"), 250);

	this->tpDNSReqOth = this->tcMain->AddTabPage(CSTR("DNS Req Other"));
	this->lbDNSReqOth = ui->NewListBox(this->tpDNSReqOth, false);
	this->lbDNSReqOth->SetRect(0, 0, 250, 23, false);
	this->lbDNSReqOth->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbDNSReqOth->HandleSelectionChange(OnDNSReqOthSelChg, this);
	this->hspDNSReqOth = ui->NewHSplitter(this->tpDNSReqOth, 3, false);
	this->pnlDNSReqOth = ui->NewPanel(this->tpDNSReqOth);
	this->pnlDNSReqOth->SetRect(0, 0, 100, 79, false);
	this->pnlDNSReqOth->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblDNSReqOthName = ui->NewLabel(this->pnlDNSReqOth, CSTR("Req Name"));
	this->lblDNSReqOthName->SetRect(4, 4, 100, 23, false);
	this->txtDNSReqOthName = ui->NewTextBox(this->pnlDNSReqOth, CSTR(""));
	this->txtDNSReqOthName->SetRect(104, 4, 250, 23, false);
	this->txtDNSReqOthName->SetReadOnly(true);
	this->lblDNSReqOthReqTime = ui->NewLabel(this->pnlDNSReqOth, CSTR("Req Time"));
	this->lblDNSReqOthReqTime->SetRect(4, 28, 100, 23, false);
	this->txtDNSReqOthReqTime = ui->NewTextBox(this->pnlDNSReqOth, CSTR(""));
	this->txtDNSReqOthReqTime->SetRect(104, 28, 150, 23, false);
	this->txtDNSReqOthReqTime->SetReadOnly(true);
	this->lblDNSReqOthTTL = ui->NewLabel(this->pnlDNSReqOth, CSTR("TTL"));
	this->lblDNSReqOthTTL->SetRect(4, 52, 100, 23, false);
	this->txtDNSReqOthTTL = ui->NewTextBox(this->pnlDNSReqOth, CSTR(""));
	this->txtDNSReqOthTTL->SetRect(104, 52, 80, 23, false);
	this->txtDNSReqOthTTL->SetReadOnly(true);
	this->lvDNSReqOth = ui->NewListView(this->tpDNSReqOth, UI::ListViewStyle::Table, 5);
	this->lvDNSReqOth->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDNSReqOth->SetFullRowSelect(true);
	this->lvDNSReqOth->SetShowGrid(true);
	this->lvDNSReqOth->AddColumn(CSTR("Name"), 250);
	this->lvDNSReqOth->AddColumn(CSTR("Type"), 40);
	this->lvDNSReqOth->AddColumn(CSTR("Class"), 40);
	this->lvDNSReqOth->AddColumn(CSTR("TTL"), 60);
	this->lvDNSReqOth->AddColumn(CSTR("RD"), 250);

	this->tpDNSTarget = this->tcMain->AddTabPage(CSTR("DNS Target"));
	this->lbDNSTarget = ui->NewListBox(this->tpDNSTarget, false);
	this->lbDNSTarget->SetRect(0, 0, 150, 23, false);
	this->lbDNSTarget->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbDNSTarget->HandleSelectionChange(OnDNSTargetSelChg, this);
	this->hspDNSTarget = ui->NewHSplitter(this->tpDNSTarget, 3, false);
	this->tcDNSTarget = ui->NewTabControl(this->tpDNSTarget);
	this->tcDNSTarget->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpDNSTargetDomain = this->tcDNSTarget->AddTabPage(CSTR("Domains"));
	this->lbDNSTargetDomain = ui->NewListBox(this->tpDNSTargetDomain, false);
	this->lbDNSTargetDomain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpDNSTargetWhois = this->tcDNSTarget->AddTabPage(CSTR("Whois"));
	this->txtDNSTargetWhois = ui->NewTextBox(this->tpDNSTargetWhois, CSTR(""), true);
	this->txtDNSTargetWhois->SetReadOnly(true);
	this->txtDNSTargetWhois->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpMDNS = this->tcMain->AddTabPage(CSTR("MDNS"));
	this->lbMDNS = ui->NewListBox(this->tpMDNS, false);
	this->lbMDNS->SetRect(0, 0, 200, 23, false);
	this->lbMDNS->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbMDNS->HandleSelectionChange(OnMDNSSelChg, this);
	this->hspMDNS = ui->NewHSplitter(this->tpMDNS, 3, false);
	this->pnlMDNS = ui->NewPanel(this->tpMDNS);
	this->pnlMDNS->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lblMDNSName = ui->NewLabel(this->pnlMDNS, CSTR("Name"));
	this->lblMDNSName->SetRect(4, 4, 100, 23, false);
	this->txtMDNSName = ui->NewTextBox(this->pnlMDNS, CSTR(""));
	this->txtMDNSName->SetRect(104, 4, 400, 23, false);
	this->txtMDNSName->SetReadOnly(true);
	this->lblMDNSType = ui->NewLabel(this->pnlMDNS, CSTR("Type"));
	this->lblMDNSType->SetRect(4, 28, 100, 23, false);
	this->txtMDNSType = ui->NewTextBox(this->pnlMDNS, CSTR(""));
	this->txtMDNSType->SetRect(104, 28, 200, 23, false);
	this->txtMDNSType->SetReadOnly(true);
	this->lblMDNSClass = ui->NewLabel(this->pnlMDNS, CSTR("Class"));
	this->lblMDNSClass->SetRect(4, 52, 100, 23, false);
	this->txtMDNSClass = ui->NewTextBox(this->pnlMDNS, CSTR(""));
	this->txtMDNSClass->SetRect(104, 52, 200, 23, false);
	this->txtMDNSClass->SetReadOnly(true);
	this->lblMDNSTTL = ui->NewLabel(this->pnlMDNS, CSTR("TTL"));
	this->lblMDNSTTL->SetRect(4, 76, 100, 23, false);
	this->txtMDNSTTL = ui->NewTextBox(this->pnlMDNS, CSTR(""));
	this->txtMDNSTTL->SetRect(104, 76, 100, 23, false);
	this->txtMDNSTTL->SetReadOnly(true);
	this->lblMDNSResult = ui->NewLabel(this->pnlMDNS, CSTR("Result"));
	this->lblMDNSResult->SetRect(4, 100, 100, 23, false);
	this->txtMDNSResult = ui->NewTextBox(this->pnlMDNS, CSTR(""));
	this->txtMDNSResult->SetRect(104, 100, 400, 23, false);
	this->txtMDNSResult->SetReadOnly(true);

	this->tpDNSClient = this->tcMain->AddTabPage(CSTR("DNS Client"));
	this->lbDNSClient = ui->NewListBox(this->tpDNSClient, false);
	this->lbDNSClient->SetRect(0, 0, 150, 23, false);
	this->lbDNSClient->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbDNSClient->HandleSelectionChange(OnDNSClientSelChg, this);
	this->hspDNSClient = ui->NewHSplitter(this->tpDNSClient, 3, false);
	this->lvDNSClient = ui->NewListView(this->tpDNSClient, UI::ListViewStyle::Table, 2);
	this->lvDNSClient->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDNSClient->SetFullRowSelect(true);
	this->lvDNSClient->SetShowGrid(true);
	this->lvDNSClient->AddColumn(CSTR("Time"), 120);
	this->lvDNSClient->AddColumn(CSTR("Count"), 100);

	this->tpDHCP = this->tcMain->AddTabPage(CSTR("DHCP"));
	this->lvDHCP = ui->NewListView(this->tpDHCP, UI::ListViewStyle::Table, 14);
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
	this->lbIPLog = ui->NewListBox(this->tpIPLog, false);
	this->lbIPLog->SetRect(0, 0, 150, 23, false);
	this->lbIPLog->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbIPLog->HandleSelectionChange(OnIPLogSelChg, this);
	this->hspIPLog = ui->NewHSplitter(this->tpIPLog, 3, false);
	this->lbIPLogVal = ui->NewListBox(this->tpIPLog, false);
	this->lbIPLogVal->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpPingIP = this->tcMain->AddTabPage(CSTR("Ping IP"));
	this->lbPingIP = ui->NewListBox(this->tpPingIP, false);
	this->lbPingIP->SetRect(0, 0, 150, 23, false);
	this->lbPingIP->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbPingIP->HandleSelectionChange(OnPingIPSelChg, this);
	this->hspPingIP = ui->NewHSplitter(this->tpPingIP, 3, false);
	this->tcPingIP = ui->NewTabControl(this->tpPingIP);
	this->tcPingIP->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpPingIPInfo = this->tcPingIP->AddTabPage(CSTR("Info"));
	this->lblPingIPCount = ui->NewLabel(this->tpPingIPInfo, CSTR("Count"));
	this->lblPingIPCount->SetRect(4, 4, 100, 23, false);
	this->txtPingIPCount = ui->NewTextBox(this->tpPingIPInfo, CSTR("0"));
	this->txtPingIPCount->SetRect(104, 4, 100, 23, false);
	this->txtPingIPCount->SetReadOnly(true);
	this->lblPingIPName = ui->NewLabel(this->tpPingIPInfo, CSTR("Name"));
	this->lblPingIPName->SetRect(4, 28, 100, 23, false);
	this->txtPingIPName = ui->NewTextBox(this->tpPingIPInfo, CSTR(""));
	this->txtPingIPName->SetRect(104, 28, 200, 23, false);
	this->txtPingIPName->SetReadOnly(true);
	this->lblPingIPCountry = ui->NewLabel(this->tpPingIPInfo, CSTR("Country"));
	this->lblPingIPCountry->SetRect(4, 52, 100, 23, false);
	this->txtPingIPCountry = ui->NewTextBox(this->tpPingIPInfo, CSTR(""));
	this->txtPingIPCountry->SetRect(104, 52, 100, 23, false);
	this->txtPingIPCountry->SetReadOnly(true);
	this->tpPingIPWhois = this->tcPingIP->AddTabPage(CSTR("Whois"));
	this->txtPingIPWhois = ui->NewTextBox(this->tpPingIPWhois, CSTR(""), true);
	this->txtPingIPWhois->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtPingIPWhois->SetReadOnly(true);

	this->tpTCP4SYN = this->tcMain->AddTabPage(CSTR("TCPv4 SYN"));
	this->lvTCP4SYN = ui->NewListView(this->tpTCP4SYN, UI::ListViewStyle::Table, 7);
	this->lvTCP4SYN->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvTCP4SYN->SetFullRowSelect(true);
	this->lvTCP4SYN->SetShowGrid(true);
	this->lvTCP4SYN->AddColumn(CSTR("Time"), 180);
	this->lvTCP4SYN->AddColumn(CSTR("Source IP"), 100);
	this->lvTCP4SYN->AddColumn(CSTR("Source Name"), 150);
	this->lvTCP4SYN->AddColumn(CSTR("Port"), 50);
	this->lvTCP4SYN->AddColumn(CSTR("Dest IP"), 100);
	this->lvTCP4SYN->AddColumn(CSTR("Dest Name"), 150);
	this->lvTCP4SYN->AddColumn(CSTR("Port"), 50);

	this->tpBandwidth = this->tcMain->AddTabPage(CSTR("Bandwidth"));
	this->lvBandwidth = ui->NewListView(this->tpBandwidth, UI::ListViewStyle::Table, 6);
	this->lvBandwidth->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvBandwidth->SetFullRowSelect(true);
	this->lvBandwidth->SetShowGrid(true);
	this->lvBandwidth->AddColumn(CSTR("IP"), 100);
	this->lvBandwidth->AddColumn(CSTR("Name"), 150);
	this->lvBandwidth->AddColumn(CSTR("Recv Rate"), 100);
	this->lvBandwidth->AddColumn(CSTR("Recv Cnt"), 50);
	this->lvBandwidth->AddColumn(CSTR("Send Rate"), 100);
	this->lvBandwidth->AddColumn(CSTR("Send Cnt"), 50);

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
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UInt32 ip;
	this->clif->GetSocketFactory()->GetConnInfoList(connInfoList);
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
			this->cboIP->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)ip);
			k++;
		}
		connInfo.Delete();
		i++;
	}
	if (this->cboIP->GetCount() > 0)
	{
		this->adapterIP = (UInt32)this->cboIP->GetItem(0).GetOSInt();
		this->cboIP->SetSelectedIndex(0);
		this->adapterChanged = true;
	}
	this->socMon = nullptr;
	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRRAWMonitorForm::~AVIRRAWMonitorForm()
{
	this->socMon.Delete();
	if (this->listener.NotNull())
	{
		this->listener.Delete();
		this->webHdlr.Delete();
	}
	this->log.RemoveLogHandler(this->logger);
	this->logger.Delete();
	this->plogWriter.Delete();

	NN<PingIPInfo> pingIPInfo;
	UOSInt i;
	i = this->pingIPMap.GetCount();
	while (i-- > 0)
	{
		pingIPInfo = this->pingIPMap.GetItemNoCheck(i);
		pingIPInfo->name->Release();
		pingIPInfo->country->Release();
		MemFreeNN(pingIPInfo);
	}

	NN<IPTranInfo> ipTran;
	i = this->ipTranMap.GetCount();
	while (i-- > 0)
	{
		ipTran = this->ipTranMap.GetItemNoCheck(i);
		MemFreeNN(ipTran);
	}
	this->analyzer.Delete();
}

void SSWR::AVIRead::AVIRRAWMonitorForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
