#include "Stdafx.h"
#include "Net/OSSocketFactory.h"
#include "SSWR/SDNSProxy/SDNSProxyCore.h"
#include "Sync/MutexUsage.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::SDNSProxy::SDNSProxyCore::OnDNSRequest(void *userObj, const UTF8Char *reqName, Int32 reqType, Int32 reqClass, const Net::SocketUtil::AddressInfo *reqAddr, UInt16 reqPort, Int32 reqId, Double timeUsed)
{
	SSWR::SDNSProxy::SDNSProxyCore *me = (SSWR::SDNSProxy::SDNSProxyCore*)userObj;
	UTF8Char sbuff[32];
	Text::StringBuilderUTF8 sb;
	sb.Append(reqName);
	sb.Append((const UTF8Char*)" from ");
	Net::SocketUtil::GetAddrName(sbuff, reqAddr, reqPort);
	sb.Append(sbuff);
	sb.Append((const UTF8Char*)", T=");
	sb.AppendI32(reqType);
	sb.Append((const UTF8Char*)", C=");
	sb.AppendI32(reqClass);
	sb.Append((const UTF8Char*)", t=");
	Text::SBAppendF64(&sb, timeUsed);
	me->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_RAW);

	Data::DateTime dt;
	ClientInfo *cli;
	dt.SetCurrTimeUTC();
	Int32 cliId = Net::SocketUtil::CalcCliId(reqAddr);
	Sync::MutexUsage mutUsage(me->cliInfoMut);
	cli = me->cliInfos->Get(cliId);
	if (cli == 0)
	{
		cli = MemAlloc(ClientInfo, 1);
		cli->cliId = cliId;
		cli->addr = *reqAddr;
		NEW_CLASS(cli->mut, Sync::Mutex());
		NEW_CLASS(cli->hourInfos, Data::ArrayList<HourInfo*>());
		me->cliInfos->Put(cliId, cli);
	}

	if (dt.GetHour() == me->lastHour && dt.GetMinute() == me->lastMinute)
	{
		me->currCnt++;
	}
	else
	{
		me->lastHour = dt.GetHour();
		me->lastMinute = dt.GetMinute();
		me->lastCnt = me->currCnt;
		me->currCnt = 0;
	}
	mutUsage.EndUse();

	HourInfo *hInfo;
	mutUsage.ReplaceMutex(cli->mut);
	hInfo = cli->hourInfos->GetItem(0);
	if (hInfo != 0 && hInfo->year == dt.GetYear() && hInfo->month == dt.GetMonth() && hInfo->day == dt.GetDay() && hInfo->hour == dt.GetHour())
	{
		hInfo->reqCount++;
	}
	else
	{
		if (cli->hourInfos->GetCount() >= 72)
		{
			hInfo = cli->hourInfos->RemoveAt(71);
		}
		else
		{
			hInfo = MemAlloc(HourInfo, 1);
		}
		hInfo->year = dt.GetYear();
		hInfo->month = dt.GetMonth();
		hInfo->day = dt.GetDay();
		hInfo->hour = dt.GetHour();
		hInfo->reqCount = 1;
		cli->hourInfos->Insert(0, hInfo);
	}
	mutUsage.EndUse();
}

SSWR::SDNSProxy::SDNSProxyCore::SDNSProxyCore(IO::ConfigFile *cfg, IO::Writer *console)
{
	this->console = console;
	NEW_CLASS(this->sockf, Net::OSSocketFactory(false));
	this->lastHour = 0;
	this->lastMinute = 0;
	this->lastCnt = 0;
	this->currCnt = 0;

	NEW_CLASS(this->cliInfoMut, Sync::Mutex());
	NEW_CLASS(this->cliInfos, Data::Integer32Map<ClientInfo*>());

	NEW_CLASS(this->log, IO::LogTool());

	NEW_CLASS(this->proxy, Net::DNSProxy(this->sockf, true));
	this->proxy->HandleDNSRequest(OnDNSRequest, this);

	if (this->proxy->IsError())
	{
		console->WriteLine((const UTF8Char*)"Error in listening to DNS port");
	}

	this->listener = 0;
	this->hdlr = 0;

	if (cfg)
	{
		const UTF8Char *csptr;
		OSInt i;
		OSInt j;
		Int32 ip;
		Int32 v;
		UTF8Char *sarr[2];
		csptr = cfg->GetValue((const UTF8Char*)"DNS");
		if (csptr)
		{
			Data::ArrayList<Int32> dnsList;
			Text::StringBuilderUTF8 sb;
			sb.Append(csptr);
			sarr[1] = sb.ToString();
			while (true)
			{
				i = Text::StrSplitTrim(sarr, 2, sarr[1], ',');
				ip = Net::SocketUtil::GetIPAddr(sarr[0]);
				if (ip)
				{
					dnsList.Add(ip);
				}
				if (i <= 1)
					break;
			}
			if (dnsList.GetCount() > 0)
			{
				this->proxy->SetServerIP(dnsList.GetItem(0));
				i = 1;
				j = dnsList.GetCount();
				while (i < j)
				{
					this->proxy->AddDNSIP(dnsList.GetItem(i));
					i++;
				}
			}
		}

		csptr = cfg->GetValue((const UTF8Char*)"LogPath");
		if (csptr)
		{
			this->log->AddFileLog(csptr, IO::ILogHandler::LOG_TYPE_PER_DAY, IO::ILogHandler::LOG_GROUP_TYPE_PER_MONTH, IO::ILogHandler::LOG_LEVEL_RAW, "yyyy-MM-dd HH:mm:ss.fff", false);
		}

		csptr = cfg->GetValue((const UTF8Char*)"DisableV6");
		if (csptr && Text::StrToInt32(csptr, &v))
		{
			this->proxy->SetDisableV6(v != 0);
		}

		csptr = cfg->GetValue((const UTF8Char*)"Blacklist");
		if (csptr && csptr[0] != 0)
		{
			Text::StringBuilderUTF8 sb;
			sb.Append(csptr);
			sarr[1] = sb.ToString();
			while (true)
			{
				i = Text::StrSplitTrim(sarr, 2, sarr[1], ',');
				if (sarr[0][0])
				{
					this->proxy->AddBlackList(sarr[0]);
				}
				if (i <= 1)
					break;
			}
		}

		Int32 managePort;
		csptr = cfg->GetValue((const UTF8Char*)"ManagePort");
		if (csptr && csptr[0] != 0 && Text::StrToInt32(csptr, &managePort))
		{
			NEW_CLASS(this->hdlr, SSWR::SDNSProxy::SDNSProxyWebHandler(this->proxy, this->log, this));
			NEW_CLASS(this->listener, Net::WebServer::WebListener(this->sockf, this->hdlr, managePort, 60, 4, (const UTF8Char*)"SDNSProxy/1.0", false, true));
			if (this->listener->IsError())
			{
				console->WriteLine((const UTF8Char*)"Error in listening to ManagePort");
				DEL_CLASS(this->listener);
				this->listener = 0;
			}
		}
		else
		{
			console->WriteLine((const UTF8Char*)"Config ManagePort not found");
		}
	}
	else
	{
		console->WriteLine((const UTF8Char*)"Config file not found");
	}
}

SSWR::SDNSProxy::SDNSProxyCore::~SDNSProxyCore()
{
	OSInt i;
	OSInt j;
	Data::ArrayList<ClientInfo*> *cliInfoList;
	ClientInfo *cli;
	SDEL_CLASS(this->listener);
	SDEL_CLASS(this->hdlr);

	DEL_CLASS(this->proxy);
	DEL_CLASS(this->cliInfoMut);
	cliInfoList = this->cliInfos->GetValues();
	i = cliInfoList->GetCount();
	while (i-- > 0)
	{
		cli = cliInfoList->GetItem(i);
		j = cli->hourInfos->GetCount();
		while (j-- > 0)
		{
			MemFree(cli->hourInfos->GetItem(j));
		}
		DEL_CLASS(cli->hourInfos);
		DEL_CLASS(cli->mut);
		MemFree(cli);
	}
	DEL_CLASS(this->cliInfos);
	DEL_CLASS(this->log);

	DEL_CLASS(this->sockf);
}

Bool SSWR::SDNSProxy::SDNSProxyCore::IsError()
{
	return this->listener == 0 || this->proxy == 0 || this->proxy->IsError();
}

void SSWR::SDNSProxy::SDNSProxyCore::Run(Core::IProgControl *progCtrl)
{
	this->console->WriteLine((const UTF8Char*)"SDNSProxy running");
	progCtrl->WaitForExit(progCtrl);
	this->console->WriteLine((const UTF8Char*)"SDNSProxy exiting");
}

OSInt SSWR::SDNSProxy::SDNSProxyCore::GetClientList(Data::ArrayList<SSWR::SDNSProxy::SDNSProxyCore::ClientInfo *> *cliList)
{
	OSInt initSize = cliList->GetCount();
	Sync::MutexUsage mutUsage(this->cliInfoMut);
	cliList->AddRange(this->cliInfos->GetValues());
	mutUsage.EndUse();
	return cliList->GetCount() - initSize;
}

OSInt SSWR::SDNSProxy::SDNSProxyCore::GetRequestPerMin()
{
	return this->lastCnt;
}
