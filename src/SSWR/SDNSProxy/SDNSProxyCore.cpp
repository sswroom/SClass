#include "Stdafx.h"
#include "Net/OSSocketFactory.h"
#include "SSWR/SDNSProxy/SDNSProxyCore.h"
#include "Sync/MutexUsage.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::SDNSProxy::SDNSProxyCore::OnDNSRequest(void *userObj, Text::CString reqName, Int32 reqType, Int32 reqClass, const Net::SocketUtil::AddressInfo *reqAddr, UInt16 reqPort, UInt32 reqId, Double timeUsed)
{
	SSWR::SDNSProxy::SDNSProxyCore *me = (SSWR::SDNSProxy::SDNSProxyCore*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	Text::StringBuilderUTF8 sb;
	sb.Append(reqName);
	sb.AppendC(UTF8STRC(" from "));
	sptr = Net::SocketUtil::GetAddrName(sbuff, reqAddr, reqPort);
	sb.AppendP(sbuff, sptr);
	sb.AppendC(UTF8STRC(", T="));
	sb.AppendI32(reqType);
	sb.AppendC(UTF8STRC(", C="));
	sb.AppendI32(reqClass);
	sb.AppendC(UTF8STRC(", t="));
	Text::SBAppendF64(&sb, timeUsed);
	me->log->LogMessage(sb.ToCString(), IO::ILogHandler::LOG_LEVEL_RAW);

	Data::DateTime dt;
	ClientInfo *cli;
	dt.SetCurrTimeUTC();
	UInt32 cliId = Net::SocketUtil::CalcCliId(reqAddr);
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
	NEW_CLASS(this->cliInfos, Data::UInt32Map<ClientInfo*>());

	NEW_CLASS(this->log, IO::LogTool());

	NEW_CLASS(this->proxy, Net::DNSProxy(this->sockf, true));
	this->proxy->HandleDNSRequest(OnDNSRequest, this);

	if (this->proxy->IsError())
	{
		console->WriteLineC(UTF8STRC("Error in listening to DNS port"));
	}

	this->listener = 0;
	this->hdlr = 0;

	if (cfg)
	{
		Text::String *s;
		UOSInt i;
		UOSInt j;
		UInt32 ip;
		Int32 v;
		Text::PString sarr[2];
		s = cfg->GetValue(CSTR("DNS"));
		if (s)
		{
			Data::ArrayList<UInt32> dnsList;
			Text::StringBuilderUTF8 sb;
			sb.Append(s);
			sarr[1].v = sb.ToString();
			sarr[1].leng = sb.GetLength();
			while (true)
			{
				i = Text::StrSplitTrimP(sarr, 2, sarr[1], ',');
				ip = Net::SocketUtil::GetIPAddr(sarr[0].ToCString());
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

		s = cfg->GetValue(CSTR("LogPath"));
		if (s)
		{
			this->log->AddFileLog(s, IO::ILogHandler::LOG_TYPE_PER_DAY, IO::ILogHandler::LOG_GROUP_TYPE_PER_MONTH, IO::ILogHandler::LOG_LEVEL_RAW, "yyyy-MM-dd HH:mm:ss.fff", false);
		}

		s = cfg->GetValue(CSTR("DisableV6"));
		if (s && s->ToInt32(&v))
		{
			this->proxy->SetDisableV6(v != 0);
		}

		s = cfg->GetValue(CSTR("Blacklist"));
		if (s && s->v[0] != 0)
		{
			Text::StringBuilderUTF8 sb;
			sb.Append(s);
			sarr[1].v = sb.ToString();
			sarr[1].leng = sb.GetLength();
			while (true)
			{
				i = Text::StrSplitTrimP(sarr, 2, sarr[1], ',');
				if (sarr[0].v[0])
				{
					this->proxy->AddBlackList(sarr[0].ToCString());
				}
				if (i <= 1)
					break;
			}
		}

		UInt16 managePort;
		s = cfg->GetValue(CSTR("ManagePort"));
		if (s && s->v[0] != 0 && s->ToUInt16(&managePort))
		{
			NEW_CLASS(this->hdlr, SSWR::SDNSProxy::SDNSProxyWebHandler(this->proxy, this->log, this));
			NEW_CLASS(this->listener, Net::WebServer::WebListener(this->sockf, 0, this->hdlr, managePort, 60, 4, CSTR("SDNSProxy/1.0"), false, true));
			if (this->listener->IsError())
			{
				console->WriteLineC(UTF8STRC("Error in listening to ManagePort"));
				DEL_CLASS(this->listener);
				this->listener = 0;
			}
		}
		else
		{
			console->WriteLineC(UTF8STRC("Config ManagePort not found"));
		}
	}
	else
	{
		console->WriteLineC(UTF8STRC("Config file not found"));
	}
}

SSWR::SDNSProxy::SDNSProxyCore::~SDNSProxyCore()
{
	UOSInt i;
	UOSInt j;
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
	this->console->WriteLineC(UTF8STRC("SDNSProxy running"));
	progCtrl->WaitForExit(progCtrl);
	this->console->WriteLineC(UTF8STRC("SDNSProxy exiting"));
}

UOSInt SSWR::SDNSProxy::SDNSProxyCore::GetClientList(Data::ArrayList<SSWR::SDNSProxy::SDNSProxyCore::ClientInfo *> *cliList)
{
	UOSInt initSize = cliList->GetCount();
	Sync::MutexUsage mutUsage(this->cliInfoMut);
	cliList->AddAll(this->cliInfos->GetValues());
	mutUsage.EndUse();
	return cliList->GetCount() - initSize;
}

UOSInt SSWR::SDNSProxy::SDNSProxyCore::GetRequestPerMin()
{
	return this->lastCnt;
}
