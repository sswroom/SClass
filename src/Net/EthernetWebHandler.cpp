#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Net/EthernetWebHandler.h"
#include "Net/MACInfo.h"
#include "Sync/MutexUsage.h"
#include "Text/StringBuilderUTF8.h"

void Net::EthernetWebHandler::AppendHeader(NN<Text::StringBuilderUTF8> sbOut)
{
	sbOut->AppendC(UTF8STRC("<html><head><title>NetRAWCapture</title></head><body>\r\n"));
}

void Net::EthernetWebHandler::AppendMenu(NN<Text::StringBuilderUTF8> sbOut)
{
	Net::EthernetAnalyzer::AnalyzeType atype = this->analyzer->GetAnalyzeType();
	sbOut->AppendC(UTF8STRC("<table border=\"1\"><tr>"));
	if (atype & Net::EthernetAnalyzer::AT_DEVICE)
		sbOut->AppendC(UTF8STRC("<td><a href=\"/device\">Device</a></td>"));
	if (atype & Net::EthernetAnalyzer::AT_IPTRANSFER)
		sbOut->AppendC(UTF8STRC("<td><a href=\"/iptransfer\">IPTransfer</a></td>"));
	if (atype & Net::EthernetAnalyzer::AT_DNSREQ)
		sbOut->AppendC(UTF8STRC("<td><a href=\"/dnsreqv4\">DNS Req v4</a></td>"));
	if (atype & Net::EthernetAnalyzer::AT_DNSREQ)
		sbOut->AppendC(UTF8STRC("<td><a href=\"/dnsreqv6\">DNS Req v6</a></td>"));
	if (atype & Net::EthernetAnalyzer::AT_DNSREQ)
		sbOut->AppendC(UTF8STRC("<td><a href=\"/dnsreqoth\">DNS Req Oth</a></td>"));
	if (atype & Net::EthernetAnalyzer::AT_DNSTARGET)
		sbOut->AppendC(UTF8STRC("<td><a href=\"/dnstarget\">DNS Target</a></td>"));
	if (atype & Net::EthernetAnalyzer::AT_DNSCLI)
		sbOut->AppendC(UTF8STRC("<td><a href=\"/dnsclient\">DNS Client</a></td>"));
	if (atype & Net::EthernetAnalyzer::AT_DHCP)
		sbOut->AppendC(UTF8STRC("<td><a href=\"/dhcp\">DHCP</a></td>"));
	if (atype & Net::EthernetAnalyzer::AT_IPLOG)
		sbOut->AppendC(UTF8STRC("<td><a href=\"/iplog\">IP Log</a></td>"));
	sbOut->AppendC(UTF8STRC("</tr></table>\r\n"));
}

void Net::EthernetWebHandler::AppendFooter(NN<Text::StringBuilderUTF8> sbOut)
{
	sbOut->AppendC(UTF8STRC("</body></html>"));
}

Bool __stdcall Net::EthernetWebHandler::DeviceReq(NN<EthernetWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	NN<Text::String> s;
	if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_GET)
	{
		UOSInt i;
		UOSInt j;
		NN<const Data::ReadingListNN<Net::EthernetAnalyzer::MACStatus>> macList;
		NN<Net::EthernetAnalyzer::MACStatus> mac;
		NN<const Net::MACInfo::MACEntry> macInfo;
		Text::StringBuilderUTF8 sb;
		AppendHeader(sb);
		me->AppendMenu(sb);
		sb.AppendC(UTF8STRC("Packet cnt = "));
		sb.AppendU64(me->analyzer->GetPacketCnt());
		sb.AppendC(UTF8STRC(", Total size = "));
		sb.AppendU64(me->analyzer->GetPacketTotalSize());
		sb.AppendC(UTF8STRC("<br/>\r\n"));

		sb.AppendC(UTF8STRC("<table border=\"1\"><tr>"));
		sb.AppendC(UTF8STRC("<td>MAC List</td>"));
		sb.AppendC(UTF8STRC("<td>Vendor</td>"));
		sb.AppendC(UTF8STRC("<td>IPv4 Src</td>"));
		sb.AppendC(UTF8STRC("<td>IPv4 Dest</td>"));
		sb.AppendC(UTF8STRC("<td>IPv6 Src</td>"));
		sb.AppendC(UTF8STRC("<td>IPv6 Dest</td>"));
		sb.AppendC(UTF8STRC("<td>Other Src</td>"));
		sb.AppendC(UTF8STRC("<td>Other Dest</td>"));
		sb.AppendC(UTF8STRC("<td>Name</td>"));
		sb.AppendC(UTF8STRC("<td>IP List</td>"));
		sb.AppendC(UTF8STRC("</tr>\r\n"));
		Sync::MutexUsage mutUsage;
		me->analyzer->UseMAC(mutUsage);
		macList = me->analyzer->MACGetList();
		i = 0;
		j = macList->GetCount();
		while (i < j)
		{
			mac = macList->GetItemNoCheck(i);
			sb.AppendC(UTF8STRC("<tr><td>"));
			WriteMUInt64(sbuff, mac->mac64Addr);
			sb.AppendHexBuff(&sbuff[0], 6, ':', Text::LineBreakType::None);
			sb.AppendC(UTF8STRC("</td><td>"));
			macInfo = Net::MACInfo::GetMAC64Info(mac->mac64Addr);
			sb.AppendC(macInfo->name, macInfo->nameLen);
			sb.AppendC(UTF8STRC("</td><td>"));
			sb.AppendU64(mac->ipv4SrcCnt);
			sb.AppendC(UTF8STRC("</td><td>"));
			sb.AppendU64(mac->ipv4DestCnt);
			sb.AppendC(UTF8STRC("</td><td>"));
			sb.AppendU64(mac->ipv6SrcCnt);
			sb.AppendC(UTF8STRC("</td><td>"));
			sb.AppendU64(mac->ipv6DestCnt);
			sb.AppendC(UTF8STRC("</td><td>"));
			sb.AppendU64(mac->othSrcCnt);
			sb.AppendC(UTF8STRC("</td><td>"));
			sb.AppendU64(mac->othDestCnt);
			sb.AppendC(UTF8STRC("</td><td>"));
			if (mac->name.SetTo(s))
			{
				sb.Append(s);
			}
			else
			{
				sb.AppendC(UTF8STRC("Unknown"));
			}
			sb.AppendC(UTF8STRC("</td><td>"));
			
			if (mac->ipv4Addr[0])
			{
				sptr = Net::SocketUtil::GetIPv4Name(sbuff, mac->ipv4Addr[0]);
				sb.AppendP(sbuff, sptr);
				if (mac->ipv4Addr[1])
				{
					sb.AppendC(UTF8STRC("<br/>"));
					sptr = Net::SocketUtil::GetIPv4Name(sbuff, mac->ipv4Addr[1]);
					sb.AppendP(sbuff, sptr);
					if (mac->ipv4Addr[2])
					{
						sb.AppendC(UTF8STRC("<br/>"));
						sptr = Net::SocketUtil::GetIPv4Name(sbuff, mac->ipv4Addr[2]);
						sb.AppendP(sbuff, sptr);
						if (mac->ipv4Addr[3])
						{
							sb.AppendC(UTF8STRC("<br/>"));
							sptr = Net::SocketUtil::GetIPv4Name(sbuff, mac->ipv4Addr[3]);
							sb.AppendP(sbuff, sptr);
						}
					}
				}
				if (mac->ipv6Addr.addrType == Net::AddrType::IPv6)
				{
					sb.AppendC(UTF8STRC("<br/>"));
					sptr = Net::SocketUtil::GetAddrName(sbuff, mac->ipv6Addr).Or(sbuff);
					sb.AppendP(sbuff, sptr);
				}
			}
			else
			{
				if (mac->ipv6Addr.addrType == Net::AddrType::IPv6)
				{
					sptr = Net::SocketUtil::GetAddrName(sbuff, mac->ipv6Addr).Or(sbuff);
					sb.AppendP(sbuff, sptr);
				}
			}
			sb.AppendC(UTF8STRC("</td></tr>\r\n"));
			i++;
		}
		mutUsage.EndUse();
		sb.AppendC(UTF8STRC("</table>"));
		AppendFooter(sb);

		resp->AddDefHeaders(req);
		resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
		resp->AddContentType(CSTR("text/html"));
		resp->AddContentLength(sb.GetLength());
		resp->Write(sb.ToByteArray());
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
	}
	return true;
}

Bool __stdcall Net::EthernetWebHandler::IPTransferReq(NN<EthernetWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_GET)
	{
		UOSInt i;
		UOSInt j;
		NN<const Data::ReadingListNN<Net::EthernetAnalyzer::IPTranStatus>> ipTranList;
		NN<Net::EthernetAnalyzer::IPTranStatus> ipTran;
		Text::StringBuilderUTF8 sb;
		AppendHeader(sb);
		me->AppendMenu(sb);

		sb.AppendC(UTF8STRC("<table border=\"1\"><tr>"));
		sb.AppendC(UTF8STRC("<td>IP List</td>"));
		sb.AppendC(UTF8STRC("<td>TCP Cnt</td>"));
		sb.AppendC(UTF8STRC("<td>UDP Cnt</td>"));
		sb.AppendC(UTF8STRC("<td>ICMP Cnt</td>"));
		sb.AppendC(UTF8STRC("<td>Other Cnt</td>"));
		sb.AppendC(UTF8STRC("</tr>\r\n"));
		Sync::MutexUsage mutUsage;
		me->analyzer->UseIPTran(mutUsage);
		ipTranList = me->analyzer->IPTranGetList();
		i = 0;
		j = ipTranList->GetCount();
		while (i < j)
		{
			ipTran = ipTranList->GetItemNoCheck(i);
			sb.AppendC(UTF8STRC("<tr><td>"));
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, ipTran->srcIP);
			sb.AppendP(sbuff, sptr);
			sb.AppendC(UTF8STRC(" -> "));
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, ipTran->destIP);
			sb.AppendP(sbuff, sptr);
			sb.AppendC(UTF8STRC("</td><td>"));
			sb.AppendU64(ipTran->tcpCnt);
			sb.AppendC(UTF8STRC("</td><td>"));
			sb.AppendU64(ipTran->udpCnt);
			sb.AppendC(UTF8STRC("</td><td>"));
			sb.AppendU64(ipTran->icmpCnt);
			sb.AppendC(UTF8STRC("</td><td>"));
			sb.AppendU64(ipTran->otherCnt);
			sb.AppendC(UTF8STRC("</td></tr>\r\n"));
			i++;
		}
		mutUsage.EndUse();
		sb.AppendC(UTF8STRC("</table>"));
		AppendFooter(sb);

		resp->AddDefHeaders(req);
		resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
		resp->AddContentType(CSTR("text/html"));
		resp->AddContentLength(sb.GetLength());
		resp->Write(sb.ToByteArray());
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
	}
	return true;
}

Bool __stdcall Net::EthernetWebHandler::DNSReqv4Req(NN<EthernetWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_GET)
	{
		UOSInt i;
		UOSInt j;
		Text::StringBuilderUTF8 sb;
		Data::ArrayListNN<Text::String> nameList;
		NN<Text::String> name;
		UOSInt nameLen;

		AppendHeader(sb);
		me->AppendMenu(sb);

		sb.AppendC(UTF8STRC("<table border=\"1\"><tr>"));
		sb.AppendC(UTF8STRC("<td>Reqv4 List</td>"));
		sb.AppendC(UTF8STRC("<td>Info</td>"));
		sb.AppendC(UTF8STRC("</tr>\r\n"));
		sb.AppendC(UTF8STRC("<tr><td>\r\n"));
		me->analyzer->DNSReqv4GetList(nameList);

		i = 0;
		j = nameList.GetCount();
		while (i < j)
		{
			name = nameList.GetItemNoCheck(i);
			nameLen = name->leng;
			sb.AllocLeng(33 + nameLen * 2);
			if (i > 0)
			{
				sb.AppendC(UTF8STRC("<br/>\r\n"));
			}
			sb.AppendC(UTF8STRC("<a href=\"/dnsreqv4?qry="));
			sb.Append(name);
			sb.AppendC(UTF8STRC("\">"));
			sb.Append(name);
			sb.AppendC(UTF8STRC("</a>"));

			i++;
		}
		sb.AppendC(UTF8STRC("</td><td valign=\"top\">\r\n"));

		if (req->GetQueryValueStr(CSTR("qry"), sbuff, 128).SetTo(sptr))
		{
			Data::ArrayListNN<Net::DNSClient::RequestAnswer> ansList;
			NN<Net::DNSClient::RequestAnswer> ans;
			Data::DateTime reqTime;
			UInt32 ttl;
			if (me->analyzer->DNSReqv4GetInfo(CSTRP(sbuff, sptr), ansList, reqTime, ttl))
			{
				sb.AppendC(UTF8STRC("<table border=\"1\"><tr><td>Request Name</td><td>"));
				sb.AppendP(sbuff, sptr);
				sb.AppendC(UTF8STRC("</td></tr>\r\n"));
				sb.AppendC(UTF8STRC("<tr><td>Request Time</td><td>"));
				reqTime.ToLocalTime();
				sptr = reqTime.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff zzzz");
				sb.AppendP(sbuff, sptr);
				sb.AppendC(UTF8STRC("</td></tr>\r\n"));
				sb.AppendC(UTF8STRC("<tr><td>TTL</td><td>"));
				sb.AppendU32(ttl);
				sb.AppendC(UTF8STRC("</td></tr>\r\n"));
				sb.AppendC(UTF8STRC("<tr><td>Results</td><td>"));

				sb.AppendC(UTF8STRC("<table border=\"1\"><tr><td>Name</td><td>Type</td><td>Class</td><td>TTL</td><td>RD</td><tr>\r\n"));
				i = 0;
				j = ansList.GetCount();
				while (i < j)
				{
					ans = ansList.GetItemNoCheck(i);
					sb.AppendC(UTF8STRC("<tr><td>"));
					sb.Append(ans->name);
					sb.AppendC(UTF8STRC("</td><td>"));
					sb.AppendI32(ans->recType);
					sb.AppendC(UTF8STRC("</td><td>"));
					sb.AppendI32(ans->recClass);
					sb.AppendC(UTF8STRC("</td><td>"));
					sb.AppendU32(ans->ttl);
					sb.AppendC(UTF8STRC("</td><td>"));
					if (ans->rd)
					{
						sb.Append(ans->rd);
					}
					sb.AppendC(UTF8STRC("</td></tr>\r\n"));
					i++;
				}
				sb.AppendC(UTF8STRC("</td></tr></table>\r\n"));

				Net::DNSClient::FreeAnswers(ansList);
				sb.AppendC(UTF8STRC("</table>\r\n"));
			}
			else
			{
			}
		}

		sb.AppendC(UTF8STRC("</td></tr></table>\r\n"));
		AppendFooter(sb);

		resp->AddDefHeaders(req);
		resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
		resp->AddContentType(CSTR("text/html"));
		resp->AddContentLength(sb.GetLength());
		resp->Write(sb.ToByteArray());
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
	}
	return true;
}

Bool __stdcall Net::EthernetWebHandler::DNSReqv6Req(NN<EthernetWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_GET)
	{
		UOSInt i;
		UOSInt j;
		Text::StringBuilderUTF8 sb;
		Data::ArrayListNN<Text::String> nameList;
		NN<Text::String> name;
		UOSInt nameLen;

		AppendHeader(sb);
		me->AppendMenu(sb);

		sb.AppendC(UTF8STRC("<table border=\"1\"><tr>"));
		sb.AppendC(UTF8STRC("<td>Reqv6 List</td>"));
		sb.AppendC(UTF8STRC("<td>Info</td>"));
		sb.AppendC(UTF8STRC("</tr>\r\n"));
		sb.AppendC(UTF8STRC("<tr><td>\r\n"));
		me->analyzer->DNSReqv6GetList(nameList);

		i = 0;
		j = nameList.GetCount();
		while (i < j)
		{
			name = nameList.GetItemNoCheck(i);
			nameLen = name->leng;
			sb.AllocLeng(33 + nameLen * 2);
			if (i > 0)
			{
				sb.AppendC(UTF8STRC("<br/>\r\n"));
			}
			sb.AppendC(UTF8STRC("<a href=\"/dnsreqv6?qry="));
			sb.Append(name);
			sb.AppendC(UTF8STRC("\">"));
			sb.Append(name);
			sb.AppendC(UTF8STRC("</a>"));

			i++;
		}
		sb.AppendC(UTF8STRC("</td><td valign=\"top\">\r\n"));

		if (req->GetQueryValueStr(CSTR("qry"), sbuff, 128).SetTo(sptr))
		{
			Data::ArrayListNN<Net::DNSClient::RequestAnswer> ansList;
			NN<Net::DNSClient::RequestAnswer> ans;
			Data::DateTime reqTime;
			UInt32 ttl;
			if (me->analyzer->DNSReqv6GetInfo(CSTRP(sbuff, sptr), ansList, reqTime, ttl))
			{
				sb.AppendC(UTF8STRC("<table border=\"1\"><tr><td>Request Name</td><td>"));
				sb.AppendP(sbuff, sptr);
				sb.AppendC(UTF8STRC("</td></tr>\r\n"));
				sb.AppendC(UTF8STRC("<tr><td>Request Time</td><td>"));
				reqTime.ToLocalTime();
				sptr = reqTime.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff zzzz");
				sb.AppendP(sbuff, sptr);
				sb.AppendC(UTF8STRC("</td></tr>\r\n"));
				sb.AppendC(UTF8STRC("<tr><td>TTL</td><td>"));
				sb.AppendU32(ttl);
				sb.AppendC(UTF8STRC("</td></tr>\r\n"));
				sb.AppendC(UTF8STRC("<tr><td>Results</td><td>"));

				sb.AppendC(UTF8STRC("<table border=\"1\"><tr><td>Name</td><td>Type</td><td>Class</td><td>TTL</td><td>RD</td><tr>\r\n"));
				i = 0;
				j = ansList.GetCount();
				while (i < j)
				{
					ans = ansList.GetItemNoCheck(i);
					sb.AppendC(UTF8STRC("<tr><td>"));
					sb.Append(ans->name);
					sb.AppendC(UTF8STRC("</td><td>"));
					sb.AppendI32(ans->recType);
					sb.AppendC(UTF8STRC("</td><td>"));
					sb.AppendI32(ans->recClass);
					sb.AppendC(UTF8STRC("</td><td>"));
					sb.AppendU32(ans->ttl);
					sb.AppendC(UTF8STRC("</td><td>"));
					if (ans->rd)
					{
						sb.Append(ans->rd);
					}
					sb.AppendC(UTF8STRC("</td></tr>\r\n"));
					i++;
				}
				sb.AppendC(UTF8STRC("</td></tr></table>\r\n"));

				Net::DNSClient::FreeAnswers(ansList);
				sb.AppendC(UTF8STRC("</table>\r\n"));
			}
			else
			{
			}
		}

		sb.AppendC(UTF8STRC("</td></tr></table>\r\n"));
		AppendFooter(sb);

		resp->AddDefHeaders(req);
		resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
		resp->AddContentType(CSTR("text/html"));
		resp->AddContentLength(sb.GetLength());
		resp->Write(sb.ToByteArray());
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
	}
	return true;
}

Bool __stdcall Net::EthernetWebHandler::DNSReqOthReq(NN<EthernetWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_GET)
	{
		UOSInt i;
		UOSInt j;
		Text::StringBuilderUTF8 sb;
		Data::ArrayListNN<Text::String> nameList;
		NN<Text::String> name;
		UOSInt nameLen;

		AppendHeader(sb);
		me->AppendMenu(sb);

		sb.AppendC(UTF8STRC("<table border=\"1\"><tr>"));
		sb.AppendC(UTF8STRC("<td>Req Oth List</td>"));
		sb.AppendC(UTF8STRC("<td>Info</td>"));
		sb.AppendC(UTF8STRC("</tr>\r\n"));
		sb.AppendC(UTF8STRC("<tr><td>\r\n"));
		me->analyzer->DNSReqOthGetList(nameList);

		i = 0;
		j = nameList.GetCount();
		while (i < j)
		{
			name = nameList.GetItemNoCheck(i);
			nameLen = name->leng;
			sb.AllocLeng(33 + nameLen * 2);
			if (i > 0)
			{
				sb.AppendC(UTF8STRC("<br/>\r\n"));
			}
			sb.AppendC(UTF8STRC("<a href=\"/dnsreqoth?qry="));
			sb.Append(name);
			sb.AppendC(UTF8STRC("\">"));
			sb.Append(name);
			sb.AppendC(UTF8STRC("</a>"));

			i++;
		}
		sb.AppendC(UTF8STRC("</td><td valign=\"top\">\r\n"));

		if (req->GetQueryValueStr(CSTR("qry"), sbuff, 128).SetTo(sptr))
		{
			Data::ArrayListNN<Net::DNSClient::RequestAnswer> ansList;
			NN<Net::DNSClient::RequestAnswer> ans;
			Data::DateTime reqTime;
			UInt32 ttl;
			if (me->analyzer->DNSReqOthGetInfo(CSTRP(sbuff, sptr), ansList, reqTime, ttl))
			{
				sb.AppendC(UTF8STRC("<table border=\"1\"><tr><td>Request Name</td><td>"));
				sb.AppendP(sbuff, sptr);
				sb.AppendC(UTF8STRC("</td></tr>\r\n"));
				sb.AppendC(UTF8STRC("<tr><td>Request Time</td><td>"));
				reqTime.ToLocalTime();
				sptr = reqTime.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff zzzz");
				sb.AppendP(sbuff, sptr);
				sb.AppendC(UTF8STRC("</td></tr>\r\n"));
				sb.AppendC(UTF8STRC("<tr><td>TTL</td><td>"));
				sb.AppendU32(ttl);
				sb.AppendC(UTF8STRC("</td></tr>\r\n"));
				sb.AppendC(UTF8STRC("<tr><td>Results</td><td>"));

				sb.AppendC(UTF8STRC("<table border=\"1\"><tr><td>Name</td><td>Type</td><td>Class</td><td>TTL</td><td>RD</td><tr>\r\n"));
				i = 0;
				j = ansList.GetCount();
				while (i < j)
				{
					ans = ansList.GetItemNoCheck(i);
					sb.AppendC(UTF8STRC("<tr><td>"));
					sb.Append(ans->name);
					sb.AppendC(UTF8STRC("</td><td>"));
					sb.AppendI32(ans->recType);
					sb.AppendC(UTF8STRC("</td><td>"));
					sb.AppendI32(ans->recClass);
					sb.AppendC(UTF8STRC("</td><td>"));
					sb.AppendU32(ans->ttl);
					sb.AppendC(UTF8STRC("</td><td>"));
					if (ans->rd)
					{
						sb.Append(ans->rd);
					}
					sb.AppendC(UTF8STRC("</td></tr>\r\n"));
					i++;
				}
				sb.AppendC(UTF8STRC("</td></tr></table>\r\n"));

				Net::DNSClient::FreeAnswers(ansList);
				sb.AppendC(UTF8STRC("</table>\r\n"));
			}
			else
			{
			}
		}

		sb.AppendC(UTF8STRC("</td></tr></table>\r\n"));
		AppendFooter(sb);

		resp->AddDefHeaders(req);
		resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
		resp->AddContentType(CSTR("text/html"));
		resp->AddContentLength(sb.GetLength());
		resp->Write(sb.ToByteArray());
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
	}
	return true;
}

Bool __stdcall Net::EthernetWebHandler::DNSTargetReq(NN<EthernetWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_GET)
	{
		UOSInt i;
		UOSInt j;
		Text::StringBuilderUTF8 sb;
		Data::ArrayListNN<Net::EthernetAnalyzer::DNSTargetInfo> targetList;
		NN<Net::EthernetAnalyzer::DNSTargetInfo> target;
		UInt32 targetIP = 0;
		OSInt targetIndex = -1;

		AppendHeader(sb);
		me->AppendMenu(sb);

		sb.AppendC(UTF8STRC("<table border=\"0\"><tr><td>DNS Target</td><td>Info</td></tr>\r\n"));
		sb.AppendC(UTF8STRC("<tr><td>\r\n"));

		if (req->GetQueryValueStr(CSTR("qry"), sbuff, 128).NotNull())
		{
			targetIP = Text::StrToUInt32(sbuff);
		}
		me->analyzer->DNSTargetGetList(targetList);

		i = 0;
		j = targetList.GetCount();
		while (i < j)
		{
			target = targetList.GetItemNoCheck(i);
			if (i > 0)
			{
				sb.AppendC(UTF8STRC("<br/>\r\n"));
			}
			sb.AppendC(UTF8STRC("<a href=\"/dnstarget?qry="));
			sb.AppendU32(target->ip);
			sb.AppendC(UTF8STRC("\">"));
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, target->ip);
			sb.AppendP(sbuff, sptr);
			sb.AppendC(UTF8STRC("</a>"));
			if (target->ip == targetIP)
			{
				targetIndex = (OSInt)i;
			}

			i++;
		}

		sb.AppendC(UTF8STRC("</td><td valign=\"top\">\r\n"));

		if (targetIndex >= 0)
		{
			sb.AppendC(UTF8STRC("<h3>"));
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, targetIP);
			sb.AppendP(sbuff, sptr);
			sb.AppendC(UTF8STRC("</h3>\r\n"));

			target = targetList.GetItemNoCheck((UOSInt)targetIndex);
			Sync::MutexUsage mutUsage(target->mut);
			i = 0;
			j = target->addrList.GetCount();
			while (i < j)
			{
				if (i > 0)
				{
					sb.AppendC(UTF8STRC("<br/>\r\n"));
				}
				sb.Append(target->addrList.GetItemNoCheck(i));
				i++;
			}
		}

		sb.AppendC(UTF8STRC("</td></tr></table>\r\n"));
		AppendFooter(sb);

		resp->AddDefHeaders(req);
		resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
		resp->AddContentType(CSTR("text/html"));
		resp->AddContentLength(sb.GetLength());
		resp->Write(sb.ToByteArray());
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
	}
	return true;
}

Bool __stdcall Net::EthernetWebHandler::DNSClientReq(NN<EthernetWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_GET)
	{
		UOSInt i;
		UOSInt j;
		OSInt dnsCliInd = -1;
		UInt32 qryVal = 0;
		NN<const Data::ReadingListNN<Net::EthernetAnalyzer::DNSClientInfo>> dnsCliList;
		NN<Net::EthernetAnalyzer::DNSClientInfo> dnsCli;
		Text::StringBuilderUTF8 sb;
		AppendHeader(sb);
		me->AppendMenu(sb);

		sb.AppendC(UTF8STRC("<table border=\"1\"><tr>"));
		sb.AppendC(UTF8STRC("<td>DNSClient List</td>"));
		sb.AppendC(UTF8STRC("<td>Info</td>"));
		sb.AppendC(UTF8STRC("</tr>\r\n"));
		sb.AppendC(UTF8STRC("</tr><td>\r\n"));
		if (req->GetQueryValueStr(CSTR("qry"), sbuff, 128).NotNull())
		{
			qryVal = Text::StrToUInt32(sbuff);
		}
		Sync::MutexUsage mutUsage;
		me->analyzer->UseDNSCli(mutUsage);
		dnsCliList = me->analyzer->DNSCliGetList();
		i = 0;
		j = dnsCliList->GetCount();
		while (i < j)
		{
			dnsCli = dnsCliList->GetItemNoCheck(i);
			if (i > 0)
			{
				sb.AppendC(UTF8STRC("<br/>\r\n"));
			}
			sb.AppendC(UTF8STRC("<a href=\"dnsclient?qry="));
			sb.AppendU32(dnsCli->cliId);
			sb.AppendC(UTF8STRC("\">"));
			sptr = Net::SocketUtil::GetAddrName(sbuff, dnsCli->addr).Or(sbuff);
			sb.AppendP(sbuff, sptr);
			sb.AppendC(UTF8STRC("</a>"));
			if ((UInt32)dnsCli->cliId == qryVal)
			{
				dnsCliInd = (OSInt)i;
			}
			i++;
		}
		sb.AppendC(UTF8STRC("</td><td>\r\n"));
		if (dnsCliInd != -1)
		{
			NN<Net::EthernetAnalyzer::DNSCliHourInfo> hourInfo;
			dnsCli = dnsCliList->GetItemNoCheck((UOSInt)dnsCliInd);
			sptr = Net::SocketUtil::GetAddrName(sbuff, dnsCli->addr).Or(sbuff);
			sb.AppendP(sbuff, sptr);
			sb.AppendC(UTF8STRC("<br/><table border=\"1\"><tr><td>Time</td><td>Count</td></tr>"));
			Sync::MutexUsage mutUsage(dnsCli->mut);
			i = 0;
			j = dnsCli->hourInfos.GetCount();
			while (i < j)
			{
				hourInfo = dnsCli->hourInfos.GetItemNoCheck(i);
				sb.AppendC(UTF8STRC("<tr><td>"));
				sb.AppendI32(hourInfo->year);
				sb.AppendC(UTF8STRC("-"));
				sb.AppendI32(hourInfo->month);
				sb.AppendC(UTF8STRC("-"));
				sb.AppendI32(hourInfo->day);
				sb.AppendC(UTF8STRC(" "));
				sb.AppendI32(hourInfo->hour);
				sb.AppendC(UTF8STRC(":00</td><td>"));
				sb.AppendU64(hourInfo->reqCount);
				sb.AppendC(UTF8STRC("</td></tr>"));
				i++;
			}
			mutUsage.EndUse();
			sb.AppendC(UTF8STRC("</table>"));
		}
		sb.AppendC(UTF8STRC("</td></tr>\r\n"));
		mutUsage.EndUse();
		sb.AppendC(UTF8STRC("</table>\r\n"));
		AppendFooter(sb);

		resp->AddDefHeaders(req);
		resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
		resp->AddContentType(CSTR("text/html"));
		resp->AddContentLength(sb.GetLength());
		resp->Write(sb.ToByteArray());
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
	}
	return true;
}

Bool __stdcall Net::EthernetWebHandler::DHCPReq(NN<EthernetWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_GET)
	{
		UOSInt i;
		UOSInt j;
		UOSInt k;
		NN<const Data::ReadingListNN<Net::EthernetAnalyzer::DHCPInfo>> dhcpList;
		NN<Net::EthernetAnalyzer::DHCPInfo> dhcp;
		NN<const Net::MACInfo::MACEntry> macInfo;
		Text::StringBuilderUTF8 sb;
		AppendHeader(sb);
		me->AppendMenu(sb);
		sb.AppendC(UTF8STRC("<table border=\"1\"><tr>"));
		sb.AppendC(UTF8STRC("<td>MAC</td>"));
		sb.AppendC(UTF8STRC("<td>Vendor</td>"));
		sb.AppendC(UTF8STRC("<td>IP Addr</td>"));
		sb.AppendC(UTF8STRC("<td>Subnet Mask</td>"));
		sb.AppendC(UTF8STRC("<td>Gateway</td>"));
		sb.AppendC(UTF8STRC("<td>DNS</td>"));
		sb.AppendC(UTF8STRC("<td>DHCP Server</td>"));
		sb.AppendC(UTF8STRC("<td>Router</td>"));
		sb.AppendC(UTF8STRC("<td>IP Addr Time</td>"));
		sb.AppendC(UTF8STRC("<td>IP Addr Lease</td>"));
		sb.AppendC(UTF8STRC("<td>Renew Time</td>"));
		sb.AppendC(UTF8STRC("<td>Rebind Time</td>"));
		sb.AppendC(UTF8STRC("<td>Host Name</td>"));
		sb.AppendC(UTF8STRC("<td>Vendor Class</td>"));
		sb.AppendC(UTF8STRC("</tr>\r\n"));
		Sync::MutexUsage mutUsage;
		me->analyzer->UseDHCP(mutUsage);
		dhcpList = me->analyzer->DHCPGetList();
		i = 0;
		j = dhcpList->GetCount();
		while (i < j)
		{
			dhcp = dhcpList->GetItemNoCheck(i);
			Sync::MutexUsage mutUsage(dhcp->mut);
			sb.AppendC(UTF8STRC("<tr><td>"));
			WriteMUInt64(sbuff, dhcp->iMAC64);
			sb.AppendHexBuff(&sbuff[0], 6, ':', Text::LineBreakType::None);
			sb.AppendC(UTF8STRC("</td><td>"));
			macInfo = Net::MACInfo::GetMAC64Info(dhcp->iMAC64);
			sb.AppendC(macInfo->name, macInfo->nameLen);
			sb.AppendC(UTF8STRC("</td><td>"));
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, dhcp->ipAddr);
			sb.AppendP(sbuff, sptr);
			sb.AppendC(UTF8STRC("</td><td>"));
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, dhcp->subnetMask);
			sb.AppendP(sbuff, sptr);
			sb.AppendC(UTF8STRC("</td><td>"));
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, dhcp->gwAddr);
			sb.AppendP(sbuff, sptr);
			sb.AppendC(UTF8STRC("</td><td>"));
			k = 0;
			while (k < 4)
			{
				if (dhcp->dns[k] == 0)
				{
					break;
				}
				if (k > 0)
				{
					sb.AppendC(UTF8STRC("<br/>"));
				}
				sptr = Net::SocketUtil::GetIPv4Name(sbuff, dhcp->dns[k]);
				sb.AppendP(sbuff, sptr);
				k++;
			}
			sb.AppendC(UTF8STRC("</td><td>"));
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, dhcp->dhcpServer);
			sb.AppendP(sbuff, sptr);
			sb.AppendC(UTF8STRC("</td><td>"));
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, dhcp->router);
			sb.AppendP(sbuff, sptr);
			sb.AppendC(UTF8STRC("</td><td>"));
			sb.AppendTSNoZone(dhcp->ipAddrTime.ToLocalTime());
			sb.AppendC(UTF8STRC("</td><td>"));
			sb.AppendU32(dhcp->ipAddrLease);
			sb.AppendC(UTF8STRC("</td><td>"));
			sb.AppendU32(dhcp->renewTime);
			sb.AppendC(UTF8STRC("</td><td>"));
			sb.AppendU32(dhcp->rebindTime);
			sb.AppendC(UTF8STRC("</td><td>"));
			sb.AppendOpt(dhcp->hostName);
			sb.AppendC(UTF8STRC("</td><td>"));
			sb.AppendOpt(dhcp->vendorClass);
			sb.AppendC(UTF8STRC("</td></tr>\r\n"));
			mutUsage.EndUse();
			i++;
		}
		mutUsage.EndUse();
		sb.AppendC(UTF8STRC("</table>"));
		AppendFooter(sb);

		resp->AddDefHeaders(req);
		resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
		resp->AddContentType(CSTR("text/html"));
		resp->AddContentLength(sb.GetLength());
		resp->Write(sb.ToByteArray());
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
	}
	return true;
}

Bool __stdcall Net::EthernetWebHandler::IPLogReq(NN<EthernetWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_GET)
	{
		UOSInt i;
		UOSInt j;
		OSInt ipLogInd = -1;
		UInt32 qryVal = 0;
		NN<const Data::ReadingListNN<Net::EthernetAnalyzer::IPLogInfo>> ipLogList;
		NN<Net::EthernetAnalyzer::IPLogInfo> ipLog;
		Text::StringBuilderUTF8 sb;
		AppendHeader(sb);
		me->AppendMenu(sb);

		sb.AppendC(UTF8STRC("<table border=\"1\"><tr>"));
		sb.AppendC(UTF8STRC("<td>IP Log List</td>"));
		sb.AppendC(UTF8STRC("<td>Info</td>"));
		sb.AppendC(UTF8STRC("</tr>\r\n"));
		sb.AppendC(UTF8STRC("</tr><td valign=\"top\">\r\n"));
		if (req->GetQueryValueStr(CSTR("qry"), sbuff, 128).NotNull())
		{
			qryVal = Text::StrToUInt32(sbuff);
		}
		Sync::MutexUsage mutUsage;
		me->analyzer->UseIPLog(mutUsage);
		ipLogList = me->analyzer->IPLogGetList();
		i = 0;
		j = ipLogList->GetCount();
		while (i < j)
		{
			ipLog = ipLogList->GetItemNoCheck(i);
			if (i > 0)
			{
				sb.AppendC(UTF8STRC("<br/>\r\n"));
			}
			sb.AppendC(UTF8STRC("<a href=\"iplog?qry="));
			sb.AppendU32(ipLog->ip);
			sb.AppendC(UTF8STRC("\">"));
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, ipLog->ip);
			sb.AppendP(sbuff, sptr);
			sb.AppendC(UTF8STRC("</a>"));
			if (ipLog->ip == qryVal)
			{
				ipLogInd = (OSInt)i;
			}
			i++;
		}
		sb.AppendC(UTF8STRC("</td><td valign=\"top\">\r\n"));
		if (ipLogInd != -1)
		{
			ipLog = ipLogList->GetItemNoCheck((UOSInt)ipLogInd);
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, ipLog->ip);
			sb.AppendP(sbuff, sptr);
			Sync::MutexUsage mutUsage(ipLog->mut);
			Data::ArrayIterator<NN<Text::String>> it = ipLog->logList.Iterator();
			while (it.HasNext())
			{
				sb.AppendC(UTF8STRC("<br/>"));
				sb.Append(it.Next());
			}
		}
		sb.AppendC(UTF8STRC("</td></tr>\r\n"));
		mutUsage.EndUse();
		sb.AppendC(UTF8STRC("</table>\r\n"));
		AppendFooter(sb);

		resp->AddDefHeaders(req);
		resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
		resp->AddContentType(CSTR("text/html"));
		resp->AddContentLength(sb.GetLength());
		resp->Write(sb.ToByteArray());
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
	}
	return true;
}

Bool Net::EthernetWebHandler::ProcessRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq)
{
	if (this->DoRequest(req, resp, subReq))
	{
		return true;
	}
	RequestHandler reqHdlr = this->reqMap.GetC(subReq);
	if (reqHdlr)
	{
		return reqHdlr(*this, req, resp);
	}
	return this->reqMap.GetItem(0)(*this, req, resp);
}

Net::EthernetWebHandler::EthernetWebHandler(NN<Net::EthernetAnalyzer> analyzer)
{
	this->analyzer = analyzer;
	Net::EthernetAnalyzer::AnalyzeType atype = this->analyzer->GetAnalyzeType();
	if (atype & Net::EthernetAnalyzer::AT_DEVICE)
		this->reqMap.PutC(CSTR("/device"), DeviceReq);
	if (atype & Net::EthernetAnalyzer::AT_IPTRANSFER)
		this->reqMap.PutC(CSTR("/iptransfer"), IPTransferReq);
	if (atype & Net::EthernetAnalyzer::AT_DNSREQ)
		this->reqMap.PutC(CSTR("/dnsreqv4"), DNSReqv4Req);
	if (atype & Net::EthernetAnalyzer::AT_DNSREQ)
		this->reqMap.PutC(CSTR("/dnsreqv6"), DNSReqv6Req);
	if (atype & Net::EthernetAnalyzer::AT_DNSREQ)
		this->reqMap.PutC(CSTR("/dnsreqoth"), DNSReqOthReq);
	if (atype & Net::EthernetAnalyzer::AT_DNSTARGET)
		this->reqMap.PutC(CSTR("/dnstarget"), DNSTargetReq);
	if (atype & Net::EthernetAnalyzer::AT_DNSCLI)
		this->reqMap.PutC(CSTR("/dnsclient"), DNSClientReq);
	if (atype & Net::EthernetAnalyzer::AT_DHCP)
		this->reqMap.PutC(CSTR("/dhcp"), DHCPReq);
	if (atype & Net::EthernetAnalyzer::AT_IPLOG)
		this->reqMap.PutC(CSTR("/iplog"), IPLogReq);
}

Net::EthernetWebHandler::~EthernetWebHandler()
{
}
