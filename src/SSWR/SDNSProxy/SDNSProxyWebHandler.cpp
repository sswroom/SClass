#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "SSWR/SDNSProxy/SDNSProxyCore.h"
#include "SSWR/SDNSProxy/SDNSProxyWebHandler.h"
#include "Sync/MutexUsage.h"
#include "Text/StringBuilderUTF8.h"

#define LOGSIZE 300

Bool __stdcall SSWR::SDNSProxy::SDNSProxyWebHandler::StatusReq(SSWR::SDNSProxy::SDNSProxyWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	Text::StringBuilderUTF8 sbOut;
	UTF8Char sbuff[128];
	AppendHeader(&sbOut);
	AppendMenu(&sbOut);

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
	Net::SocketUtil::GetIPv4Name(sbuff, me->proxy->GetServerIP());
	sbOut.Append(sbuff);
	sbOut.AppendC(UTF8STRC("</td></tr>\r\n"));
	sbOut.AppendC(UTF8STRC("<tr><td>DNS List</td><td>"));
	Data::ArrayList<UInt32> dnsList;
	UOSInt i;
	UOSInt j;
	me->proxy->GetDNSList(&dnsList);
	i = 0;
	j = dnsList.GetCount();
	while (i < j)
	{
		if (i > 0)
		{
			sbOut.AppendC(UTF8STRC("<br/>\r\n"));
		}
		Net::SocketUtil::GetIPv4Name(sbuff, dnsList.GetItem(i));
		sbOut.Append(sbuff);
		i++;
	}
	sbOut.AppendC(UTF8STRC("</td></tr>\r\n"));
	sbOut.AppendC(UTF8STRC("</table>"));
	AppendFooter(&sbOut);

	resp->SetStatusCode(Net::WebStatus::SC_OK);
	resp->AddDefHeaders(req);
	resp->AddContentType(UTF8STRC("text/html; charset=UTF-8"));
	resp->AddContentLength(sbOut.GetLength());
	resp->Write(sbOut.ToString(), sbOut.GetLength());
	return true;
}

Bool __stdcall SSWR::SDNSProxy::SDNSProxyWebHandler::ReqV4Req(SSWR::SDNSProxy::SDNSProxyWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	Text::StringBuilderUTF8 sbOut;
	AppendHeader(&sbOut);
	AppendMenu(&sbOut);

	sbOut.AppendC(UTF8STRC("<h2>Req V4</h2>\r\n"));
	sbOut.AppendC(UTF8STRC("<table border=\"0\"><tr><td>\r\n"));

	Data::ArrayList<const UTF8Char *> nameList;
	UTF8Char sbuff[128];
	const UTF8Char *name;
	UOSInt nameLen;
	UOSInt i;
	UOSInt j;
	me->proxy->GetReqv4List(&nameList);

	i = 0;
	j = nameList.GetCount();
	while (i < j)
	{
		name = nameList.GetItem(i);
		nameLen = Text::StrCharCnt(name);
		sbOut.AllocLeng(33 + nameLen * 2);
		if (i > 0)
		{
			sbOut.AppendC((const UTF8Char*)"<br/>\r\n", 7);
		}
		sbOut.AppendC((const UTF8Char*)"<a href=\"/reqv4?qry=", 20);
		sbOut.AppendC(name, nameLen);
		sbOut.AppendC((const UTF8Char*)"\">", 2);
		sbOut.AppendC(name, nameLen);
		sbOut.AppendC((const UTF8Char*)"</a>", 4);

		i++;
	}

	sbOut.AppendC(UTF8STRC("</td><td valign=\"top\">\r\n"));

	if (req->GetQueryValueStr(UTF8STRC("qry"), sbuff, 128))
	{
		Data::ArrayList<Net::DNSClient::RequestAnswer *> ansList;
		Net::DNSClient::RequestAnswer *ans;
		Data::DateTime reqTime;
		UInt32 ttl;
		if (me->proxy->GetRequestInfov4(sbuff, &ansList, &reqTime, &ttl))
		{
			sbOut.AppendC(UTF8STRC("<table border=\"1\"><tr><td>Request Name</td><td>"));
			sbOut.Append(sbuff);
			sbOut.AppendC(UTF8STRC("</td></tr>\r\n"));
			sbOut.AppendC(UTF8STRC("<tr><td>Request Time</td><td>"));
			reqTime.ToLocalTime();
			reqTime.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff zzzz");
			sbOut.Append(sbuff);
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
				ans = ansList.GetItem(i);
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

			Net::DNSClient::FreeAnswers(&ansList);
			sbOut.AppendC(UTF8STRC("</td></tr></table>\r\n"));
		}
		else
		{
		}
	}

	sbOut.AppendC(UTF8STRC("</td></tr></table>\r\n"));
	AppendFooter(&sbOut);

	resp->SetStatusCode(Net::WebStatus::SC_OK);
	resp->AddDefHeaders(req);
	resp->AddContentType(UTF8STRC("text/html; charset=UTF-8"));
	resp->AddContentLength(sbOut.GetLength());
	resp->Write(sbOut.ToString(), sbOut.GetLength());
	return true;
}

Bool __stdcall SSWR::SDNSProxy::SDNSProxyWebHandler::ReqV6Req(SSWR::SDNSProxy::SDNSProxyWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	Text::StringBuilderUTF8 sbOut;
	AppendHeader(&sbOut);
	AppendMenu(&sbOut);

	sbOut.AppendC(UTF8STRC("<h2>Req V6</h2>\r\n"));
	sbOut.AppendC(UTF8STRC("<table border=\"0\"><tr><td>\r\n"));

	Data::ArrayList<const UTF8Char *> nameList;
	UTF8Char sbuff[128];
	const UTF8Char *name;
	UOSInt i;
	UOSInt j;
	me->proxy->GetReqv6List(&nameList);

	i = 0;
	j = nameList.GetCount();
	while (i < j)
	{
		name = nameList.GetItem(i);
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

	if (req->GetQueryValueStr(UTF8STRC("qry"), sbuff, 128))
	{
		Data::ArrayList<Net::DNSClient::RequestAnswer *> ansList;
		Net::DNSClient::RequestAnswer *ans;
		Data::DateTime reqTime;
		UInt32 ttl;
		if (me->proxy->GetRequestInfov6(sbuff, &ansList, &reqTime, &ttl))
		{
			sbOut.AppendC(UTF8STRC("<table border=\"1\"><tr><td>Request Name</td><td>"));
			sbOut.Append(sbuff);
			sbOut.AppendC(UTF8STRC("</td></tr>\r\n"));
			sbOut.AppendC(UTF8STRC("<tr><td>Request Time</td><td>"));
			reqTime.ToLocalTime();
			reqTime.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff zzzz");
			sbOut.Append(sbuff);
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
				ans = ansList.GetItem(i);
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

			Net::DNSClient::FreeAnswers(&ansList);
			sbOut.AppendC(UTF8STRC("</table>\r\n"));
		}
		else
		{
		}
	}

	sbOut.AppendC(UTF8STRC("</td></tr></table>\r\n"));
	AppendFooter(&sbOut);

	resp->SetStatusCode(Net::WebStatus::SC_OK);
	resp->AddDefHeaders(req);
	resp->AddContentType(UTF8STRC("text/html; charset=UTF-8"));
	resp->AddContentLength(sbOut.GetLength());
	resp->Write(sbOut.ToString(), sbOut.GetLength());
	return true;
}

Bool __stdcall SSWR::SDNSProxy::SDNSProxyWebHandler::ReqOthReq(SSWR::SDNSProxy::SDNSProxyWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	Text::StringBuilderUTF8 sbOut;
	AppendHeader(&sbOut);
	AppendMenu(&sbOut);

	sbOut.AppendC(UTF8STRC("<h2>Req Oth</h2>\r\n"));
	sbOut.AppendC(UTF8STRC("<table border=\"0\"><tr><td>\r\n"));

	Data::ArrayList<const UTF8Char *> nameList;
	UTF8Char sbuff[128];
	const UTF8Char *name;
	UOSInt i;
	UOSInt j;
	me->proxy->GetReqOthList(&nameList);

	i = 0;
	j = nameList.GetCount();
	while (i < j)
	{
		name = nameList.GetItem(i);
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

	if (req->GetQueryValueStr(UTF8STRC("qry"), sbuff, 128))
	{
		Data::ArrayList<Net::DNSClient::RequestAnswer *> ansList;
		Net::DNSClient::RequestAnswer *ans;
		Data::DateTime reqTime;
		UInt32 ttl;
		if (me->proxy->GetRequestInfoOth(sbuff, &ansList, &reqTime, &ttl))
		{
			sbOut.AppendC(UTF8STRC("<table border=\"1\"><tr><td>Request Name</td><td>"));
			sbOut.Append(sbuff);
			sbOut.AppendC(UTF8STRC("</td></tr>\r\n"));
			sbOut.AppendC(UTF8STRC("<tr><td>Request Time</td><td>"));
			reqTime.ToLocalTime();
			reqTime.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff zzzz");
			sbOut.Append(sbuff);
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
				ans = ansList.GetItem(i);
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

			Net::DNSClient::FreeAnswers(&ansList);
			sbOut.AppendC(UTF8STRC("</table>\r\n"));
		}
		else
		{
		}
	}

	sbOut.AppendC(UTF8STRC("</td></tr></table>\r\n"));
	AppendFooter(&sbOut);

	resp->SetStatusCode(Net::WebStatus::SC_OK);
	resp->AddDefHeaders(req);
	resp->AddContentType(UTF8STRC("text/html; charset=UTF-8"));
	resp->AddContentLength(sbOut.GetLength());
	resp->Write(sbOut.ToString(), sbOut.GetLength());
	return true;
}

Bool __stdcall SSWR::SDNSProxy::SDNSProxyWebHandler::TargetReq(SSWR::SDNSProxy::SDNSProxyWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	Text::StringBuilderUTF8 sbOut;
	AppendHeader(&sbOut);
	AppendMenu(&sbOut);

	sbOut.AppendC(UTF8STRC("<h2>Target</h2>\r\n"));
	sbOut.AppendC(UTF8STRC("<table border=\"0\"><tr><td>\r\n"));

	Data::ArrayList<Net::DNSProxy::TargetInfo *> targetList;
	UTF8Char sbuff[128];
	Net::DNSProxy::TargetInfo *target;
	UOSInt i;
	UOSInt j;
	UInt32 targetIP = 0;
	OSInt targetIndex = -1;
	req->GetQueryValueU32(UTF8STRC("qry"), &targetIP);
	me->proxy->GetTargetList(&targetList);

	i = 0;
	j = targetList.GetCount();
	while (i < j)
	{
		target = targetList.GetItem(i);
		if (i > 0)
		{
			sbOut.AppendC(UTF8STRC("<br/>\r\n"));
		}
		sbOut.AppendC(UTF8STRC("<a href=\"/target?qry="));
		sbOut.AppendU32(target->ip);
		sbOut.AppendC(UTF8STRC("\">"));
		Net::SocketUtil::GetIPv4Name(sbuff, target->ip);
		sbOut.Append(sbuff);
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
		Net::SocketUtil::GetIPv4Name(sbuff, targetIP);
		sbOut.Append(sbuff);
		sbOut.AppendC(UTF8STRC("</h3>\r\n"));

		target = targetList.GetItem((UOSInt)targetIndex);
		Sync::MutexUsage mutUsage(target->mut);
		i = 0;
		j = target->addrList->GetCount();
		while (i < j)
		{
			if (i > 0)
			{
				sbOut.AppendC((const UTF8Char*)"<br/>\r\n", 7);
			}
			sbOut.Append(target->addrList->GetItem(i));
			i++;
		}
		mutUsage.EndUse();
	}

	sbOut.AppendC(UTF8STRC("</td></tr></table>\r\n"));
	AppendFooter(&sbOut);

	resp->SetStatusCode(Net::WebStatus::SC_OK);
	resp->AddDefHeaders(req);
	resp->AddContentType(UTF8STRC("text/html; charset=UTF-8"));
	resp->AddContentLength(sbOut.GetLength());
	resp->Write(sbOut.ToString(), sbOut.GetLength());
	return true;
}

Bool __stdcall SSWR::SDNSProxy::SDNSProxyWebHandler::BlacklistReq(SSWR::SDNSProxy::SDNSProxyWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST)
	{
		req->ParseHTTPForm();
		Text::String *blackList = req->GetHTTPFormStr(UTF8STRC("blacklist"));
		if (blackList && blackList->v[0] != 0)
		{
			me->proxy->AddBlackList(blackList);
		}
	}
	Text::StringBuilderUTF8 sbOut;
	AppendHeader(&sbOut);
	AppendMenu(&sbOut);

	sbOut.AppendC(UTF8STRC("<h2>Blacklist</h2>\r\n"));
	sbOut.AppendC(UTF8STRC("<form method=\"POST\" action=\"/blacklist\"><input type=\"text\" name=\"blacklist\" />"));
	sbOut.AppendC(UTF8STRC("<input type=\"submit\" value=\"Add\" /></form>\r\n"));

	UOSInt i;
	UOSInt j;
	Data::ArrayList<const UTF8Char*> blkList;
	me->proxy->GetBlackList(&blkList);
	i = 0;
	j = blkList.GetCount();
	while (i < j)
	{
		sbOut.AppendC(UTF8STRC("<br/>\r\n"));
		sbOut.Append(blkList.GetItem(i));
		i++;
	}

	AppendFooter(&sbOut);

	resp->SetStatusCode(Net::WebStatus::SC_OK);
	resp->AddDefHeaders(req);
	resp->AddContentType(UTF8STRC("text/html; charset=UTF-8"));
	resp->AddContentLength(sbOut.GetLength());
	resp->Write(sbOut.ToString(), sbOut.GetLength());
	return true;
}

Bool __stdcall SSWR::SDNSProxy::SDNSProxyWebHandler::LogReq(SSWR::SDNSProxy::SDNSProxyWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	Text::StringBuilderUTF8 sbOut;
	AppendHeader(&sbOut);
	AppendMenu(&sbOut);

	sbOut.AppendC(UTF8STRC("<h2>Log</h2>\r\n"));
	me->logBuff->GetLogs(&sbOut, (const UTF8Char*)"<br/>\r\n");
	AppendFooter(&sbOut);

	resp->SetStatusCode(Net::WebStatus::SC_OK);
	resp->AddDefHeaders(req);
	resp->AddContentType(UTF8STRC("text/html; charset=UTF-8"));
	resp->AddContentLength(sbOut.GetLength());
	resp->Write(sbOut.ToString(), sbOut.GetLength());
	return true;
}

Bool __stdcall SSWR::SDNSProxy::SDNSProxyWebHandler::ClientReq(SSWR::SDNSProxy::SDNSProxyWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	Text::StringBuilderUTF8 sbOut;
	AppendHeader(&sbOut);
	AppendMenu(&sbOut);

	sbOut.AppendC(UTF8STRC("<h2>Client</h2>\r\n"));
	sbOut.AppendC(UTF8STRC("<table border=\"0\"><tr><td valign=\"top\">\r\n"));

	Data::ArrayList<SSWR::SDNSProxy::SDNSProxyCore::ClientInfo *> cliList;
	UOSInt i;
	UOSInt j;
	UTF8Char sbuff[128];
	UInt32 selCliId = 0;
	SSWR::SDNSProxy::SDNSProxyCore::ClientInfo *cli;
	SSWR::SDNSProxy::SDNSProxyCore::ClientInfo *selCli = 0;
	me->core->GetClientList(&cliList);

	req->GetQueryValueU32(UTF8STRC("cliId"), &selCliId);

	i = 0;
	j = cliList.GetCount();
	while (i < j)
	{
		cli = cliList.GetItem(i);
		Net::SocketUtil::GetAddrName(sbuff, &cli->addr);
		if (i > 0)
		{
			sbOut.AppendC(UTF8STRC("<br/>\r\n"));
		}
		sbOut.AppendC(UTF8STRC("<a href=\"/client?cliId="));
		sbOut.AppendU32(ReadUInt32(cli->addr.addr));
		sbOut.AppendC(UTF8STRC("\">"));
		sbOut.Append(sbuff);
		sbOut.AppendC(UTF8STRC("</a>"));

		if (ReadUInt32(cli->addr.addr) == selCliId)
		{
			selCli = cli;
		}
		i++;
	}

	sbOut.AppendC(UTF8STRC("</td><td valign=\"top\">\r\n"));

	if (selCli)
	{
		Net::SocketUtil::GetAddrName(sbuff, &selCli->addr);
		sbOut.AppendC(UTF8STRC("<h3>"));
		sbOut.Append(sbuff);
		sbOut.AppendC(UTF8STRC("</h3>"));
		sbOut.AppendC(UTF8STRC("<table border=\"1\"><tr><td>Time</td><td>Count</td></tr>"));
		SSWR::SDNSProxy::SDNSProxyCore::HourInfo *hInfo;
		Sync::MutexUsage mutUsage(selCli->mut);
		i = selCli->hourInfos->GetCount();
		while (i-- > 0)
		{
			hInfo = selCli->hourInfos->GetItem(i);

			sbOut.AppendC(UTF8STRC("<tr><td>"));
			sbOut.AppendI32(hInfo->year);
			sbOut.AppendChar('-', 1);
			sbOut.AppendI32(hInfo->month);
			sbOut.AppendChar('-', 1);
			sbOut.AppendI32(hInfo->day);
			sbOut.AppendChar(' ', 1);
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
	AppendFooter(&sbOut);

	resp->SetStatusCode(Net::WebStatus::SC_OK);
	resp->AddDefHeaders(req);
	resp->AddContentType(UTF8STRC("text/html; charset=UTF-8"));
	resp->AddContentLength(sbOut.GetLength());
	resp->Write(sbOut.ToString(), sbOut.GetLength());
	return true;
}

Bool __stdcall SSWR::SDNSProxy::SDNSProxyWebHandler::ReqPerMinReq(SSWR::SDNSProxy::SDNSProxyWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr = Text::StrUOSInt(sbuff, me->core->GetRequestPerMin());
	resp->SetStatusCode(Net::WebStatus::SC_OK);
	resp->AddDefHeaders(req);
	resp->AddContentType(UTF8STRC("text/plain"));
	resp->AddContentLength((UOSInt)(sptr - sbuff));
	resp->Write(sbuff, (UOSInt)(sptr - sbuff));
	return true;
}

void SSWR::SDNSProxy::SDNSProxyWebHandler::AppendHeader(Text::StringBuilderUTF8 *sbOut)
{
	sbOut->AppendC(UTF8STRC("<html><head><title>SDNSProxy</title></head><body>\r\n"));
}

void SSWR::SDNSProxy::SDNSProxyWebHandler::AppendMenu(Text::StringBuilderUTF8 *sbOut)
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

void SSWR::SDNSProxy::SDNSProxyWebHandler::AppendFooter(Text::StringBuilderUTF8 *sbOut)
{
	sbOut->AppendC(UTF8STRC("</body></html>"));
}

Bool SSWR::SDNSProxy::SDNSProxyWebHandler::ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, UOSInt subReqLen)
{
	if (this->DoRequest(req, resp, subReq, subReqLen))
	{
		return true;
	}
	RequestHandler reqHdlr = this->reqMap->GetC(subReq, subReqLen);
	if (reqHdlr)
	{
		return reqHdlr(this, req, resp);
	}
	resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
	return true;
}

SSWR::SDNSProxy::SDNSProxyWebHandler::SDNSProxyWebHandler(Net::DNSProxy *proxy, IO::LogTool *log, SDNSProxyCore *core)
{
	this->proxy = proxy;
	this->core = core;
	NEW_CLASS(this->reqMap, Data::FastStringMap<RequestHandler>());
	this->log = log;
	NEW_CLASS(this->logBuff, IO::CyclicLogBuffer(LOGSIZE));

	this->reqMap->PutC(UTF8STRC("/"), StatusReq);
	this->reqMap->PutC(UTF8STRC("/reqv4"), ReqV4Req);
	this->reqMap->PutC(UTF8STRC("/reqv6"), ReqV6Req);
	this->reqMap->PutC(UTF8STRC("/reqoth"), ReqOthReq);
	this->reqMap->PutC(UTF8STRC("/target"), TargetReq);
	this->reqMap->PutC(UTF8STRC("/blacklist"), BlacklistReq);
	this->reqMap->PutC(UTF8STRC("/log"), LogReq);
	this->reqMap->PutC(UTF8STRC("/client"), ClientReq);
	this->reqMap->PutC(UTF8STRC("/reqpm"), ReqPerMinReq);

	this->log->AddLogHandler(this->logBuff, IO::ILogHandler::LOG_LEVEL_RAW);
}

SSWR::SDNSProxy::SDNSProxyWebHandler::~SDNSProxyWebHandler()
{
	this->log->RemoveLogHandler(this->logBuff);
	DEL_CLASS(this->logBuff);
	DEL_CLASS(this->reqMap);
}

void SSWR::SDNSProxy::SDNSProxyWebHandler::Release()
{
}
