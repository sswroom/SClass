#include "Stdafx.h"
#include "Net/OSSocketFactory.h"
#include "SSWR/SDNSProxy/SDNSProxyCore.h"
#include "Sync/MutexUsage.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::SDNSProxy::SDNSProxyCore::OnDNSRequest(AnyType userObj, Text::CStringNN reqName, Int32 reqType, Int32 reqClass, NN<const Net::SocketUtil::AddressInfo> reqAddr, UInt16 reqPort, UInt32 reqId, Double timeUsed)
{
	NN<SSWR::SDNSProxy::SDNSProxyCore> me = userObj.GetNN<SSWR::SDNSProxy::SDNSProxyCore>();
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
	sb.AppendDouble(timeUsed);
	me->log.LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Raw);

	Data::DateTime dt;
	NN<ClientInfo> cli;
	dt.SetCurrTimeUTC();
	UInt32 cliId = Net::SocketUtil::CalcCliId(reqAddr);
	Sync::MutexUsage mutUsage(me->cliInfoMut);
	if (!me->cliInfos.Get(cliId).SetTo(cli))
	{
		NEW_CLASSNN(cli, ClientInfo());
		cli->cliId = cliId;
		cli->addr = reqAddr.Ptr()[0];
		me->cliInfos.Put(cliId, cli);
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

	NN<HourInfo> hInfo;
	mutUsage.ReplaceMutex(cli->mut);
	if (cli->hourInfos.GetItem(0).SetTo(hInfo) && hInfo->year == dt.GetYear() && hInfo->month == dt.GetMonth() && hInfo->day == dt.GetDay() && hInfo->hour == dt.GetHour())
	{
		hInfo->reqCount++;
	}
	else
	{
		if (cli->hourInfos.GetCount() >= 72 && cli->hourInfos.RemoveAt(71).SetTo(hInfo))
		{
		}
		else
		{
			hInfo = MemAllocNN(HourInfo);
		}
		hInfo->year = dt.GetYear();
		hInfo->month = dt.GetMonth();
		hInfo->day = dt.GetDay();
		hInfo->hour = dt.GetHour();
		hInfo->reqCount = 1;
		cli->hourInfos.Insert(0, hInfo);
	}
	mutUsage.EndUse();
}

SSWR::SDNSProxy::SDNSProxyCore::SDNSProxyCore(IO::ConfigFile *cfg, IO::Writer *console)
{
	this->console = console;
	NEW_CLASSNN(this->sockf, Net::OSSocketFactory(false));
	this->lastHour = 0;
	this->lastMinute = 0;
	this->lastCnt = 0;
	this->currCnt = 0;

	NEW_CLASSNN(this->proxy, Net::DNSProxy(this->sockf, true, this->log));
	this->proxy->HandleDNSRequest(OnDNSRequest, this);

	if (this->proxy->IsError())
	{
		console->WriteLine(CSTR("Error in listening to DNS port"));
	}

	this->listener = 0;
	this->hdlr = 0;

	if (cfg)
	{
		NN<Text::String> s;
		UOSInt i;
		UOSInt j;
		UInt32 ip;
		Int32 v;
		Text::PString sarr[2];
		if (cfg->GetValue(CSTR("DNS")).SetTo(s))
		{
			Data::ArrayList<UInt32> dnsList;
			Text::StringBuilderUTF8 sb;
			sb.Append(s);
			sarr[1] = sb;
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

		if (cfg->GetValue(CSTR("LogPath")).SetTo(s))
		{
			this->log.AddFileLog(Text::String::OrEmpty(s), IO::LogHandler::LogType::PerDay, IO::LogHandler::LogGroup::PerMonth, IO::LogHandler::LogLevel::Raw, "yyyy-MM-dd HH:mm:ss.fff", false);
		}

		if (cfg->GetValue(CSTR("DisableV6")).SetTo(s) && s->ToInt32(v))
		{
			this->proxy->SetDisableV6(v != 0);
		}

		if (cfg->GetValue(CSTR("Blacklist")).SetTo(s) && s->v[0] != 0)
		{
			Text::StringBuilderUTF8 sb;
			sb.Append(s);
			sarr[1] = sb;
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
		if (cfg->GetValue(CSTR("ManagePort")).SetTo(s) && s->v[0] != 0 && s->ToUInt16(managePort))
		{
			NN<SSWR::SDNSProxy::SDNSProxyWebHandler> hdlr;
			NEW_CLASSNN(hdlr, SSWR::SDNSProxy::SDNSProxyWebHandler(this->proxy, this->log, this));
			NEW_CLASS(this->listener, Net::WebServer::WebListener(this->sockf, 0, hdlr, managePort, 60, 1, 4, CSTR("SDNSProxy/1.0"), false, Net::WebServer::KeepAlive::Default, true));
			if (this->listener->IsError())
			{
				console->WriteLine(CSTR("Error in listening to ManagePort"));
				DEL_CLASS(this->listener);
				this->listener = 0;
				hdlr.Delete();
			}
			else
			{
				this->hdlr = hdlr.Ptr();
			}
		}
		else
		{
			console->WriteLine(CSTR("Config ManagePort not found"));
		}
	}
	else
	{
		console->WriteLine(CSTR("Config file not found"));
	}
}

SSWR::SDNSProxy::SDNSProxyCore::~SDNSProxyCore()
{
	UOSInt i;
	UOSInt j;
	NN<ClientInfo> cli;
	SDEL_CLASS(this->listener);
	SDEL_CLASS(this->hdlr);

	this->proxy.Delete();
	i = this->cliInfos.GetCount();
	while (i-- > 0)
	{
		cli = this->cliInfos.GetItemNoCheck(i);
		j = cli->hourInfos.GetCount();
		while (j-- > 0)
		{
			MemFreeNN(cli->hourInfos.GetItemNoCheck(j));
		}
		cli.Delete();
	}

	this->sockf.Delete();
}

Bool SSWR::SDNSProxy::SDNSProxyCore::IsError()
{
	return this->listener == 0 || this->proxy->IsError();
}

void SSWR::SDNSProxy::SDNSProxyCore::Run(NN<Core::IProgControl> progCtrl)
{
	this->console->WriteLine(CSTR("SDNSProxy running"));
	progCtrl->WaitForExit(progCtrl);
	this->console->WriteLine(CSTR("SDNSProxy exiting"));
}

UOSInt SSWR::SDNSProxy::SDNSProxyCore::GetClientList(NN<Data::ArrayListNN<SSWR::SDNSProxy::SDNSProxyCore::ClientInfo>> cliList)
{
	UOSInt initSize = cliList->GetCount();
	Sync::MutexUsage mutUsage(this->cliInfoMut);
	cliList->AddAll(this->cliInfos);
	mutUsage.EndUse();
	return cliList->GetCount() - initSize;
}

UOSInt SSWR::SDNSProxy::SDNSProxyCore::GetRequestPerMin()
{
	return this->lastCnt;
}
