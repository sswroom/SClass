#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Net/EthernetWebHandler.h"
#include "Net/MACInfo.h"
#include "Sync/MutexUsage.h"
#include "Text/StringBuilderUTF8.h"

void Net::EthernetWebHandler::AppendHeader(Text::StringBuilderUTF *sbOut)
{
	sbOut->Append((const UTF8Char*)"<html><head><title>NetRAWCapture</title></head><body>\r\n");
}

void Net::EthernetWebHandler::AppendMenu(Text::StringBuilderUTF *sbOut)
{
	Net::EthernetAnalyzer::AnalyzeType atype = this->analyzer->GetAnalyzeType();
	sbOut->Append((const UTF8Char*)"<table border=\"1\"><tr>");
	if (atype & Net::EthernetAnalyzer::AT_DEVICE)
		sbOut->Append((const UTF8Char*)"<td><a href=\"/device\">Device</a></td>");
	if (atype & Net::EthernetAnalyzer::AT_IPTRANSFER)
		sbOut->Append((const UTF8Char*)"<td><a href=\"/iptransfer\">IPTransfer</a></td>");
	if (atype & Net::EthernetAnalyzer::AT_DNSREQ)
		sbOut->Append((const UTF8Char*)"<td><a href=\"/dnsreqv4\">DNS Req v4</a></td>");
	if (atype & Net::EthernetAnalyzer::AT_DNSREQ)
		sbOut->Append((const UTF8Char*)"<td><a href=\"/dnsreqv6\">DNS Req v6</a></td>");
	if (atype & Net::EthernetAnalyzer::AT_DNSREQ)
		sbOut->Append((const UTF8Char*)"<td><a href=\"/dnsreqoth\">DNS Req Oth</a></td>");
	if (atype & Net::EthernetAnalyzer::AT_DNSTARGET)
		sbOut->Append((const UTF8Char*)"<td><a href=\"/dnstarget\">DNS Target</a></td>");
	if (atype & Net::EthernetAnalyzer::AT_DNSCLI)
		sbOut->Append((const UTF8Char*)"<td><a href=\"/dnsclient\">DNS Client</a></td>");
	if (atype & Net::EthernetAnalyzer::AT_DHCP)
		sbOut->Append((const UTF8Char*)"<td><a href=\"/dhcp\">DHCP</a></td>");
	if (atype & Net::EthernetAnalyzer::AT_IPLOG)
		sbOut->Append((const UTF8Char*)"<td><a href=\"/iplog\">IP Log</a></td>");
	sbOut->Append((const UTF8Char*)"</tr></table>\r\n");
}

void Net::EthernetWebHandler::AppendFooter(Text::StringBuilderUTF *sbOut)
{
	sbOut->Append((const UTF8Char*)"</body></html>");
}

Bool __stdcall Net::EthernetWebHandler::DeviceReq(EthernetWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	UTF8Char sbuff[128];
	if (req->GetReqMethod() == Net::WebServer::IWebRequest::REQMETH_HTTP_GET)
	{
		UOSInt i;
		UOSInt j;
		Data::ArrayList<Net::EthernetAnalyzer::MACStatus*> *macList;
		Net::EthernetAnalyzer::MACStatus *mac;
		const Net::MACInfo::MACEntry *macInfo;
		Text::StringBuilderUTF8 sb;
		AppendHeader(&sb);
		me->AppendMenu(&sb);
		sb.Append((const UTF8Char*)"Packet cnt = ");
		sb.AppendU64(me->analyzer->GetPacketCnt());
		sb.Append((const UTF8Char*)", Total size = ");
		sb.AppendU64(me->analyzer->GetPacketTotalSize());
		sb.Append((const UTF8Char*)"<br/>\r\n");

		sb.Append((const UTF8Char*)"<table border=\"1\"><tr>");
		sb.Append((const UTF8Char*)"<td>MAC List</td>");
		sb.Append((const UTF8Char*)"<td>Vendor</td>");
		sb.Append((const UTF8Char*)"<td>IPv4 Src</td>");
		sb.Append((const UTF8Char*)"<td>IPv4 Dest</td>");
		sb.Append((const UTF8Char*)"<td>IPv6 Src</td>");
		sb.Append((const UTF8Char*)"<td>IPv6 Dest</td>");
		sb.Append((const UTF8Char*)"<td>Other Src</td>");
		sb.Append((const UTF8Char*)"<td>Other Dest</td>");
		sb.Append((const UTF8Char*)"<td>Name</td>");
		sb.Append((const UTF8Char*)"<td>IP List</td>");
		sb.Append((const UTF8Char*)"</tr>\r\n");
		Sync::MutexUsage mutUsage;
		me->analyzer->UseMAC(&mutUsage);
		macList = me->analyzer->MACGetList();
		i = 0;
		j = macList->GetCount();
		while (i < j)
		{
			mac = macList->GetItem(i);
			sb.Append((const UTF8Char*)"<tr><td>");
			WriteMUInt64(sbuff, mac->macAddr);
			sb.AppendHexBuff(&sbuff[2], 6, ':', Text::LineBreakType::None);
			sb.Append((const UTF8Char*)"</td><td>");
			macInfo = Net::MACInfo::GetMACInfo(mac->macAddr);
			sb.Append((const UTF8Char*)macInfo->name);
			sb.Append((const UTF8Char*)"</td><td>");
			sb.AppendU64(mac->ipv4SrcCnt);
			sb.Append((const UTF8Char*)"</td><td>");
			sb.AppendU64(mac->ipv4DestCnt);
			sb.Append((const UTF8Char*)"</td><td>");
			sb.AppendU64(mac->ipv6SrcCnt);
			sb.Append((const UTF8Char*)"</td><td>");
			sb.AppendU64(mac->ipv6DestCnt);
			sb.Append((const UTF8Char*)"</td><td>");
			sb.AppendU64(mac->othSrcCnt);
			sb.Append((const UTF8Char*)"</td><td>");
			sb.AppendU64(mac->othDestCnt);
			sb.Append((const UTF8Char*)"</td><td>");
			if (mac->name)
			{
				sb.Append(mac->name);
			}
			else
			{
				sb.Append((const UTF8Char*)"Unknown");
			}
			sb.Append((const UTF8Char*)"</td><td>");
			
			if (mac->ipv4Addr[0])
			{
				Net::SocketUtil::GetIPv4Name(sbuff, mac->ipv4Addr[0]);
				sb.Append(sbuff);
				if (mac->ipv4Addr[1])
				{
					sb.Append((const UTF8Char*)"<br/>");
					Net::SocketUtil::GetIPv4Name(sbuff, mac->ipv4Addr[1]);
					sb.Append(sbuff);
					if (mac->ipv4Addr[2])
					{
						sb.Append((const UTF8Char*)"<br/>");
						Net::SocketUtil::GetIPv4Name(sbuff, mac->ipv4Addr[2]);
						sb.Append(sbuff);
						if (mac->ipv4Addr[3])
						{
							sb.Append((const UTF8Char*)"<br/>");
							Net::SocketUtil::GetIPv4Name(sbuff, mac->ipv4Addr[3]);
							sb.Append(sbuff);
						}
					}
				}
				if (mac->ipv6Addr.addrType == Net::SocketUtil::AT_IPV6)
				{
					sb.Append((const UTF8Char*)"<br/>");
					Net::SocketUtil::GetAddrName(sbuff, &mac->ipv6Addr);
					sb.Append(sbuff);
				}
			}
			else
			{
				if (mac->ipv6Addr.addrType == Net::SocketUtil::AT_IPV6)
				{
					Net::SocketUtil::GetAddrName(sbuff, &mac->ipv6Addr);
					sb.Append(sbuff);
				}
			}
			sb.Append((const UTF8Char*)"</td></tr>\r\n");
			i++;
		}
		mutUsage.EndUse();
		sb.Append((const UTF8Char*)"</table>");
		AppendFooter(&sb);

		resp->AddDefHeaders(req);
		resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
		resp->AddContentType((const UTF8Char*)"text/html");
		resp->AddContentLength(sb.GetLength());
		resp->Write(sb.ToString(), sb.GetLength());
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
	}
	return true;
}

Bool __stdcall Net::EthernetWebHandler::IPTransferReq(EthernetWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	UTF8Char sbuff[128];
	if (req->GetReqMethod() == Net::WebServer::IWebRequest::REQMETH_HTTP_GET)
	{
		UOSInt i;
		UOSInt j;
		Data::ArrayList<Net::EthernetAnalyzer::IPTranStatus*> *ipTranList;
		Net::EthernetAnalyzer::IPTranStatus *ipTran;
		Text::StringBuilderUTF8 sb;
		AppendHeader(&sb);
		me->AppendMenu(&sb);

		sb.Append((const UTF8Char*)"<table border=\"1\"><tr>");
		sb.Append((const UTF8Char*)"<td>IP List</td>");
		sb.Append((const UTF8Char*)"<td>TCP Cnt</td>");
		sb.Append((const UTF8Char*)"<td>UDP Cnt</td>");
		sb.Append((const UTF8Char*)"<td>ICMP Cnt</td>");
		sb.Append((const UTF8Char*)"<td>Other Cnt</td>");
		sb.Append((const UTF8Char*)"</tr>\r\n");
		Sync::MutexUsage mutUsage;
		me->analyzer->UseIPTran(&mutUsage);
		ipTranList = me->analyzer->IPTranGetList();
		i = 0;
		j = ipTranList->GetCount();
		while (i < j)
		{
			ipTran = ipTranList->GetItem(i);
			sb.Append((const UTF8Char*)"<tr><td>");
			Net::SocketUtil::GetIPv4Name(sbuff, ipTran->srcIP);
			sb.Append(sbuff);
			sb.Append((const UTF8Char*)" -> ");
			Net::SocketUtil::GetIPv4Name(sbuff, ipTran->destIP);
			sb.Append(sbuff);
			sb.Append((const UTF8Char*)"</td><td>");
			sb.AppendU64(ipTran->tcpCnt);
			sb.Append((const UTF8Char*)"</td><td>");
			sb.AppendU64(ipTran->udpCnt);
			sb.Append((const UTF8Char*)"</td><td>");
			sb.AppendU64(ipTran->icmpCnt);
			sb.Append((const UTF8Char*)"</td><td>");
			sb.AppendU64(ipTran->otherCnt);
			sb.Append((const UTF8Char*)"</td></tr>\r\n");
			i++;
		}
		mutUsage.EndUse();
		sb.Append((const UTF8Char*)"</table>");
		AppendFooter(&sb);

		resp->AddDefHeaders(req);
		resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
		resp->AddContentType((const UTF8Char*)"text/html");
		resp->AddContentLength(sb.GetLength());
		resp->Write(sb.ToString(), sb.GetLength());
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
	}
	return true;
}

Bool __stdcall Net::EthernetWebHandler::DNSReqv4Req(EthernetWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	UTF8Char sbuff[128];
	if (req->GetReqMethod() == Net::WebServer::IWebRequest::REQMETH_HTTP_GET)
	{
		UOSInt i;
		UOSInt j;
		Text::StringBuilderUTF8 sb;
		Data::ArrayList<const UTF8Char *> nameList;
		const UTF8Char *name;
		UOSInt nameLen;

		AppendHeader(&sb);
		me->AppendMenu(&sb);

		sb.Append((const UTF8Char*)"<table border=\"1\"><tr>");
		sb.Append((const UTF8Char*)"<td>Reqv4 List</td>");
		sb.Append((const UTF8Char*)"<td>Info</td>");
		sb.Append((const UTF8Char*)"</tr>\r\n");
		sb.Append((const UTF8Char*)"<tr><td>\r\n");
		me->analyzer->DNSReqv4GetList(&nameList);

		i = 0;
		j = nameList.GetCount();
		while (i < j)
		{
			name = nameList.GetItem(i);
			nameLen = Text::StrCharCnt(name);
			sb.AllocLeng(33 + nameLen * 2);
			if (i > 0)
			{
				sb.AppendC((const UTF8Char*)"<br/>\r\n", 7);
			}
			sb.AppendC((const UTF8Char*)"<a href=\"/dnsreqv4?qry=", 23);
			sb.AppendC(name, nameLen);
			sb.AppendC((const UTF8Char*)"\">", 2);
			sb.AppendC(name, nameLen);
			sb.AppendC((const UTF8Char*)"</a>", 4);

			i++;
		}
		sb.Append((const UTF8Char*)"</td><td valign=\"top\">\r\n");

		if (req->GetQueryValueStr((const UTF8Char*)"qry", sbuff, 128))
		{
			Data::ArrayList<Net::DNSClient::RequestAnswer *> ansList;
			Net::DNSClient::RequestAnswer *ans;
			Data::DateTime reqTime;
			UInt32 ttl;
			if (me->analyzer->DNSReqv4GetInfo(sbuff, &ansList, &reqTime, &ttl))
			{
				sb.Append((const UTF8Char*)"<table border=\"1\"><tr><td>Request Name</td><td>");
				sb.Append(sbuff);
				sb.Append((const UTF8Char*)"</td></tr>\r\n");
				sb.Append((const UTF8Char*)"<tr><td>Request Time</td><td>");
				reqTime.ToLocalTime();
				reqTime.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff zzzz");
				sb.Append(sbuff);
				sb.Append((const UTF8Char*)"</td></tr>\r\n");
				sb.Append((const UTF8Char*)"<tr><td>TTL</td><td>");
				sb.AppendU32(ttl);
				sb.Append((const UTF8Char*)"</td></tr>\r\n");
				sb.Append((const UTF8Char*)"<tr><td>Results</td><td>");

				sb.Append((const UTF8Char*)"<table border=\"1\"><tr><td>Name</td><td>Type</td><td>Class</td><td>TTL</td><td>RD</td><tr>\r\n");
				i = 0;
				j = ansList.GetCount();
				while (i < j)
				{
					ans = ansList.GetItem(i);
					sb.Append((const UTF8Char*)"<tr><td>");
					sb.Append(ans->name);
					sb.Append((const UTF8Char*)"</td><td>");
					sb.AppendI32(ans->recType);
					sb.Append((const UTF8Char*)"</td><td>");
					sb.AppendI32(ans->recClass);
					sb.Append((const UTF8Char*)"</td><td>");
					sb.AppendU32(ans->ttl);
					sb.Append((const UTF8Char*)"</td><td>");
					if (ans->rd)
					{
						sb.Append(ans->rd);
					}
					sb.Append((const UTF8Char*)"</td></tr>\r\n");
					i++;
				}
				sb.Append((const UTF8Char*)"</td></tr></table>\r\n");

				Net::DNSClient::FreeAnswers(&ansList);
				sb.Append((const UTF8Char*)"</table>\r\n");
			}
			else
			{
			}
		}

		sb.Append((const UTF8Char*)"</td></tr></table>\r\n");
		AppendFooter(&sb);

		resp->AddDefHeaders(req);
		resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
		resp->AddContentType((const UTF8Char*)"text/html");
		resp->AddContentLength(sb.GetLength());
		resp->Write(sb.ToString(), sb.GetLength());
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
	}
	return true;
}

Bool __stdcall Net::EthernetWebHandler::DNSReqv6Req(EthernetWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	UTF8Char sbuff[128];
	if (req->GetReqMethod() == Net::WebServer::IWebRequest::REQMETH_HTTP_GET)
	{
		UOSInt i;
		UOSInt j;
		Text::StringBuilderUTF8 sb;
		Data::ArrayList<const UTF8Char *> nameList;
		const UTF8Char *name;
		UOSInt nameLen;

		AppendHeader(&sb);
		me->AppendMenu(&sb);

		sb.Append((const UTF8Char*)"<table border=\"1\"><tr>");
		sb.Append((const UTF8Char*)"<td>Reqv6 List</td>");
		sb.Append((const UTF8Char*)"<td>Info</td>");
		sb.Append((const UTF8Char*)"</tr>\r\n");
		sb.Append((const UTF8Char*)"<tr><td>\r\n");
		me->analyzer->DNSReqv6GetList(&nameList);

		i = 0;
		j = nameList.GetCount();
		while (i < j)
		{
			name = nameList.GetItem(i);
			nameLen = Text::StrCharCnt(name);
			sb.AllocLeng(33 + nameLen * 2);
			if (i > 0)
			{
				sb.AppendC((const UTF8Char*)"<br/>\r\n", 7);
			}
			sb.AppendC((const UTF8Char*)"<a href=\"/dnsreqv6?qry=", 23);
			sb.AppendC(name, nameLen);
			sb.AppendC((const UTF8Char*)"\">", 2);
			sb.AppendC(name, nameLen);
			sb.AppendC((const UTF8Char*)"</a>", 4);

			i++;
		}
		sb.Append((const UTF8Char*)"</td><td valign=\"top\">\r\n");

		if (req->GetQueryValueStr((const UTF8Char*)"qry", sbuff, 128))
		{
			Data::ArrayList<Net::DNSClient::RequestAnswer *> ansList;
			Net::DNSClient::RequestAnswer *ans;
			Data::DateTime reqTime;
			UInt32 ttl;
			if (me->analyzer->DNSReqv6GetInfo(sbuff, &ansList, &reqTime, &ttl))
			{
				sb.Append((const UTF8Char*)"<table border=\"1\"><tr><td>Request Name</td><td>");
				sb.Append(sbuff);
				sb.Append((const UTF8Char*)"</td></tr>\r\n");
				sb.Append((const UTF8Char*)"<tr><td>Request Time</td><td>");
				reqTime.ToLocalTime();
				reqTime.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff zzzz");
				sb.Append(sbuff);
				sb.Append((const UTF8Char*)"</td></tr>\r\n");
				sb.Append((const UTF8Char*)"<tr><td>TTL</td><td>");
				sb.AppendU32(ttl);
				sb.Append((const UTF8Char*)"</td></tr>\r\n");
				sb.Append((const UTF8Char*)"<tr><td>Results</td><td>");

				sb.Append((const UTF8Char*)"<table border=\"1\"><tr><td>Name</td><td>Type</td><td>Class</td><td>TTL</td><td>RD</td><tr>\r\n");
				i = 0;
				j = ansList.GetCount();
				while (i < j)
				{
					ans = ansList.GetItem(i);
					sb.Append((const UTF8Char*)"<tr><td>");
					sb.Append(ans->name);
					sb.Append((const UTF8Char*)"</td><td>");
					sb.AppendI32(ans->recType);
					sb.Append((const UTF8Char*)"</td><td>");
					sb.AppendI32(ans->recClass);
					sb.Append((const UTF8Char*)"</td><td>");
					sb.AppendU32(ans->ttl);
					sb.Append((const UTF8Char*)"</td><td>");
					if (ans->rd)
					{
						sb.Append(ans->rd);
					}
					sb.Append((const UTF8Char*)"</td></tr>\r\n");
					i++;
				}
				sb.Append((const UTF8Char*)"</td></tr></table>\r\n");

				Net::DNSClient::FreeAnswers(&ansList);
				sb.Append((const UTF8Char*)"</table>\r\n");
			}
			else
			{
			}
		}

		sb.Append((const UTF8Char*)"</td></tr></table>\r\n");
		AppendFooter(&sb);

		resp->AddDefHeaders(req);
		resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
		resp->AddContentType((const UTF8Char*)"text/html");
		resp->AddContentLength(sb.GetLength());
		resp->Write(sb.ToString(), sb.GetLength());
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
	}
	return true;
}

Bool __stdcall Net::EthernetWebHandler::DNSReqOthReq(EthernetWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	UTF8Char sbuff[128];
	if (req->GetReqMethod() == Net::WebServer::IWebRequest::REQMETH_HTTP_GET)
	{
		UOSInt i;
		UOSInt j;
		Text::StringBuilderUTF8 sb;
		Data::ArrayList<const UTF8Char *> nameList;
		const UTF8Char *name;
		UOSInt nameLen;

		AppendHeader(&sb);
		me->AppendMenu(&sb);

		sb.Append((const UTF8Char*)"<table border=\"1\"><tr>");
		sb.Append((const UTF8Char*)"<td>Req Oth List</td>");
		sb.Append((const UTF8Char*)"<td>Info</td>");
		sb.Append((const UTF8Char*)"</tr>\r\n");
		sb.Append((const UTF8Char*)"<tr><td>\r\n");
		me->analyzer->DNSReqOthGetList(&nameList);

		i = 0;
		j = nameList.GetCount();
		while (i < j)
		{
			name = nameList.GetItem(i);
			nameLen = Text::StrCharCnt(name);
			sb.AllocLeng(33 + nameLen * 2);
			if (i > 0)
			{
				sb.AppendC((const UTF8Char*)"<br/>\r\n", 7);
			}
			sb.AppendC((const UTF8Char*)"<a href=\"/dnsreqoth?qry=", 24);
			sb.AppendC(name, nameLen);
			sb.AppendC((const UTF8Char*)"\">", 2);
			sb.AppendC(name, nameLen);
			sb.AppendC((const UTF8Char*)"</a>", 4);

			i++;
		}
		sb.Append((const UTF8Char*)"</td><td valign=\"top\">\r\n");

		if (req->GetQueryValueStr((const UTF8Char*)"qry", sbuff, 128))
		{
			Data::ArrayList<Net::DNSClient::RequestAnswer *> ansList;
			Net::DNSClient::RequestAnswer *ans;
			Data::DateTime reqTime;
			UInt32 ttl;
			if (me->analyzer->DNSReqOthGetInfo(sbuff, &ansList, &reqTime, &ttl))
			{
				sb.Append((const UTF8Char*)"<table border=\"1\"><tr><td>Request Name</td><td>");
				sb.Append(sbuff);
				sb.Append((const UTF8Char*)"</td></tr>\r\n");
				sb.Append((const UTF8Char*)"<tr><td>Request Time</td><td>");
				reqTime.ToLocalTime();
				reqTime.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff zzzz");
				sb.Append(sbuff);
				sb.Append((const UTF8Char*)"</td></tr>\r\n");
				sb.Append((const UTF8Char*)"<tr><td>TTL</td><td>");
				sb.AppendU32(ttl);
				sb.Append((const UTF8Char*)"</td></tr>\r\n");
				sb.Append((const UTF8Char*)"<tr><td>Results</td><td>");

				sb.Append((const UTF8Char*)"<table border=\"1\"><tr><td>Name</td><td>Type</td><td>Class</td><td>TTL</td><td>RD</td><tr>\r\n");
				i = 0;
				j = ansList.GetCount();
				while (i < j)
				{
					ans = ansList.GetItem(i);
					sb.Append((const UTF8Char*)"<tr><td>");
					sb.Append(ans->name);
					sb.Append((const UTF8Char*)"</td><td>");
					sb.AppendI32(ans->recType);
					sb.Append((const UTF8Char*)"</td><td>");
					sb.AppendI32(ans->recClass);
					sb.Append((const UTF8Char*)"</td><td>");
					sb.AppendU32(ans->ttl);
					sb.Append((const UTF8Char*)"</td><td>");
					if (ans->rd)
					{
						sb.Append(ans->rd);
					}
					sb.Append((const UTF8Char*)"</td></tr>\r\n");
					i++;
				}
				sb.Append((const UTF8Char*)"</td></tr></table>\r\n");

				Net::DNSClient::FreeAnswers(&ansList);
				sb.Append((const UTF8Char*)"</table>\r\n");
			}
			else
			{
			}
		}

		sb.Append((const UTF8Char*)"</td></tr></table>\r\n");
		AppendFooter(&sb);

		resp->AddDefHeaders(req);
		resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
		resp->AddContentType((const UTF8Char*)"text/html");
		resp->AddContentLength(sb.GetLength());
		resp->Write(sb.ToString(), sb.GetLength());
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
	}
	return true;
}

Bool __stdcall Net::EthernetWebHandler::DNSTargetReq(EthernetWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	UTF8Char sbuff[128];
	if (req->GetReqMethod() == Net::WebServer::IWebRequest::REQMETH_HTTP_GET)
	{
		UOSInt i;
		UOSInt j;
		Text::StringBuilderUTF8 sb;
		Data::ArrayList<Net::EthernetAnalyzer::DNSTargetInfo *> targetList;
		Net::EthernetAnalyzer::DNSTargetInfo *target;
		UInt32 targetIP = 0;
		OSInt targetIndex = -1;

		AppendHeader(&sb);
		me->AppendMenu(&sb);

		sb.Append((const UTF8Char*)"<table border=\"0\"><tr><td>DNS Target</td><td>Info</td></tr>\r\n");
		sb.Append((const UTF8Char*)"<tr><td>\r\n");

		if (req->GetQueryValueStr((const UTF8Char*)"qry", sbuff, 128))
		{
			targetIP = Text::StrToUInt32(sbuff);
		}
		me->analyzer->DNSTargetGetList(&targetList);

		i = 0;
		j = targetList.GetCount();
		while (i < j)
		{
			target = targetList.GetItem(i);
			if (i > 0)
			{
				sb.Append((const UTF8Char*)"<br/>\r\n");
			}
			sb.Append((const UTF8Char*)"<a href=\"/dnstarget?qry=");
			sb.AppendU32(target->ip);
			sb.Append((const UTF8Char*)"\">");
			Net::SocketUtil::GetIPv4Name(sbuff, target->ip);
			sb.Append(sbuff);
			sb.Append((const UTF8Char*)"</a>");
			if (target->ip == targetIP)
			{
				targetIndex = (OSInt)i;
			}

			i++;
		}

		sb.Append((const UTF8Char*)"</td><td valign=\"top\">\r\n");

		if (targetIndex >= 0)
		{
			sb.Append((const UTF8Char*)"<h3>");
			Net::SocketUtil::GetIPv4Name(sbuff, targetIP);
			sb.Append(sbuff);
			sb.Append((const UTF8Char*)"</h3>\r\n");

			target = targetList.GetItem((UOSInt)targetIndex);
			Sync::MutexUsage mutUsage(target->mut);
			i = 0;
			j = target->addrList->GetCount();
			while (i < j)
			{
				if (i > 0)
				{
					sb.AppendC((const UTF8Char*)"<br/>\r\n", 7);
				}
				sb.Append(target->addrList->GetItem(i));
				i++;
			}
			mutUsage.EndUse();
		}

		sb.Append((const UTF8Char*)"</td></tr></table>\r\n");
		AppendFooter(&sb);

		resp->AddDefHeaders(req);
		resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
		resp->AddContentType((const UTF8Char*)"text/html");
		resp->AddContentLength(sb.GetLength());
		resp->Write(sb.ToString(), sb.GetLength());
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
	}
	return true;
}

Bool __stdcall Net::EthernetWebHandler::DNSClientReq(EthernetWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	UTF8Char sbuff[128];
	if (req->GetReqMethod() == Net::WebServer::IWebRequest::REQMETH_HTTP_GET)
	{
		UOSInt i;
		UOSInt j;
		OSInt dnsCliInd = -1;
		UInt32 qryVal = 0;
		Data::ArrayList<Net::EthernetAnalyzer::DNSClientInfo*> *dnsCliList;
		Net::EthernetAnalyzer::DNSClientInfo *dnsCli;
		Text::StringBuilderUTF8 sb;
		AppendHeader(&sb);
		me->AppendMenu(&sb);

		sb.Append((const UTF8Char*)"<table border=\"1\"><tr>");
		sb.Append((const UTF8Char*)"<td>DNSClient List</td>");
		sb.Append((const UTF8Char*)"<td>Info</td>");
		sb.Append((const UTF8Char*)"</tr>\r\n");
		sb.Append((const UTF8Char*)"</tr><td>\r\n");
		if (req->GetQueryValueStr((const UTF8Char*)"qry", sbuff, 128))
		{
			qryVal = Text::StrToUInt32(sbuff);
		}
		Sync::MutexUsage mutUsage;
		me->analyzer->UseDNSCli(&mutUsage);
		dnsCliList = me->analyzer->DNSCliGetList();
		i = 0;
		j = dnsCliList->GetCount();
		while (i < j)
		{
			dnsCli = dnsCliList->GetItem(i);
			if (i > 0)
			{
				sb.Append((const UTF8Char*)"<br/>\r\n");
			}
			sb.Append((const UTF8Char*)"<a href=\"dnsclient?qry=");
			sb.AppendU32(dnsCli->cliId);
			sb.Append((const UTF8Char*)"\">");
			Net::SocketUtil::GetAddrName(sbuff, &dnsCli->addr);
			sb.Append(sbuff);
			sb.Append((const UTF8Char*)"</a>");
			if ((UInt32)dnsCli->cliId == qryVal)
			{
				dnsCliInd = (OSInt)i;
			}
			i++;
		}
		sb.Append((const UTF8Char*)"</td><td>\r\n");
		if (dnsCliInd != -1)
		{
			Net::EthernetAnalyzer::DNSCliHourInfo *hourInfo;
			dnsCli = dnsCliList->GetItem((UOSInt)dnsCliInd);
			Net::SocketUtil::GetAddrName(sbuff, &dnsCli->addr);
			sb.Append(sbuff);
			sb.Append((const UTF8Char*)"<br/><table border=\"1\"><tr><td>Time</td><td>Count</td></tr>");
			Sync::MutexUsage mutUsage(dnsCli->mut);
			i = 0;
			j = dnsCli->hourInfos->GetCount();
			while (i < j)
			{
				hourInfo = dnsCli->hourInfos->GetItem(i);
				sb.Append((const UTF8Char*)"<tr><td>");
				sb.AppendI32(hourInfo->year);
				sb.Append((const UTF8Char*)"-");
				sb.AppendI32(hourInfo->month);
				sb.Append((const UTF8Char*)"-");
				sb.AppendI32(hourInfo->day);
				sb.Append((const UTF8Char*)" ");
				sb.AppendI32(hourInfo->hour);
				sb.Append((const UTF8Char*)":00</td><td>");
				sb.AppendU64(hourInfo->reqCount);
				sb.Append((const UTF8Char*)"</td></tr>");
				i++;
			}
			mutUsage.EndUse();
			sb.Append((const UTF8Char*)"</table>");
		}
		sb.Append((const UTF8Char*)"</td></tr>\r\n");
		mutUsage.EndUse();
		sb.Append((const UTF8Char*)"</table>\r\n");
		AppendFooter(&sb);

		resp->AddDefHeaders(req);
		resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
		resp->AddContentType((const UTF8Char*)"text/html");
		resp->AddContentLength(sb.GetLength());
		resp->Write(sb.ToString(), sb.GetLength());
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
	}
	return true;
}

Bool __stdcall Net::EthernetWebHandler::DHCPReq(EthernetWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	UTF8Char sbuff[128];
	if (req->GetReqMethod() == Net::WebServer::IWebRequest::REQMETH_HTTP_GET)
	{
		UOSInt i;
		UOSInt j;
		UOSInt k;
		Data::ArrayList<Net::EthernetAnalyzer::DHCPInfo*> *dhcpList;
		Net::EthernetAnalyzer::DHCPInfo *dhcp;
		const Net::MACInfo::MACEntry *macInfo;
		Text::StringBuilderUTF8 sb;
		Data::DateTime dt;
		AppendHeader(&sb);
		me->AppendMenu(&sb);
		sb.Append((const UTF8Char*)"<table border=\"1\"><tr>");
		sb.Append((const UTF8Char*)"<td>MAC</td>");
		sb.Append((const UTF8Char*)"<td>Vendor</td>");
		sb.Append((const UTF8Char*)"<td>IP Addr</td>");
		sb.Append((const UTF8Char*)"<td>Subnet Mask</td>");
		sb.Append((const UTF8Char*)"<td>Gateway</td>");
		sb.Append((const UTF8Char*)"<td>DNS</td>");
		sb.Append((const UTF8Char*)"<td>DHCP Server</td>");
		sb.Append((const UTF8Char*)"<td>Router</td>");
		sb.Append((const UTF8Char*)"<td>IP Addr Time</td>");
		sb.Append((const UTF8Char*)"<td>IP Addr Lease</td>");
		sb.Append((const UTF8Char*)"<td>Renew Time</td>");
		sb.Append((const UTF8Char*)"<td>Rebind Time</td>");
		sb.Append((const UTF8Char*)"<td>Host Name</td>");
		sb.Append((const UTF8Char*)"<td>Vendor Class</td>");
		sb.Append((const UTF8Char*)"</tr>\r\n");
		Sync::MutexUsage mutUsage;
		me->analyzer->UseDHCP(&mutUsage);
		dhcpList = me->analyzer->DHCPGetList();
		i = 0;
		j = dhcpList->GetCount();
		while (i < j)
		{
			dhcp = dhcpList->GetItem(i);
			Sync::MutexUsage mutUsage(dhcp->mut);
			sb.Append((const UTF8Char*)"<tr><td>");
			WriteMUInt64(sbuff, dhcp->iMAC);
			sb.AppendHexBuff(&sbuff[2], 6, ':', Text::LineBreakType::None);
			sb.Append((const UTF8Char*)"</td><td>");
			macInfo = Net::MACInfo::GetMACInfo(dhcp->iMAC);
			sb.Append((const UTF8Char*)macInfo->name);
			sb.Append((const UTF8Char*)"</td><td>");
			Net::SocketUtil::GetIPv4Name(sbuff, dhcp->ipAddr);
			sb.Append(sbuff);
			sb.Append((const UTF8Char*)"</td><td>");
			Net::SocketUtil::GetIPv4Name(sbuff, dhcp->subnetMask);
			sb.Append(sbuff);
			sb.Append((const UTF8Char*)"</td><td>");
			Net::SocketUtil::GetIPv4Name(sbuff, dhcp->gwAddr);
			sb.Append(sbuff);
			sb.Append((const UTF8Char*)"</td><td>");
			k = 0;
			while (k < 4)
			{
				if (dhcp->dns[k] == 0)
				{
					break;
				}
				if (k > 0)
				{
					sb.Append((const UTF8Char*)"<br/>");
				}
				Net::SocketUtil::GetIPv4Name(sbuff, dhcp->dns[k]);
				sb.Append(sbuff);
				k++;
			}
			sb.Append((const UTF8Char*)"</td><td>");
			Net::SocketUtil::GetIPv4Name(sbuff, dhcp->dhcpServer);
			sb.Append(sbuff);
			sb.Append((const UTF8Char*)"</td><td>");
			Net::SocketUtil::GetIPv4Name(sbuff, dhcp->router);
			sb.Append(sbuff);
			sb.Append((const UTF8Char*)"</td><td>");
			dt.SetTicks(dhcp->ipAddrTime);
			dt.ToLocalTime();
			sb.AppendDate(&dt);
			sb.Append((const UTF8Char*)"</td><td>");
			sb.AppendU32(dhcp->ipAddrLease);
			sb.Append((const UTF8Char*)"</td><td>");
			sb.AppendU32(dhcp->renewTime);
			sb.Append((const UTF8Char*)"</td><td>");
			sb.AppendU32(dhcp->rebindTime);
			sb.Append((const UTF8Char*)"</td><td>");
			if (dhcp->hostName)
				sb.Append(dhcp->hostName);
			sb.Append((const UTF8Char*)"</td><td>");
			if (dhcp->vendorClass)
				sb.Append(dhcp->vendorClass);
			sb.Append((const UTF8Char*)"</td></tr>\r\n");
			mutUsage.EndUse();
			i++;
		}
		mutUsage.EndUse();
		sb.Append((const UTF8Char*)"</table>");
		AppendFooter(&sb);

		resp->AddDefHeaders(req);
		resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
		resp->AddContentType((const UTF8Char*)"text/html");
		resp->AddContentLength(sb.GetLength());
		resp->Write(sb.ToString(), sb.GetLength());
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
	}
	return true;
}

Bool __stdcall Net::EthernetWebHandler::IPLogReq(EthernetWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	UTF8Char sbuff[128];
	if (req->GetReqMethod() == Net::WebServer::IWebRequest::REQMETH_HTTP_GET)
	{
		UOSInt i;
		UOSInt j;
		OSInt ipLogInd = -1;
		UInt32 qryVal = 0;
		Data::ArrayList<Net::EthernetAnalyzer::IPLogInfo*> *ipLogList;
		Net::EthernetAnalyzer::IPLogInfo *ipLog;
		Text::StringBuilderUTF8 sb;
		AppendHeader(&sb);
		me->AppendMenu(&sb);

		sb.Append((const UTF8Char*)"<table border=\"1\"><tr>");
		sb.Append((const UTF8Char*)"<td>IP Log List</td>");
		sb.Append((const UTF8Char*)"<td>Info</td>");
		sb.Append((const UTF8Char*)"</tr>\r\n");
		sb.Append((const UTF8Char*)"</tr><td valign=\"top\">\r\n");
		if (req->GetQueryValueStr((const UTF8Char*)"qry", sbuff, 128))
		{
			qryVal = Text::StrToUInt32(sbuff);
		}
		Sync::MutexUsage mutUsage;
		me->analyzer->UseIPLog(&mutUsage);
		ipLogList = me->analyzer->IPLogGetList();
		i = 0;
		j = ipLogList->GetCount();
		while (i < j)
		{
			ipLog = ipLogList->GetItem(i);
			if (i > 0)
			{
				sb.Append((const UTF8Char*)"<br/>\r\n");
			}
			sb.Append((const UTF8Char*)"<a href=\"iplog?qry=");
			sb.AppendU32(ipLog->ip);
			sb.Append((const UTF8Char*)"\">");
			Net::SocketUtil::GetIPv4Name(sbuff, ipLog->ip);
			sb.Append(sbuff);
			sb.Append((const UTF8Char*)"</a>");
			if (ipLog->ip == qryVal)
			{
				ipLogInd = (OSInt)i;
			}
			i++;
		}
		sb.Append((const UTF8Char*)"</td><td valign=\"top\">\r\n");
		if (ipLogInd != -1)
		{
			ipLog = ipLogList->GetItem((UOSInt)ipLogInd);
			Net::SocketUtil::GetIPv4Name(sbuff, ipLog->ip);
			sb.Append(sbuff);
			Sync::MutexUsage mutUsage(ipLog->mut);
			i = 0;
			j = ipLog->logList->GetCount();
			while (i < j)
			{
				sb.Append((const UTF8Char*)"<br/>");
				sb.Append(ipLog->logList->GetItem(i));
				i++;
			}
			mutUsage.EndUse();
		}
		sb.Append((const UTF8Char*)"</td></tr>\r\n");
		mutUsage.EndUse();
		sb.Append((const UTF8Char*)"</table>\r\n");
		AppendFooter(&sb);

		resp->AddDefHeaders(req);
		resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
		resp->AddContentType((const UTF8Char*)"text/html");
		resp->AddContentLength(sb.GetLength());
		resp->Write(sb.ToString(), sb.GetLength());
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
	}
	return true;
}

Bool Net::EthernetWebHandler::ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq)
{
	if (this->DoRequest(req, resp, subReq))
	{
		return true;
	}
	RequestHandler reqHdlr = this->reqMap->Get(subReq);
	if (reqHdlr)
	{
		return reqHdlr(this, req, resp);
	}
	return this->reqMap->GetValues()->GetItem(0)(this, req, resp);
}

Net::EthernetWebHandler::EthernetWebHandler(Net::EthernetAnalyzer *analyzer)
{
	this->analyzer = analyzer;
	Net::EthernetAnalyzer::AnalyzeType atype = this->analyzer->GetAnalyzeType();
	NEW_CLASS(this->reqMap, Data::StringUTF8Map<RequestHandler>());
	if (atype & Net::EthernetAnalyzer::AT_DEVICE)
		this->reqMap->Put((const UTF8Char*)"/device", DeviceReq);
	if (atype & Net::EthernetAnalyzer::AT_IPTRANSFER)
		this->reqMap->Put((const UTF8Char*)"/iptransfer", IPTransferReq);
	if (atype & Net::EthernetAnalyzer::AT_DNSREQ)
		this->reqMap->Put((const UTF8Char*)"/dnsreqv4", DNSReqv4Req);
	if (atype & Net::EthernetAnalyzer::AT_DNSREQ)
		this->reqMap->Put((const UTF8Char*)"/dnsreqv6", DNSReqv6Req);
	if (atype & Net::EthernetAnalyzer::AT_DNSREQ)
		this->reqMap->Put((const UTF8Char*)"/dnsreqoth", DNSReqOthReq);
	if (atype & Net::EthernetAnalyzer::AT_DNSTARGET)
		this->reqMap->Put((const UTF8Char*)"/dnstarget", DNSTargetReq);
	if (atype & Net::EthernetAnalyzer::AT_DNSCLI)
		this->reqMap->Put((const UTF8Char*)"/dnsclient", DNSClientReq);
	if (atype & Net::EthernetAnalyzer::AT_DHCP)
		this->reqMap->Put((const UTF8Char*)"/dhcp", DHCPReq);
	if (atype & Net::EthernetAnalyzer::AT_IPLOG)
		this->reqMap->Put((const UTF8Char*)"/iplog", IPLogReq);
}

Net::EthernetWebHandler::~EthernetWebHandler()
{
	DEL_CLASS(this->reqMap);
}
