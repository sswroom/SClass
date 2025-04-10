#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "SSWR/SDNSProxy/SDNSProxyCore.h"
#include "SSWR/SDNSProxy/SDNSProxyWebHandler.h"
#include "Sync/MutexUsage.h"
#include "Text/StringBuilderUTF8.h"

#define LOGSIZE 300

Bool __stdcall SSWR::SDNSProxy::SDNSProxyWebHandler::StatusReq(NN<SSWR::SDNSProxy::SDNSProxyWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	Text::StringBuilderUTF8 sbOut;
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	AppendHeader(sbOut);
	AppendMenu(sbOut);

	sbOut.AppendC(UTF8STRC("<h2>Status</h2>\r\n"));

	sbOut.AppendC(UTF8STRC("<table border=\"1\">\r\n"));
	sbOut.AppendC(UTF8STRC("<tr><td>Listening Port</td><td>53</td></tr>\r\n"));
	sbOut.AppendC(UTF8STRC("<tr><td>Disable IPv6</td><td>"));
	if (me->proxy->IsDisableV6())
	{
		sbOut.AppendC(UTF8STRC("Yes"));
	}
	else
	{
		sbOut.AppendC(UTF8STRC("No"));
	}
	sbOut.AppendC(UTF8STRC("</td></tr>\r\n"));
	sbOut.AppendC(UTF8STRC("<tr><td>DNS Server</td><td>"));
	sptr = Net::SocketUtil::GetIPv4Name(sbuff, me->proxy->GetServerIP());
	sbOut.AppendP(sbuff, sptr);
	sbOut.AppendC(UTF8STRC("</td></tr>\r\n"));
	sbOut.AppendC(UTF8STRC("<tr><td>DNS List</td><td>"));
	Data::ArrayList<UInt32> dnsList;
	UOSInt i;
	UOSInt j;
	me->proxy->GetDNSList(dnsList);
	i = 0;
	j = dnsList.GetCount();
	while (i < j)
	{
		if (i > 0)
		{
			sbOut.AppendC(UTF8STRC("<br/>\r\n"));
		}
		sptr = Net::SocketUtil::GetIPv4Name(sbuff, dnsList.GetItem(i));
		sbOut.AppendP(sbuff, sptr);
		i++;
	}
	sbOut.AppendC(UTF8STRC("</td></tr>\r\n"));
	sbOut.AppendC(UTF8STRC("</table>"));
	AppendFooter(sbOut);

	resp->SetStatusCode(Net::WebStatus::SC_OK);
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("text/html; charset=UTF-8"));
	resp->AddContentLength(sbOut.GetLength());
	resp->Write(sbOut.ToByteArray());
	return true;
}

Bool __stdcall SSWR::SDNSProxy::SDNSProxyWebHandler::ReqV4Req(NN<SSWR::SDNSProxy::SDNSProxyWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	Text::StringBuilderUTF8 sbOut;
	AppendHeader(sbOut);
	AppendMenu(sbOut);

	sbOut.AppendC(UTF8STRC("<h2>Req V4</h2>\r\n"));
	sbOut.AppendC(UTF8STRC("<table border=\"0\"><tr><td>\r\n"));

	Data::ArrayListNN<Text::String> nameList;
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	NN<Text::String> name;
	UOSInt i;
	UOSInt j;
	me->proxy->GetReqv4List(nameList);

	i = 0;
	j = nameList.GetCount();
	while (i < j)
	{
		name = nameList.GetItemNoCheck(i);
		sbOut.AllocLeng(33 + name->leng * 2);
		if (i > 0)
		{
			sbOut.AppendC(UTF8STRC("<br/>\r\n"));
		}
		sbOut.AppendC(UTF8STRC("<a href=\"/reqv4?qry="));
		sbOut.Append(name);
		sbOut.AppendC(UTF8STRC("\">"));
		sbOut.Append(name);
		sbOut.AppendC(UTF8STRC("</a>"));

		i++;
	}

	sbOut.AppendC(UTF8STRC("</td><td valign=\"top\">\r\n"));

	if (req->GetQueryValueStr(CSTR("qry"), sbuff, 128).SetTo(sptr))
	{
		Data::ArrayListNN<Net::DNSClient::RequestAnswer> ansList;
		NN<Net::DNSClient::RequestAnswer> ans;
		Data::DateTime reqTime;
		UInt32 ttl;
		if (me->proxy->GetRequestInfov4(CSTRP(sbuff, sptr), ansList, reqTime, ttl))
		{
			sbOut.AppendC(UTF8STRC("<table border=\"1\"><tr><td>Request Name</td><td>"));
			sbOut.AppendP(sbuff, sptr);
			sbOut.AppendC(UTF8STRC("</td></tr>\r\n"));
			sbOut.AppendC(UTF8STRC("<tr><td>Request Time</td><td>"));
			reqTime.ToLocalTime();
			sptr = reqTime.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff zzzz");
			sbOut.AppendP(sbuff, sptr);
			sbOut.AppendC(UTF8STRC("</td></tr>\r\n"));
			sbOut.AppendC(UTF8STRC("<tr><td>TTL</td><td>"));
			sbOut.AppendU32(ttl);
			sbOut.AppendC(UTF8STRC("</td></tr>\r\n"));
			sbOut.AppendC(UTF8STRC("<tr><td>Results</td><td>"));

			sbOut.AppendC(UTF8STRC("<table border=\"1\"><tr><td>Name</td><td>Type</td><td>Class</td><td>TTL</td><td>RD</td><tr>\r\n"));
			i = 0;
			j = ansList.GetCount();
			while (i < j)
			{
				ans = ansList.GetItemNoCheck(i);
				sbOut.AppendC(UTF8STRC("<tr><td>"));
				sbOut.Append(ans->name);
				sbOut.AppendC(UTF8STRC("</td><td>"));
				sbOut.AppendI32(ans->recType);
				sbOut.AppendC(UTF8STRC("</td><td>"));
				sbOut.AppendI32(ans->recClass);
				sbOut.AppendC(UTF8STRC("</td><td>"));
				sbOut.AppendU32(ans->ttl);
				sbOut.AppendC(UTF8STRC("</td><td>"));
				if (ans->rd)
				{
					sbOut.Append(ans->rd);
				}
				sbOut.AppendC(UTF8STRC("</td></tr>\r\n"));
				i++;
			}
			sbOut.AppendC(UTF8STRC("</table>\r\n"));

			Net::DNSClient::FreeAnswers(ansList);
			sbOut.AppendC(UTF8STRC("</td></tr></table>\r\n"));
		}
		else
		{
		}
	}

	sbOut.AppendC(UTF8STRC("</td></tr></table>\r\n"));
	AppendFooter(sbOut);

	resp->SetStatusCode(Net::WebStatus::SC_OK);
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("text/html; charset=UTF-8"));
	resp->AddContentLength(sbOut.GetLength());
	resp->Write(sbOut.ToByteArray());
	return true;
}

Bool __stdcall SSWR::SDNSProxy::SDNSProxyWebHandler::ReqV6Req(NN<SSWR::SDNSProxy::SDNSProxyWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	Text::StringBuilderUTF8 sbOut;
	AppendHeader(sbOut);
	AppendMenu(sbOut);

	sbOut.AppendC(UTF8STRC("<h2>Req V6</h2>\r\n"));
	sbOut.AppendC(UTF8STRC("<table border=\"0\"><tr><td>\r\n"));

	Data::ArrayListNN<Text::String> nameList;
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	NN<Text::String> name;
	UOSInt i;
	UOSInt j;
	me->proxy->GetReqv6List(nameList);

	i = 0;
	j = nameList.GetCount();
	while (i < j)
	{
		name = nameList.GetItemNoCheck(i);
		if (i > 0)
		{
			sbOut.AppendC(UTF8STRC("<br/>\r\n"));
		}
		sbOut.AppendC(UTF8STRC("<a href=\"/reqv6?qry="));
		sbOut.Append(name);
		sbOut.AppendC(UTF8STRC("\">"));
		sbOut.Append(name);
		sbOut.AppendC(UTF8STRC("</a>"));

		i++;
	}

	sbOut.AppendC(UTF8STRC("</td><td valign=\"top\">\r\n"));

	if (req->GetQueryValueStr(CSTR("qry"), sbuff, 128).SetTo(sptr))
	{
		Data::ArrayListNN<Net::DNSClient::RequestAnswer> ansList;
		NN<Net::DNSClient::RequestAnswer> ans;
		Data::DateTime reqTime;
		UInt32 ttl;
		if (me->proxy->GetRequestInfov6(CSTRP(sbuff, sptr), ansList, reqTime, ttl))
		{
			sbOut.AppendC(UTF8STRC("<table border=\"1\"><tr><td>Request Name</td><td>"));
			sbOut.AppendP(sbuff, sptr);
			sbOut.AppendC(UTF8STRC("</td></tr>\r\n"));
			sbOut.AppendC(UTF8STRC("<tr><td>Request Time</td><td>"));
			reqTime.ToLocalTime();
			sptr = reqTime.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff zzzz");
			sbOut.AppendP(sbuff, sptr);
			sbOut.AppendC(UTF8STRC("</td></tr>\r\n"));
			sbOut.AppendC(UTF8STRC("<tr><td>TTL</td><td>"));
			sbOut.AppendU32(ttl);
			sbOut.AppendC(UTF8STRC("</td></tr>\r\n"));
			sbOut.AppendC(UTF8STRC("<tr><td>Results</td><td>"));

			sbOut.AppendC(UTF8STRC("<table border=\"1\"><tr><td>Name</td><td>Type</td><td>Class</td><td>TTL</td><td>RD</td><tr>\r\n"));
			i = 0;
			j = ansList.GetCount();
			while (i < j)
			{
				ans = ansList.GetItemNoCheck(i);
				sbOut.AppendC(UTF8STRC("<tr><td>"));
				sbOut.Append(ans->name);
				sbOut.AppendC(UTF8STRC("</td><td>"));
				sbOut.AppendI32(ans->recType);
				sbOut.AppendC(UTF8STRC("</td><td>"));
				sbOut.AppendI32(ans->recClass);
				sbOut.AppendC(UTF8STRC("</td><td>"));
				sbOut.AppendU32(ans->ttl);
				sbOut.AppendC(UTF8STRC("</td><td>"));
				if (ans->rd)
				{
					sbOut.Append(ans->rd);
				}
				sbOut.AppendC(UTF8STRC("</td><tr>\r\n"));
				i++;
			}
			sbOut.AppendC(UTF8STRC("</table>\r\n"));

			Net::DNSClient::FreeAnswers(ansList);
			sbOut.AppendC(UTF8STRC("</table>\r\n"));
		}
		else
		{
		}
	}

	sbOut.AppendC(UTF8STRC("</td></tr></table>\r\n"));
	AppendFooter(sbOut);

	resp->SetStatusCode(Net::WebStatus::SC_OK);
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("text/html; charset=UTF-8"));
	resp->AddContentLength(sbOut.GetLength());
	resp->Write(sbOut.ToByteArray());
	return true;
}

Bool __stdcall SSWR::SDNSProxy::SDNSProxyWebHandler::ReqOthReq(NN<SSWR::SDNSProxy::SDNSProxyWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	Text::StringBuilderUTF8 sbOut;
	AppendHeader(sbOut);
	AppendMenu(sbOut);

	sbOut.AppendC(UTF8STRC("<h2>Req Oth</h2>\r\n"));
	sbOut.AppendC(UTF8STRC("<table border=\"0\"><tr><td>\r\n"));

	Data::ArrayListNN<Text::String> nameList;
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	NN<Text::String> name;
	UOSInt i;
	UOSInt j;
	me->proxy->GetReqOthList(nameList);

	i = 0;
	j = nameList.GetCount();
	while (i < j)
	{
		name = nameList.GetItemNoCheck(i);
		if (i > 0)
		{
			sbOut.AppendC(UTF8STRC("<br/>\r\n"));
		}
		sbOut.AppendC(UTF8STRC("<a href=\"/reqoth?qry="));
		sbOut.Append(name);
		sbOut.AppendC(UTF8STRC("\">"));
		sbOut.Append(name);
		sbOut.AppendC(UTF8STRC("</a>"));

		i++;
	}

	sbOut.AppendC(UTF8STRC("</td><td valign=\"top\">\r\n"));

	if (req->GetQueryValueStr(CSTR("qry"), sbuff, 128).SetTo(sptr))
	{
		Data::ArrayListNN<Net::DNSClient::RequestAnswer> ansList;
		NN<Net::DNSClient::RequestAnswer> ans;
		Data::DateTime reqTime;
		UInt32 ttl;
		if (me->proxy->GetRequestInfoOth(CSTRP(sbuff, sptr), ansList, reqTime, ttl))
		{
			sbOut.AppendC(UTF8STRC("<table border=\"1\"><tr><td>Request Name</td><td>"));
			sbOut.AppendP(sbuff, sptr);
			sbOut.AppendC(UTF8STRC("</td></tr>\r\n"));
			sbOut.AppendC(UTF8STRC("<tr><td>Request Time</td><td>"));
			reqTime.ToLocalTime();
			sptr = reqTime.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff zzzz");
			sbOut.AppendP(sbuff, sptr);
			sbOut.AppendC(UTF8STRC("</td></tr>\r\n"));
			sbOut.AppendC(UTF8STRC("<tr><td>TTL</td><td>"));
			sbOut.AppendU32(ttl);
			sbOut.AppendC(UTF8STRC("</td></tr>\r\n"));
			sbOut.AppendC(UTF8STRC("<tr><td>Results</td><td>"));

			sbOut.AppendC(UTF8STRC("<table border=\"1\"><tr><td>Name</td><td>Type</td><td>Class</td><td>TTL</td><td>RD</td><tr>\r\n"));
			i = 0;
			j = ansList.GetCount();
			while (i < j)
			{
				ans = ansList.GetItemNoCheck(i);
				sbOut.AppendC(UTF8STRC("<tr><td>"));
				sbOut.Append(ans->name);
				sbOut.AppendC(UTF8STRC("</td><td>"));
				sbOut.AppendI32(ans->recType);
				sbOut.AppendC(UTF8STRC("</td><td>"));
				sbOut.AppendI32(ans->recClass);
				sbOut.AppendC(UTF8STRC("</td><td>"));
				sbOut.AppendU32(ans->ttl);
				sbOut.AppendC(UTF8STRC("</td><td>"));
				if (ans->rd)
				{
					sbOut.Append(ans->rd);
				}
				sbOut.AppendC(UTF8STRC("</td><tr>\r\n"));
				i++;
			}
			sbOut.AppendC(UTF8STRC("</table>\r\n"));

			Net::DNSClient::FreeAnswers(ansList);
			sbOut.AppendC(UTF8STRC("</table>\r\n"));
		}
		else
		{
		}
	}

	sbOut.AppendC(UTF8STRC("</td></tr></table>\r\n"));
	AppendFooter(sbOut);

	resp->SetStatusCode(Net::WebStatus::SC_OK);
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("text/html; charset=UTF-8"));
	resp->AddContentLength(sbOut.GetLength());
	resp->Write(sbOut.ToByteArray());
	return true;
}

Bool __stdcall SSWR::SDNSProxy::SDNSProxyWebHandler::TargetReq(NN<SSWR::SDNSProxy::SDNSProxyWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	Text::StringBuilderUTF8 sbOut;
	AppendHeader(sbOut);
	AppendMenu(sbOut);

	sbOut.AppendC(UTF8STRC("<h2>Target</h2>\r\n"));
	sbOut.AppendC(UTF8STRC("<table border=\"0\"><tr><td>\r\n"));

	Data::ArrayListNN<Net::DNSProxy::TargetInfo> targetList;
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	NN<Net::DNSProxy::TargetInfo> target;
	UOSInt i;
	UOSInt j;
	UInt32 targetIP = 0;
	OSInt targetIndex = -1;
	req->GetQueryValueU32(CSTR("qry"), targetIP);
	me->proxy->GetTargetList(targetList);

	i = 0;
	j = targetList.GetCount();
	while (i < j)
	{
		target = targetList.GetItemNoCheck(i);
		if (i > 0)
		{
			sbOut.AppendC(UTF8STRC("<br/>\r\n"));
		}
		sbOut.AppendC(UTF8STRC("<a href=\"/target?qry="));
		sbOut.AppendU32(target->ip);
		sbOut.AppendC(UTF8STRC("\">"));
		sptr = Net::SocketUtil::GetIPv4Name(sbuff, target->ip);
		sbOut.AppendP(sbuff, sptr);
		sbOut.AppendC(UTF8STRC("</a>"));
		if (target->ip == targetIP)
		{
			targetIndex = (OSInt)i;
		}

		i++;
	}

	sbOut.AppendC(UTF8STRC("</td><td valign=\"top\">\r\n"));

	if (targetIndex >= 0)
	{
		sbOut.AppendC(UTF8STRC("<h3>"));
		sptr = Net::SocketUtil::GetIPv4Name(sbuff, targetIP);
		sbOut.AppendP(sbuff, sptr);
		sbOut.AppendC(UTF8STRC("</h3>\r\n"));

		target = targetList.GetItemNoCheck((UOSInt)targetIndex);
		Sync::MutexUsage mutUsage(target->mut);
		i = 0;
		j = target->addrList.GetCount();
		while (i < j)
		{
			if (i > 0)
			{
				sbOut.AppendC(UTF8STRC("<br/>\r\n"));
			}
			sbOut.Append(target->addrList.GetItem(i));
			i++;
		}
		mutUsage.EndUse();
	}

	sbOut.AppendC(UTF8STRC("</td></tr></table>\r\n"));
	AppendFooter(sbOut);

	resp->SetStatusCode(Net::WebStatus::SC_OK);
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("text/html; charset=UTF-8"));
	resp->AddContentLength(sbOut.GetLength());
	resp->Write(sbOut.ToByteArray());
	return true;
}

Bool __stdcall SSWR::SDNSProxy::SDNSProxyWebHandler::BlacklistReq(NN<SSWR::SDNSProxy::SDNSProxyWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST)
	{
		req->ParseHTTPForm();
		NN<Text::String> blackList;
		if (req->GetHTTPFormStr(CSTR("blacklist")).SetTo(blackList) && blackList->v[0] != 0)
		{
			me->proxy->AddBlackList(blackList);
		}
	}
	Text::StringBuilderUTF8 sbOut;
	AppendHeader(sbOut);
	AppendMenu(sbOut);

	sbOut.AppendC(UTF8STRC("<h2>Blacklist</h2>\r\n"));
	sbOut.AppendC(UTF8STRC("<form method=\"POST\" action=\"/blacklist\"><input type=\"text\" name=\"blacklist\" />"));
	sbOut.AppendC(UTF8STRC("<input type=\"submit\" value=\"Add\" /></form>\r\n"));

	Data::ArrayListNN<Text::String> blkList;
	me->proxy->GetBlackList(blkList);
	sbOut.AppendJoin(blkList.Iterator(), CSTR("<br/>\r\n"));
	AppendFooter(sbOut);

	resp->SetStatusCode(Net::WebStatus::SC_OK);
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("text/html; charset=UTF-8"));
	resp->AddContentLength(sbOut.GetLength());
	resp->Write(sbOut.ToByteArray());
	return true;
}

Bool __stdcall SSWR::SDNSProxy::SDNSProxyWebHandler::LogReq(NN<SSWR::SDNSProxy::SDNSProxyWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	Text::StringBuilderUTF8 sbOut;
	AppendHeader(sbOut);
	AppendMenu(sbOut);

	sbOut.AppendC(UTF8STRC("<h2>Log</h2>\r\n"));
	me->logBuff->GetLogs(sbOut, CSTR("<br/>\r\n"));
	AppendFooter(sbOut);

	resp->SetStatusCode(Net::WebStatus::SC_OK);
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("text/html; charset=UTF-8"));
	resp->AddContentLength(sbOut.GetLength());
	resp->Write(sbOut.ToByteArray());
	return true;
}

Bool __stdcall SSWR::SDNSProxy::SDNSProxyWebHandler::ClientReq(NN<SSWR::SDNSProxy::SDNSProxyWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	Text::StringBuilderUTF8 sbOut;
	AppendHeader(sbOut);
	AppendMenu(sbOut);

	sbOut.AppendC(UTF8STRC("<h2>Client</h2>\r\n"));
	sbOut.AppendC(UTF8STRC("<table border=\"0\"><tr><td valign=\"top\">\r\n"));

	Data::ArrayListNN<SSWR::SDNSProxy::SDNSProxyCore::ClientInfo> cliList;
	UOSInt i;
	UOSInt j;
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	UInt32 selCliId = 0;
	NN<SSWR::SDNSProxy::SDNSProxyCore::ClientInfo> cli;
	Optional<SSWR::SDNSProxy::SDNSProxyCore::ClientInfo> selCli = 0;
	me->core->GetClientList(cliList);

	req->GetQueryValueU32(CSTR("cliId"), selCliId);

	i = 0;
	j = cliList.GetCount();
	while (i < j)
	{
		cli = cliList.GetItemNoCheck(i);
		sptr = Net::SocketUtil::GetAddrName(sbuff, cli->addr).Or(sbuff);
		if (i > 0)
		{
			sbOut.AppendC(UTF8STRC("<br/>\r\n"));
		}
		sbOut.AppendC(UTF8STRC("<a href=\"/client?cliId="));
		sbOut.AppendU32(ReadUInt32(cli->addr.addr));
		sbOut.AppendC(UTF8STRC("\">"));
		sbOut.AppendP(sbuff, sptr);
		sbOut.AppendC(UTF8STRC("</a>"));

		if (ReadUInt32(cli->addr.addr) == selCliId)
		{
			selCli = cli;
		}
		i++;
	}

	sbOut.AppendC(UTF8STRC("</td><td valign=\"top\">\r\n"));

	if (selCli.SetTo(cli))
	{
		sptr = Net::SocketUtil::GetAddrName(sbuff, cli->addr).Or(sbuff);
		sbOut.AppendC(UTF8STRC("<h3>"));
		sbOut.AppendP(sbuff, sptr);
		sbOut.AppendC(UTF8STRC("</h3>"));
		sbOut.AppendC(UTF8STRC("<table border=\"1\"><tr><td>Time</td><td>Count</td></tr>"));
		NN<SSWR::SDNSProxy::SDNSProxyCore::HourInfo> hInfo;
		Sync::MutexUsage mutUsage(cli->mut);
		i = cli->hourInfos.GetCount();
		while (i-- > 0)
		{
			hInfo = cli->hourInfos.GetItemNoCheck(i);

			sbOut.AppendC(UTF8STRC("<tr><td>"));
			sbOut.AppendI32(hInfo->year);
			sbOut.AppendUTF8Char('-');
			sbOut.AppendI32(hInfo->month);
			sbOut.AppendUTF8Char('-');
			sbOut.AppendI32(hInfo->day);
			sbOut.AppendUTF8Char(' ');
			sbOut.AppendI32(hInfo->hour);
			sbOut.AppendC(UTF8STRC(":00"));
			sbOut.AppendC(UTF8STRC("</td><td>"));
			sbOut.AppendI64(hInfo->reqCount);
			sbOut.AppendC(UTF8STRC("</td></tr>"));
		}
		mutUsage.EndUse();

		sbOut.AppendC(UTF8STRC("</table>"));
	}

	sbOut.AppendC(UTF8STRC("</td></tr></table>\r\n"));
	AppendFooter(sbOut);

	resp->SetStatusCode(Net::WebStatus::SC_OK);
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("text/html; charset=UTF-8"));
	resp->AddContentLength(sbOut.GetLength());
	resp->Write(sbOut.ToByteArray());
	return true;
}

Bool __stdcall SSWR::SDNSProxy::SDNSProxyWebHandler::ReqPerMinReq(NN<SSWR::SDNSProxy::SDNSProxyWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr = Text::StrUOSInt(sbuff, me->core->GetRequestPerMin());
	resp->SetStatusCode(Net::WebStatus::SC_OK);
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("text/plain"));
	resp->AddContentLength((UOSInt)(sptr - sbuff));
	resp->Write(Data::ByteArrayR(sbuff, (UOSInt)(sptr - sbuff)));
	return true;
}

void SSWR::SDNSProxy::SDNSProxyWebHandler::AppendHeader(NN<Text::StringBuilderUTF8> sbOut)
{
	sbOut->AppendC(UTF8STRC("<html><head><title>SDNSProxy</title></head><body>\r\n"));
}

void SSWR::SDNSProxy::SDNSProxyWebHandler::AppendMenu(NN<Text::StringBuilderUTF8> sbOut)
{
	sbOut->AppendC(UTF8STRC("<table border=\"1\"><tr>"));
	sbOut->AppendC(UTF8STRC("<td><a href=\"/\">Status</a></td>"));
	sbOut->AppendC(UTF8STRC("<td><a href=\"/reqv4\">Req v4</a></td>"));
	sbOut->AppendC(UTF8STRC("<td><a href=\"/reqv6\">Req v6</a></td>"));
	sbOut->AppendC(UTF8STRC("<td><a href=\"/reqoth\">Req Oth</a></td>"));
	sbOut->AppendC(UTF8STRC("<td><a href=\"/target\">Target</a></td>"));
//	sbOut->AppendC(UTF8STRC("<td><a href=\"/search\">Search</a></td>"));
	sbOut->AppendC(UTF8STRC("<td><a href=\"/blacklist\">Blacklist</a></td>"));
	sbOut->AppendC(UTF8STRC("<td><a href=\"/log\">Log</a></td>"));
	sbOut->AppendC(UTF8STRC("<td><a href=\"/client\">Client</a></td>"));
	sbOut->AppendC(UTF8STRC("</table>\r\n"));
}

void SSWR::SDNSProxy::SDNSProxyWebHandler::AppendFooter(NN<Text::StringBuilderUTF8> sbOut)
{
	sbOut->AppendC(UTF8STRC("</body></html>"));
}

Bool SSWR::SDNSProxy::SDNSProxyWebHandler::ProcessRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq)
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
	resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
	return true;
}

SSWR::SDNSProxy::SDNSProxyWebHandler::SDNSProxyWebHandler(NN<Net::DNSProxy> proxy, NN<IO::LogTool> log, SDNSProxyCore *core)
{
	this->proxy = proxy;
	this->core = core;
	this->log = log;
	NEW_CLASSNN(this->logBuff, IO::CyclicLogBuffer(LOGSIZE));

	this->reqMap.PutC(CSTR("/"), StatusReq);
	this->reqMap.PutC(CSTR("/reqv4"), ReqV4Req);
	this->reqMap.PutC(CSTR("/reqv6"), ReqV6Req);
	this->reqMap.PutC(CSTR("/reqoth"), ReqOthReq);
	this->reqMap.PutC(CSTR("/target"), TargetReq);
	this->reqMap.PutC(CSTR("/blacklist"), BlacklistReq);
	this->reqMap.PutC(CSTR("/log"), LogReq);
	this->reqMap.PutC(CSTR("/client"), ClientReq);
	this->reqMap.PutC(CSTR("/reqpm"), ReqPerMinReq);

	this->log->AddLogHandler(this->logBuff, IO::LogHandler::LogLevel::Raw);
}

SSWR::SDNSProxy::SDNSProxyWebHandler::~SDNSProxyWebHandler()
{
	this->log->RemoveLogHandler(this->logBuff);
	this->logBuff.Delete();
}

void SSWR::SDNSProxy::SDNSProxyWebHandler::Release()
{
}
