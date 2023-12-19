#include "Stdafx.h"
#include "Manage/HiResClock.h"
#include "SSWR/AVIRead/AVIRDNSProxyForm.h"
#include "Sync/MutexUsage.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRDNSProxyForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRDNSProxyForm *me = (SSWR::AVIRead::AVIRDNSProxyForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	UInt32 ip = me->proxy->GetServerIP();
	if (ip != me->currServer)
	{
		me->currServer = ip;
		sptr = Net::SocketUtil::GetIPv4Name(sbuff, ip);
		me->txtDNSServer->SetText(CSTRP(sbuff, sptr));
	}

	if (me->proxy->IsReqListv4Chg())
	{
		Data::ArrayList<Text::String *> nameList;
		Text::StringBuilderUTF8 sb;
		UOSInt i;
		UOSInt j;
		me->proxy->GetReqv4List(&nameList);

		me->lbV4Request->ClearItems();
		i = 0;
		j = nameList.GetCount();
		while (i < j)
		{
			sb.ClearStr();
			sb.Append(nameList.GetItem(i));
			me->lbV4Request->AddItem(sb.ToCString(), 0);
			i++;
		}
	}
	if (me->proxy->IsReqListv6Chg())
	{
		Data::ArrayList<Text::String *> nameList;
		Text::StringBuilderUTF8 sb;
		UOSInt i;
		UOSInt j;
		me->proxy->GetReqv6List(&nameList);

		me->lbV6Request->ClearItems();
		i = 0;
		j = nameList.GetCount();
		while (i < j)
		{
			sb.ClearStr();
			sb.Append(nameList.GetItem(i));
			me->lbV6Request->AddItem(sb.ToCString(), 0);
			i++;
		}
	}
	if (me->proxy->IsReqListOthChg())
	{
		Data::ArrayList<Text::String *> nameList;
		Text::StringBuilderUTF8 sb;
		UOSInt i;
		UOSInt j;
		me->proxy->GetReqOthList(&nameList);

		me->lbOthRequest->ClearItems();
		i = 0;
		j = nameList.GetCount();
		while (i < j)
		{
			sb.ClearStr();
			sb.Append(nameList.GetItem(i));
			me->lbOthRequest->AddItem(sb.ToCString(), 0);
			i++;
		}
	}
	if (me->proxy->IsTargetChg())
	{
		Data::ArrayList<Net::DNSProxy::TargetInfo*> targetList;
		UOSInt i;
		UOSInt j;
		Net::DNSProxy::TargetInfo *target;
		me->proxy->GetTargetList(&targetList);

		me->lbTarget->ClearItems();
		i = 0;
		j = targetList.GetCount();
		while (i < j)
		{
			target = targetList.GetItem(i);
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, target->ip);
			me->lbTarget->AddItem(CSTRP(sbuff, sptr), target);
			if (me->currTarget == target)
			{
				me->lbTarget->SetSelectedIndex(i);
			}
			i++;
		}
	}

	if (me->cliChg)
	{
		WChar wbuff[32];
		ClientInfo *cli;
		UOSInt i;
		UOSInt j;
		me->cliChg = false;
		me->lbClientIP->ClearItems();
		me->lvClient->ClearItems();

		Sync::MutexUsage mutUsage(me->cliInfoMut);
		i = 0;
		j = me->cliInfos.GetCount();
		while (i < j)
		{
			cli = me->cliInfos.GetItem(i);
			Net::SocketUtil::GetAddrName(wbuff, cli->addr);
			me->lbClientIP->AddItem(wbuff, cli);
			i++;
		}
		mutUsage.EndUse();
	}
}

void __stdcall SSWR::AVIRead::AVIRDNSProxyForm::OnV4ReqSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRDNSProxyForm *me = (SSWR::AVIRead::AVIRDNSProxyForm*)userObj;
	Net::DNSClient::FreeAnswers(me->v4ansList);
	UOSInt i;
	UOSInt j;

	me->lbV4Answer->ClearItems();
	NotNullPtr<Text::String> req;
	if (me->lbV4Request->GetSelectedItemTextNew().SetTo(req))
	{
		UTF8Char sbuff[32];
		UTF8Char *sptr;
		Net::DNSClient::RequestAnswer *ans;
		Data::DateTime reqTime;
		UInt32 ttl;
		if (me->proxy->GetRequestInfov4(req->ToCString(), me->v4ansList, reqTime, ttl))
		{
			UOSInt bestInd = (UOSInt)-1;
			i = 0;
			j = me->v4ansList.GetCount();
			while (i < j)
			{
				ans = me->v4ansList.GetItem(i);
				if (bestInd == (UOSInt)-1 && ans->recType == 1)
					bestInd = i;
				me->lbV4Answer->AddItem(ans->name, ans);
				i++;
			}
			if (bestInd == (UOSInt)-1)
				bestInd = 0;
			if (j > 0)
			{
				me->lbV4Answer->SetSelectedIndex(bestInd);
			}
			reqTime.ToLocalTime();
			sptr = reqTime.ToString(sbuff);
			me->txtV4RequestTime->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrUInt32(sbuff, ttl);
			me->txtV4RequestTTL->SetText(CSTRP(sbuff, sptr));
		}
		else
		{
		}
		req->Release();
	}
}

void __stdcall SSWR::AVIRead::AVIRDNSProxyForm::OnV4AnsSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRDNSProxyForm *me = (SSWR::AVIRead::AVIRDNSProxyForm*)userObj;
	Net::DNSClient::RequestAnswer *ans = (Net::DNSClient::RequestAnswer*)me->lbV4Answer->GetSelectedItem();
	if (ans)
	{
		UTF8Char sbuff[16];
		UTF8Char *sptr;
		Data::DateTime t;
		me->txtV4AnsName->SetText(ans->name->ToCString());
		sptr = Text::StrInt32(sbuff, ans->recType);
		me->txtV4AnsType->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrInt32(sbuff, ans->recClass);
		me->txtV4AnsClass->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrUInt32(sbuff, ans->ttl);
		me->txtV4AnsTTL->SetText(CSTRP(sbuff, sptr));
		if (ans->rd)
		{
			me->txtV4AnsRD->SetText(ans->rd->ToCString());
		}
		else
		{
			me->txtV4AnsRD->SetText(CSTR(""));
		}
	}
	else
	{
		me->txtV4AnsName->SetText(CSTR(""));
		me->txtV4AnsType->SetText(CSTR(""));
		me->txtV4AnsClass->SetText(CSTR(""));
		me->txtV4AnsTTL->SetText(CSTR(""));
		me->txtV4AnsRD->SetText(CSTR(""));
	}
}

void __stdcall SSWR::AVIRead::AVIRDNSProxyForm::OnV6ReqSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRDNSProxyForm *me = (SSWR::AVIRead::AVIRDNSProxyForm*)userObj;
	Net::DNSClient::FreeAnswers(me->v6ansList);
	UOSInt i;
	UOSInt j;

	me->lbV6Answer->ClearItems();
	NotNullPtr<Text::String> req;
	if (me->lbV6Request->GetSelectedItemTextNew().SetTo(req))
	{
		UTF8Char sbuff[32];
		UTF8Char *sptr;
		Net::DNSClient::RequestAnswer *ans;
		Data::DateTime reqTime;
		UInt32 ttl;
		if (me->proxy->GetRequestInfov6(req->ToCString(), me->v6ansList, reqTime, ttl))
		{
			UOSInt bestInd = (UOSInt)-1;
			i = 0;
			j = me->v6ansList.GetCount();
			while (i < j)
			{
				ans = me->v6ansList.GetItem(i);
				if (bestInd == (UOSInt)-1 && ans->recType == 1)
					bestInd = i;
				me->lbV6Answer->AddItem(ans->name, ans);
				i++;
			}
			if (bestInd == (UOSInt)-1)
				bestInd = 0;
			if (j > 0)
			{
				me->lbV6Answer->SetSelectedIndex(bestInd);
			}
			reqTime.ToLocalTime();
			sptr = reqTime.ToString(sbuff);
			me->txtV6RequestTime->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrUInt32(sbuff, ttl);
			me->txtV6RequestTTL->SetText(CSTRP(sbuff, sptr));
		}
		else
		{
		}
		req->Release();
	}
}

void __stdcall SSWR::AVIRead::AVIRDNSProxyForm::OnV6AnsSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRDNSProxyForm *me = (SSWR::AVIRead::AVIRDNSProxyForm*)userObj;
	Net::DNSClient::RequestAnswer *ans = (Net::DNSClient::RequestAnswer*)me->lbV6Answer->GetSelectedItem();
	if (ans)
	{
		UTF8Char sbuff[16];
		UTF8Char *sptr;
		Data::DateTime t;
		me->txtV6AnsName->SetText(ans->name->ToCString());
		sptr = Text::StrInt32(sbuff, ans->recType);
		me->txtV6AnsType->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrInt32(sbuff, ans->recClass);
		me->txtV6AnsClass->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrUInt32(sbuff, ans->ttl);
		me->txtV6AnsTTL->SetText(CSTRP(sbuff, sptr));
		if (ans->rd)
		{
			me->txtV6AnsRD->SetText(ans->rd->ToCString());
		}
		else
		{
			me->txtV6AnsRD->SetText(CSTR(""));
		}
	}
	else
	{
		me->txtV6AnsName->SetText(CSTR(""));
		me->txtV6AnsType->SetText(CSTR(""));
		me->txtV6AnsClass->SetText(CSTR(""));
		me->txtV6AnsTTL->SetText(CSTR(""));
		me->txtV6AnsRD->SetText(CSTR(""));
	}
}

void __stdcall SSWR::AVIRead::AVIRDNSProxyForm::OnOthReqSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRDNSProxyForm *me = (SSWR::AVIRead::AVIRDNSProxyForm*)userObj;
	Net::DNSClient::FreeAnswers(me->othansList);
	UOSInt i;
	UOSInt j;

	me->lbOthAnswer->ClearItems();
	NotNullPtr<Text::String> req;
	if (me->lbOthRequest->GetSelectedItemTextNew().SetTo(req))
	{
		UTF8Char sbuff[32];
		UTF8Char *sptr;
		Net::DNSClient::RequestAnswer *ans;
		Data::DateTime reqTime;
		UInt32 ttl;
		if (me->proxy->GetRequestInfoOth(req->ToCString(), me->othansList, reqTime, ttl))
		{
			UOSInt bestInd = (UOSInt)-1;
			i = 0;
			j = me->othansList.GetCount();
			while (i < j)
			{
				ans = me->othansList.GetItem(i);
				if (bestInd == (UOSInt)-1 && ans->recType == 1)
					bestInd = i;
				me->lbOthAnswer->AddItem(ans->name, ans);
				i++;
			}
			if (bestInd == (UOSInt)-1)
				bestInd = 0;
			if (j > 0)
			{
				me->lbOthAnswer->SetSelectedIndex(bestInd);
			}
			reqTime.ToLocalTime();
			sptr = reqTime.ToString(sbuff);
			me->txtOthRequestTime->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrUInt32(sbuff, ttl);
			me->txtOthRequestTTL->SetText(CSTRP(sbuff, sptr));
		}
		else
		{
		}
		req->Release();
	}
}

void __stdcall SSWR::AVIRead::AVIRDNSProxyForm::OnOthAnsSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRDNSProxyForm *me = (SSWR::AVIRead::AVIRDNSProxyForm*)userObj;
	Net::DNSClient::RequestAnswer *ans = (Net::DNSClient::RequestAnswer*)me->lbOthAnswer->GetSelectedItem();
	if (ans)
	{
		UTF8Char sbuff[16];
		UTF8Char *sptr;
		Data::DateTime t;
		me->txtOthAnsName->SetText(ans->name->ToCString());
		sptr = Text::StrInt32(sbuff, ans->recType);
		me->txtOthAnsType->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrInt32(sbuff, ans->recClass);
		me->txtOthAnsClass->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrUInt32(sbuff, ans->ttl);
		me->txtOthAnsTTL->SetText(CSTRP(sbuff, sptr));
		if (ans->rd)
		{
			me->txtOthAnsRD->SetText(ans->rd->ToCString());
		}
		else
		{
			me->txtOthAnsRD->SetText(CSTR(""));
		}
	}
	else
	{
		me->txtOthAnsName->SetText(CSTR(""));
		me->txtOthAnsType->SetText(CSTR(""));
		me->txtOthAnsClass->SetText(CSTR(""));
		me->txtOthAnsTTL->SetText(CSTR(""));
		me->txtOthAnsRD->SetText(CSTR(""));
	}
}

void __stdcall SSWR::AVIRead::AVIRDNSProxyForm::OnTargetSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRDNSProxyForm *me = (SSWR::AVIRead::AVIRDNSProxyForm*)userObj;
	Net::DNSProxy::TargetInfo *target = (Net::DNSProxy::TargetInfo*)me->lbTarget->GetSelectedItem();
	me->currTarget = target;
	if (target)
	{
		Net::WhoisRecord *rec = me->whois.RequestIP(target->ip);
		UTF8Char sbuff[256];
		UTF8Char *sptr;
		UOSInt i;
		UOSInt j;
		if (rec)
		{
			if ((sptr = rec->GetNetworkName(sbuff)) != 0)
			{
				me->txtTargetName->SetText(CSTRP(sbuff, sptr));
			}
			else
			{
				me->txtTargetName->SetText(CSTR("Unknown"));
			}
			if ((sptr = rec->GetCountryCode(sbuff)) != 0)
			{
				me->txtTargetCountry->SetText(CSTRP(sbuff, sptr));
			}
			else
			{
				me->txtTargetCountry->SetText(CSTR("Unk"));
			}
			Text::StringBuilderUTF8 sb;
			sb.AppendJoin(rec->Iterator(), CSTR("\r\n"));
			me->txtTargetWhois->SetText(sb.ToCString());
		}
		else
		{
			me->txtTargetWhois->SetText(CSTR(""));
			me->txtTargetName->SetText(CSTR(""));
			me->txtTargetCountry->SetText(CSTR(""));
		}
		Sync::MutexUsage mutUsage(target->mut);
		me->lbTargetDomains->ClearItems();
		i = 0;
		j = target->addrList.GetCount();
		while (i < j)
		{
			me->lbTargetDomains->AddItem(Text::String::OrEmpty(target->addrList.GetItem(i)), 0);
			i++;
		}
		mutUsage.EndUse();
	}
	else
	{
		me->txtTargetWhois->SetText(CSTR(""));
		me->txtTargetName->SetText(CSTR(""));
		me->txtTargetCountry->SetText(CSTR(""));
		me->lbTargetDomains->ClearItems();
	}
}

void __stdcall SSWR::AVIRead::AVIRDNSProxyForm::OnDNSSwitchClicked(void *userObj)
{
	SSWR::AVIRead::AVIRDNSProxyForm *me = (SSWR::AVIRead::AVIRDNSProxyForm*)userObj;
	me->proxy->SwitchDNS();
}

void __stdcall SSWR::AVIRead::AVIRDNSProxyForm::OnDNSSetClicked(void *userObj)
{
	SSWR::AVIRead::AVIRDNSProxyForm *me = (SSWR::AVIRead::AVIRDNSProxyForm*)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtDNSServer2->GetText(sb);
	UInt32 svrIP = Net::SocketUtil::GetIPAddr(sb.ToCString());
	if (svrIP == 0)
	{
		me->ui->ShowMsgOK(CSTR("Error in parsing server ip"), CSTR("Error"), me);
		return;
	}
	else
	{
		me->proxy->SetServerIP(svrIP);
		me->UpdateDNSList();
	}
}

void __stdcall SSWR::AVIRead::AVIRDNSProxyForm::OnDNSAddClicked(void *userObj)
{
	SSWR::AVIRead::AVIRDNSProxyForm *me = (SSWR::AVIRead::AVIRDNSProxyForm*)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtDNSServer2->GetText(sb);
	UInt32 svrIP = Net::SocketUtil::GetIPAddr(sb.ToCString());
	if (svrIP == 0)
	{
		me->ui->ShowMsgOK(CSTR("Error in parsing server ip"), CSTR("Error"), me);
		return;
	}
	else
	{
		me->proxy->AddDNSIP(svrIP);
		me->UpdateDNSList();
	}
}

void __stdcall SSWR::AVIRead::AVIRDNSProxyForm::OnSearchClicked(void *userObj)
{
	SSWR::AVIRead::AVIRDNSProxyForm *me = (SSWR::AVIRead::AVIRDNSProxyForm*)userObj;
	Data::ArrayList<Text::String *> nameList;
	Text::StringBuilderUTF8 sb;
	UInt32 ip;
	UInt32 mask;
	UOSInt i;
	UOSInt j;
	me->txtSearchIPRange->GetText(sb);
	ip = Net::SocketUtil::GetIPAddr(sb.ToCString());
	if (ip == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter IP Range"), CSTR("Search"), me);
		return;
	}
	sb.ClearStr();
	me->txtSearchMask->GetText(sb);
	if (sb.ToUInt32(mask))
	{
		if (mask >= 32)
		{
			mask = (UInt32)-1;
		}
		else
		{
			mask = (UInt32)~((1 << (32 - mask)) - 1);
			mask = BSWAPU32(mask);
		}
	}
	else if ((mask = Net::SocketUtil::GetIPAddr(sb.ToCString())) != 0)
	{
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid mask"), CSTR("Search"), me);
		return;
	}
	me->proxy->SearchIPv4(&nameList, ip, mask);

	me->lbSearch->ClearItems();
	i = 0;
	j = nameList.GetCount();
	while (i < j)
	{
		sb.ClearStr();
		sb.Append(nameList.GetItem(i));
		me->lbSearch->AddItem(sb.ToCString(), 0);
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRDNSProxyForm::OnSReqSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRDNSProxyForm *me = (SSWR::AVIRead::AVIRDNSProxyForm*)userObj;
	Net::DNSClient::FreeAnswers(me->v4sansList);
	UOSInt i;
	UOSInt j;

	me->lbSAnswer->ClearItems();
	NotNullPtr<Text::String> req;
	if (me->lbSearch->GetSelectedItemTextNew().SetTo(req))
	{
		UTF8Char sbuff[32];
		UTF8Char *sptr;
		Net::DNSClient::RequestAnswer *ans;
		Data::DateTime reqTime;
		UInt32 ttl;
		if (me->proxy->GetRequestInfov4(req->ToCString(), me->v4sansList, reqTime, ttl))
		{
			UOSInt bestInd = (UOSInt)-1;
			i = 0;
			j = me->v4sansList.GetCount();
			while (i < j)
			{
				ans = me->v4sansList.GetItem(i);
				if (bestInd == (UOSInt)-1 && ans->recType == 1)
					bestInd = i;
				me->lbSAnswer->AddItem(ans->name, ans);
				i++;
			}
			if (bestInd == (UOSInt)-1)
				bestInd = 0;
			if (j > 0)
			{
				me->lbSAnswer->SetSelectedIndex(bestInd);
			}
			reqTime.ToLocalTime();
			sptr = reqTime.ToString(sbuff);
			me->txtSRequestTime->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrUInt32(sbuff, ttl);
			me->txtSRequestTTL->SetText(CSTRP(sbuff, sptr));
		}
		else
		{
		}
		req->Release();
	}
}

void __stdcall SSWR::AVIRead::AVIRDNSProxyForm::OnSAnsSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRDNSProxyForm *me = (SSWR::AVIRead::AVIRDNSProxyForm*)userObj;
	Net::DNSClient::RequestAnswer *ans = (Net::DNSClient::RequestAnswer*)me->lbSAnswer->GetSelectedItem();
	if (ans)
	{
		UTF8Char sbuff[16];
		UTF8Char *sptr;
		Data::DateTime t;
		me->txtSAnsName->SetText(ans->name->ToCString());
		sptr = Text::StrInt32(sbuff, ans->recType);
		me->txtSAnsType->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrInt32(sbuff, ans->recClass);
		me->txtSAnsClass->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrUInt32(sbuff, ans->ttl);
		me->txtSAnsTTL->SetText(CSTRP(sbuff, sptr));
		if (ans->rd)
		{
			me->txtSAnsRD->SetText(ans->rd->ToCString());
		}
		else
		{
			me->txtSAnsRD->SetText(CSTR(""));
		}
	}
	else
	{
		me->txtSAnsName->SetText(CSTR(""));
		me->txtSAnsType->SetText(CSTR(""));
		me->txtSAnsClass->SetText(CSTR(""));
		me->txtSAnsTTL->SetText(CSTR(""));
		me->txtSAnsRD->SetText(CSTR(""));
	}
}

void __stdcall SSWR::AVIRead::AVIRDNSProxyForm::OnClientSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRDNSProxyForm *me = (SSWR::AVIRead::AVIRDNSProxyForm*)userObj;
	ClientInfo *cli = (ClientInfo*)me->lbClientIP->GetSelectedItem();
	me->lvClient->ClearItems();
	if (cli)
	{
		UTF8Char sbuff[32];
		UTF8Char *sptr;
		UOSInt i;
		UOSInt j;
		HourInfo *hInfo;
		Sync::MutexUsage mutUsage(cli->mut);
		i = cli->hourInfos.GetCount();
		while (i-- > 0)
		{
			hInfo = cli->hourInfos.GetItem(i);
			sptr = Text::StrInt32(sbuff, hInfo->year);
			sptr = Text::StrConcatC(sptr, UTF8STRC("-"));
			sptr = Text::StrInt32(sptr, hInfo->month);
			sptr = Text::StrConcatC(sptr, UTF8STRC("-"));
			sptr = Text::StrInt32(sptr, hInfo->day);
			sptr = Text::StrConcatC(sptr, UTF8STRC("-"));
			sptr = Text::StrInt32(sptr, hInfo->hour);
			sptr = Text::StrConcatC(sptr, UTF8STRC(":00"));
			j = me->lvClient->AddItem(CSTRP(sbuff, sptr), 0);
			sptr = Text::StrInt64(sbuff, hInfo->reqCount);
			me->lvClient->SetSubItem(j, 1, CSTRP(sbuff, sptr));
		}
		mutUsage.EndUse();
	}
}

void __stdcall SSWR::AVIRead::AVIRDNSProxyForm::OnDisableV6Chg(void *userObj, Bool isChecked)
{
	SSWR::AVIRead::AVIRDNSProxyForm *me = (SSWR::AVIRead::AVIRDNSProxyForm*)userObj;
	me->proxy->SetDisableV6(isChecked);
}

void __stdcall SSWR::AVIRead::AVIRDNSProxyForm::OnBlackListClicked(void *userObj)
{
	SSWR::AVIRead::AVIRDNSProxyForm *me = (SSWR::AVIRead::AVIRDNSProxyForm*)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtBlackList->GetText(sb);
	if (sb.GetLength() > 0)
	{
		if (me->proxy->AddBlackList(sb.ToCString()))
		{
			me->txtBlackList->SetText(CSTR(""));
			me->UpdateBlackList();
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRDNSProxyForm::OnWPADClicked(void *userObj)
{
	SSWR::AVIRead::AVIRDNSProxyForm *me = (SSWR::AVIRead::AVIRDNSProxyForm*)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtWPAD->GetText(sb);
	if (sb.GetLength() > 0)
	{
		Net::SocketUtil::AddressInfo addr;
		if (Net::SocketUtil::GetIPAddr(sb.ToCString(), addr))
		{
			me->proxy->SetWebProxyAutoDiscovery(&addr);
		}
		else
		{
			me->ui->ShowMsgOK(CSTR("Please input a valid IP address in WPAD"), CSTR("DNS Proxy"), me);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRDNSProxyForm::OnDNSRequest(void *userObj, Text::CString reqName, Int32 reqType, Int32 reqClass, NotNullPtr<const Net::SocketUtil::AddressInfo> reqAddr, UInt16 reqPort, UInt32 reqId, Double timeUsed)
{
	SSWR::AVIRead::AVIRDNSProxyForm *me = (SSWR::AVIRead::AVIRDNSProxyForm*)userObj;
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

	ClientInfo *cli;
	UInt32 cliId = Net::SocketUtil::CalcCliId(reqAddr);
	Sync::MutexUsage ciMutUsage(me->cliInfoMut);
	cli = me->cliInfos.Get(cliId);
	if (cli == 0)
	{
		NEW_CLASS(cli, ClientInfo());
		cli->cliId = cliId;
		cli->addr = reqAddr.Ptr()[0];
		me->cliInfos.Put(cliId, cli);
		me->cliChg = true;
	}
	ciMutUsage.EndUse();
	Data::DateTime dt;
	HourInfo *hInfo;
	dt.SetCurrTimeUTC();
	Sync::MutexUsage mutUsage(cli->mut);
	hInfo = cli->hourInfos.GetItem(0);
	if (hInfo != 0 && hInfo->year == dt.GetYear() && hInfo->month == dt.GetMonth() && hInfo->day == dt.GetDay() && hInfo->hour == dt.GetHour())
	{
		hInfo->reqCount++;
	}
	else
	{
		if (cli->hourInfos.GetCount() >= 72)
		{
			hInfo = cli->hourInfos.RemoveAt(71);
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
		cli->hourInfos.Insert(0, hInfo);
	}
	mutUsage.EndUse();
}

void SSWR::AVIRead::AVIRDNSProxyForm::UpdateDNSList()
{
	WChar wbuff[32];
	Data::ArrayList<UInt32> ipList;
	UOSInt i;
	UOSInt j;
	UInt32 ip;
	this->proxy->GetDNSList(&ipList);
	this->lbDNSList->ClearItems();
	i = 0;
	j = ipList.GetCount();
	while (i < j)
	{
		ip = ipList.GetItem(i);
		Net::SocketUtil::GetIPv4Name(wbuff, ip);
		this->lbDNSList->AddItem(wbuff, (void*)(OSInt)ip);
		i++;
	}
}

void SSWR::AVIRead::AVIRDNSProxyForm::UpdateBlackList()
{
	Data::ArrayListStringNN blackList;
	this->proxy->GetBlackList(blackList);
	this->lbBlackList->ClearItems();
	Data::ArrayIterator<NotNullPtr<Text::String>> it = blackList.Iterator();
	while (it.HasNext())
	{
		this->lbBlackList->AddItem(it.Next(), 0);
	}
	
}

SSWR::AVIRead::AVIRDNSProxyForm::AVIRDNSProxyForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui), whois(core->GetSocketFactory(), 15000)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("DNS Proxy"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->currServer = 0;
	this->currTarget = 0;

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, *this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpStatus = this->tcMain->AddTabPage(CSTR("Status"));
	this->lblDNSPort = ui->NewLabel(this->tpStatus, CSTR("Listening Port"));
	this->lblDNSPort->SetRect(4, 4, 100, 23, false);
	this->txtDNSPort = ui->NewTextBox(this->tpStatus, CSTR("53"));
	this->txtDNSPort->SetRect(104, 4, 100, 23, false);
	this->txtDNSPort->SetReadOnly(true);
	this->lblDisableV6 = ui->NewLabel(this->tpStatus, CSTR("Disable IPv6"));
	this->lblDisableV6->SetRect(4, 28, 100, 23, false);
	this->chkDisableV6 = ui->NewCheckBox(this->tpStatus, CSTR(""), false);
	this->chkDisableV6->SetRect(104, 28, 100, 23, false);
	this->chkDisableV6->HandleCheckedChange(OnDisableV6Chg, this);
	this->lblDNSServer = ui->NewLabel(this->tpStatus, CSTR("DNS Server"));
	this->lblDNSServer->SetRect(4, 52, 100, 23, false);
	this->txtDNSServer = ui->NewTextBox(this->tpStatus, CSTR(""));
	this->txtDNSServer->SetRect(104, 52, 200, 23, false);
	this->txtDNSServer->SetReadOnly(true);
	this->btnDNSSwitch = ui->NewButton(this->tpStatus, CSTR("Switch"));
	this->btnDNSSwitch->SetRect(304, 52, 75, 23, false);
	this->btnDNSSwitch->HandleButtonClick(OnDNSSwitchClicked, this);
	this->lblDNSList = ui->NewLabel(this->tpStatus, CSTR("DNS List"));
	this->lblDNSList->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->lbDNSList, UI::GUIListBox(ui, this->tpStatus, false));
	this->lbDNSList->SetRect(104, 76, 100, 240, false);
	this->txtDNSServer2 = ui->NewTextBox(this->tpStatus, CSTR(""));
	this->txtDNSServer2->SetRect(104, 316, 100, 23, false);
	this->btnDNSAdd = ui->NewButton(this->tpStatus, CSTR("&Add"));
	this->btnDNSAdd->SetRect(204, 316, 75, 23, false);
	this->btnDNSAdd->HandleButtonClick(OnDNSAddClicked, this);
	this->btnDNSSet = ui->NewButton(this->tpStatus, CSTR("&Set"));
	this->btnDNSSet->SetRect(284, 316, 75, 23, false);
	this->btnDNSSet->HandleButtonClick(OnDNSSetClicked, this);
	this->lblWPAD = ui->NewLabel(this->tpStatus, CSTR("WPAD Address"));
	this->lblWPAD->SetRect(4, 340, 100, 23, false);
	this->txtWPAD = ui->NewTextBox(this->tpStatus, CSTR(""));
	this->txtWPAD->SetRect(104, 340, 100, 23, false);
	this->btnWPAD = ui->NewButton(this->tpStatus, CSTR("Set"));
	this->btnWPAD->SetRect(204, 340, 75, 23, false);
	this->btnWPAD->HandleButtonClick(OnWPADClicked, this);

	this->tpV4Main = this->tcMain->AddTabPage(CSTR("Req v4"));
	NEW_CLASS(this->lbV4Request, UI::GUIListBox(ui, this->tpV4Main, false));
	this->lbV4Request->SetRect(0, 0, 200, 100, false);
	this->lbV4Request->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbV4Request->HandleSelectionChange(OnV4ReqSelChg, this);
	this->hspV4Request = ui->NewHSplitter(this->tpV4Main, 3, false);
	NEW_CLASSNN(this->pnlV4Request, UI::GUIPanel(ui, this->tpV4Main));
	this->pnlV4Request->SetRect(0, 0, 100, 56, false);
	this->pnlV4Request->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblV4RequestTime = ui->NewLabel(this->pnlV4Request, CSTR("Request Time"));
	this->lblV4RequestTime->SetRect(4, 4, 100, 23, false);
	this->txtV4RequestTime = ui->NewTextBox(this->pnlV4Request, CSTR(""));
	this->txtV4RequestTime->SetRect(104, 4, 200, 23, false);
	this->txtV4RequestTime->SetReadOnly(true);
	this->lblV4RequestTTL = ui->NewLabel(this->pnlV4Request, CSTR("TTL"));
	this->lblV4RequestTTL->SetRect(4, 28, 100, 23, false);
	this->txtV4RequestTTL = ui->NewTextBox(this->pnlV4Request, CSTR(""));
	this->txtV4RequestTTL->SetRect(104, 27, 200, 23, false);
	this->txtV4RequestTTL->SetReadOnly(true);
	NEW_CLASS(this->lbV4Answer, UI::GUIListBox(ui, this->tpV4Main, false));
	this->lbV4Answer->SetRect(0, 0, 200, 100, false);
	this->lbV4Answer->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbV4Answer->HandleSelectionChange(OnV4AnsSelChg, this);
	this->hspV4Answer = ui->NewHSplitter(this->tpV4Main, 3, false);
	NEW_CLASSNN(this->pnlV4Detail, UI::GUIPanel(ui, this->tpV4Main));
	this->pnlV4Detail->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lblV4AnsName = ui->NewLabel(this->pnlV4Detail, CSTR("Name"));
	this->lblV4AnsName->SetRect(4, 4, 100, 23, false);
	this->txtV4AnsName = ui->NewTextBox(this->pnlV4Detail, CSTR(""));
	this->txtV4AnsName->SetRect(104, 4, 200, 23, false);
	this->txtV4AnsName->SetReadOnly(true);
	this->lblV4AnsType = ui->NewLabel(this->pnlV4Detail, CSTR("Type"));
	this->lblV4AnsType->SetRect(4, 28, 100, 23, false);
	this->txtV4AnsType = ui->NewTextBox(this->pnlV4Detail, CSTR(""));
	this->txtV4AnsType->SetRect(104, 28, 100, 23, false);
	this->txtV4AnsType->SetReadOnly(true);
	this->lblV4AnsClass = ui->NewLabel(this->pnlV4Detail, CSTR("Class"));
	this->lblV4AnsClass->SetRect(4, 52, 100, 23, false);
	this->txtV4AnsClass = ui->NewTextBox(this->pnlV4Detail, CSTR(""));
	this->txtV4AnsClass->SetRect(104, 52, 100, 23, false);
	this->txtV4AnsClass->SetReadOnly(true);
	this->lblV4AnsTTL = ui->NewLabel(this->pnlV4Detail, CSTR("TTL"));
	this->lblV4AnsTTL->SetRect(4, 76, 100, 23, false);
	this->txtV4AnsTTL = ui->NewTextBox(this->pnlV4Detail, CSTR(""));
	this->txtV4AnsTTL->SetRect(104, 76, 100, 23, false);
	this->txtV4AnsTTL->SetReadOnly(true);
	this->lblV4AnsRD = ui->NewLabel(this->pnlV4Detail, CSTR("RD"));
	this->lblV4AnsRD->SetRect(4, 100, 100, 23, false);
	this->txtV4AnsRD = ui->NewTextBox(this->pnlV4Detail, CSTR(""));
	this->txtV4AnsRD->SetRect(104, 100, 160, 23, false);
	this->txtV4AnsRD->SetReadOnly(true);

	this->tpV6Main = this->tcMain->AddTabPage(CSTR("Req v6"));
	NEW_CLASS(this->lbV6Request, UI::GUIListBox(ui, this->tpV6Main, false));
	this->lbV6Request->SetRect(0, 0, 200, 100, false);
	this->lbV6Request->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbV6Request->HandleSelectionChange(OnV6ReqSelChg, this);
	this->hspV6Request = ui->NewHSplitter(this->tpV6Main, 3, false);
	NEW_CLASSNN(this->pnlV6Request, UI::GUIPanel(ui, this->tpV6Main));
	this->pnlV6Request->SetRect(0, 0, 100, 56, false);
	this->pnlV6Request->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblV6RequestTime = ui->NewLabel(this->pnlV6Request, CSTR("Request Time"));
	this->lblV6RequestTime->SetRect(4, 4, 100, 23, false);
	this->txtV6RequestTime = ui->NewTextBox(this->pnlV6Request, CSTR(""));
	this->txtV6RequestTime->SetRect(104, 4, 200, 23, false);
	this->txtV6RequestTime->SetReadOnly(true);
	this->lblV6RequestTTL = ui->NewLabel(this->pnlV6Request, CSTR("TTL"));
	this->lblV6RequestTTL->SetRect(4, 28, 100, 23, false);
	this->txtV6RequestTTL = ui->NewTextBox(this->pnlV6Request, CSTR(""));
	this->txtV6RequestTTL->SetRect(104, 27, 200, 23, false);
	this->txtV6RequestTTL->SetReadOnly(true);
	NEW_CLASS(this->lbV6Answer, UI::GUIListBox(ui, this->tpV6Main, false));
	this->lbV6Answer->SetRect(0, 0, 200, 100, false);
	this->lbV6Answer->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbV6Answer->HandleSelectionChange(OnV6AnsSelChg, this);
	this->hspV6Answer = ui->NewHSplitter(this->tpV6Main, 3, false);
	NEW_CLASSNN(this->pnlV6Detail, UI::GUIPanel(ui, this->tpV6Main));
	this->pnlV6Detail->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lblV6AnsName = ui->NewLabel(this->pnlV6Detail, CSTR("Name"));
	this->lblV6AnsName->SetRect(4, 4, 100, 23, false);
	this->txtV6AnsName = ui->NewTextBox(this->pnlV6Detail, CSTR(""));
	this->txtV6AnsName->SetRect(104, 4, 200, 23, false);
	this->txtV6AnsName->SetReadOnly(true);
	this->lblV6AnsType = ui->NewLabel(this->pnlV6Detail, CSTR("Type"));
	this->lblV6AnsType->SetRect(4, 28, 100, 23, false);
	this->txtV6AnsType = ui->NewTextBox(this->pnlV6Detail, CSTR(""));
	this->txtV6AnsType->SetRect(104, 28, 100, 23, false);
	this->txtV6AnsType->SetReadOnly(true);
	this->lblV6AnsClass = ui->NewLabel(this->pnlV6Detail, CSTR("Class"));
	this->lblV6AnsClass->SetRect(4, 52, 100, 23, false);
	this->txtV6AnsClass = ui->NewTextBox(this->pnlV6Detail, CSTR(""));
	this->txtV6AnsClass->SetRect(104, 52, 100, 23, false);
	this->txtV6AnsClass->SetReadOnly(true);
	this->lblV6AnsTTL = ui->NewLabel(this->pnlV6Detail, CSTR("TTL"));
	this->lblV6AnsTTL->SetRect(4, 76, 100, 23, false);
	this->txtV6AnsTTL = ui->NewTextBox(this->pnlV6Detail, CSTR(""));
	this->txtV6AnsTTL->SetRect(104, 76, 100, 23, false);
	this->txtV6AnsTTL->SetReadOnly(true);
	this->lblV6AnsRD = ui->NewLabel(this->pnlV6Detail, CSTR("RD"));
	this->lblV6AnsRD->SetRect(4, 100, 100, 23, false);
	this->txtV6AnsRD = ui->NewTextBox(this->pnlV6Detail, CSTR(""));
	this->txtV6AnsRD->SetRect(104, 100, 160, 23, false);
	this->txtV6AnsRD->SetReadOnly(true);

	this->tpOthMain = this->tcMain->AddTabPage(CSTR("Req Oth"));
	NEW_CLASS(this->lbOthRequest, UI::GUIListBox(ui, this->tpOthMain, false));
	this->lbOthRequest->SetRect(0, 0, 200, 100, false);
	this->lbOthRequest->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbOthRequest->HandleSelectionChange(OnOthReqSelChg, this);
	this->hspOthRequest = ui->NewHSplitter(this->tpOthMain, 3, false);
	NEW_CLASSNN(this->pnlOthRequest, UI::GUIPanel(ui, this->tpOthMain));
	this->pnlOthRequest->SetRect(0, 0, 100, 56, false);
	this->pnlOthRequest->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblOthRequestTime = ui->NewLabel(this->pnlOthRequest, CSTR("Request Time"));
	this->lblOthRequestTime->SetRect(4, 4, 100, 23, false);
	this->txtOthRequestTime = ui->NewTextBox(this->pnlOthRequest, CSTR(""));
	this->txtOthRequestTime->SetRect(104, 4, 200, 23, false);
	this->txtOthRequestTime->SetReadOnly(true);
	this->lblOthRequestTTL = ui->NewLabel(this->pnlOthRequest, CSTR("TTL"));
	this->lblOthRequestTTL->SetRect(4, 28, 100, 23, false);
	this->txtOthRequestTTL = ui->NewTextBox(this->pnlOthRequest, CSTR(""));
	this->txtOthRequestTTL->SetRect(104, 27, 200, 23, false);
	this->txtOthRequestTTL->SetReadOnly(true);
	NEW_CLASS(this->lbOthAnswer, UI::GUIListBox(ui, this->tpOthMain, false));
	this->lbOthAnswer->SetRect(0, 0, 200, 100, false);
	this->lbOthAnswer->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbOthAnswer->HandleSelectionChange(OnOthAnsSelChg, this);
	this->hspOthAnswer = ui->NewHSplitter(this->tpOthMain, 3, false);
	NEW_CLASSNN(this->pnlOthDetail, UI::GUIPanel(ui, this->tpOthMain));
	this->pnlOthDetail->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lblOthAnsName = ui->NewLabel(this->pnlOthDetail, CSTR("Name"));
	this->lblOthAnsName->SetRect(4, 4, 100, 23, false);
	this->txtOthAnsName = ui->NewTextBox(this->pnlOthDetail, CSTR(""));
	this->txtOthAnsName->SetRect(104, 4, 200, 23, false);
	this->txtOthAnsName->SetReadOnly(true);
	this->lblOthAnsType = ui->NewLabel(this->pnlOthDetail, CSTR("Type"));
	this->lblOthAnsType->SetRect(4, 28, 100, 23, false);
	this->txtOthAnsType = ui->NewTextBox(this->pnlOthDetail, CSTR(""));
	this->txtOthAnsType->SetRect(104, 28, 100, 23, false);
	this->txtOthAnsType->SetReadOnly(true);
	this->lblOthAnsClass = ui->NewLabel(this->pnlOthDetail, CSTR("Class"));
	this->lblOthAnsClass->SetRect(4, 52, 100, 23, false);
	this->txtOthAnsClass = ui->NewTextBox(this->pnlOthDetail, CSTR(""));
	this->txtOthAnsClass->SetRect(104, 52, 100, 23, false);
	this->txtOthAnsClass->SetReadOnly(true);
	this->lblOthAnsTTL = ui->NewLabel(this->pnlOthDetail, CSTR("TTL"));
	this->lblOthAnsTTL->SetRect(4, 76, 100, 23, false);
	this->txtOthAnsTTL = ui->NewTextBox(this->pnlOthDetail, CSTR(""));
	this->txtOthAnsTTL->SetRect(104, 76, 100, 23, false);
	this->txtOthAnsTTL->SetReadOnly(true);
	this->lblOthAnsRD = ui->NewLabel(this->pnlOthDetail, CSTR("RD"));
	this->lblOthAnsRD->SetRect(4, 100, 100, 23, false);
	this->txtOthAnsRD = ui->NewTextBox(this->pnlOthDetail, CSTR(""));
	this->txtOthAnsRD->SetRect(104, 100, 160, 23, false);
	this->txtOthAnsRD->SetReadOnly(true);

	this->tpTarget = this->tcMain->AddTabPage(CSTR("Target"));
	NEW_CLASS(this->lbTarget, UI::GUIListBox(ui, this->tpTarget, false));
	this->lbTarget->SetRect(0, 0, 150, 100, false);
	this->lbTarget->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbTarget->HandleSelectionChange(OnTargetSelChg, this);
	this->hspTarget = ui->NewHSplitter(this->tpTarget, 3, false);
	NEW_CLASS(this->tcTarget, UI::GUITabControl(ui, this->tpTarget));
	this->tcTarget->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpTargetInfo = this->tcTarget->AddTabPage(CSTR("Info"));
	this->lblTargetName = ui->NewLabel(this->tpTargetInfo, CSTR("Name"));
	this->lblTargetName->SetRect(4, 4, 100, 23, false);
	this->txtTargetName = ui->NewTextBox(this->tpTargetInfo, CSTR(""));
	this->txtTargetName->SetRect(104, 4, 200, 23, false);
	this->txtTargetName->SetReadOnly(true);
	this->lblTargetCountry = ui->NewLabel(this->tpTargetInfo, CSTR("Country"));
	this->lblTargetCountry->SetRect(4, 28, 100, 23, false);
	this->txtTargetCountry = ui->NewTextBox(this->tpTargetInfo, CSTR(""));
	this->txtTargetCountry->SetRect(104, 28, 100, 23, false);
	this->txtTargetCountry->SetReadOnly(true);
	this->lblTargetDomains = ui->NewLabel(this->tpTargetInfo, CSTR("Domains"));
	this->lblTargetDomains->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->lbTargetDomains, UI::GUIListBox(ui, this->tpTargetInfo, false));
	this->lbTargetDomains->SetRect(104, 52, 400, 300, false);
	this->tpTargetWhois = this->tcTarget->AddTabPage(CSTR("Whois"));
	this->txtTargetWhois = ui->NewTextBox(this->tpTargetWhois, CSTR(""), true);
	this->txtTargetWhois->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtTargetWhois->SetReadOnly(true);

	this->tpSearch = this->tcMain->AddTabPage(CSTR("Search"));
	NEW_CLASSNN(this->pnlSearch, UI::GUIPanel(ui, this->tpSearch));
	this->pnlSearch->SetRect(0, 0, 100, 31, false);
	this->pnlSearch->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblSearchIPRange = ui->NewLabel(this->pnlSearch, CSTR("IP Range"));
	this->lblSearchIPRange->SetRect(4, 4, 100, 23, false);
	this->txtSearchIPRange = ui->NewTextBox(this->pnlSearch, CSTR("202.128.224.0"));
	this->txtSearchIPRange->SetRect(104, 4, 100, 23, false);
	this->lblSearchMask = ui->NewLabel(this->pnlSearch, CSTR("Mask"));
	this->lblSearchMask->SetRect(204, 4, 100, 23, false);
	this->txtSearchMask = ui->NewTextBox(this->pnlSearch, CSTR("19"));
	this->txtSearchMask->SetRect(304, 4, 100, 23, false);
	this->btnSearch = ui->NewButton(this->pnlSearch, CSTR("Search"));
	this->btnSearch->SetRect(404, 4, 75, 23, false);
	this->btnSearch->HandleButtonClick(OnSearchClicked, this);
	NEW_CLASS(this->lbSearch, UI::GUIListBox(ui, this->tpSearch, false));
	this->lbSearch->SetRect(0, 0, 200, 100, false);
	this->lbSearch->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbSearch->HandleSelectionChange(OnSReqSelChg, this);
	this->hspSearch = ui->NewHSplitter(this->tpSearch, 3, false);
	NEW_CLASSNN(this->pnlSRequest, UI::GUIPanel(ui, this->tpSearch));
	this->pnlSRequest->SetRect(0, 0, 100, 56, false);
	this->pnlSRequest->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblSRequestTime = ui->NewLabel(this->pnlSRequest, CSTR("Request Time"));
	this->lblSRequestTime->SetRect(4, 4, 100, 23, false);
	this->txtSRequestTime = ui->NewTextBox(this->pnlSRequest, CSTR(""));
	this->txtSRequestTime->SetRect(104, 4, 200, 23, false);
	this->txtSRequestTime->SetReadOnly(true);
	this->lblSRequestTTL = ui->NewLabel(this->pnlSRequest, CSTR("TTL"));
	this->lblSRequestTTL->SetRect(4, 28, 100, 23, false);
	this->txtSRequestTTL = ui->NewTextBox(this->pnlSRequest, CSTR(""));
	this->txtSRequestTTL->SetRect(104, 27, 200, 23, false);
	this->txtSRequestTTL->SetReadOnly(true);
	NEW_CLASS(this->lbSAnswer, UI::GUIListBox(ui, this->tpSearch, false));
	this->lbSAnswer->SetRect(0, 0, 200, 100, false);
	this->lbSAnswer->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbSAnswer->HandleSelectionChange(OnSAnsSelChg, this);
	this->hspSAnswer = ui->NewHSplitter(this->tpSearch, 3, false);
	NEW_CLASSNN(this->pnlSDetail, UI::GUIPanel(ui, this->tpSearch));
	this->pnlSDetail->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lblSAnsName = ui->NewLabel(this->pnlSDetail, CSTR("Name"));
	this->lblSAnsName->SetRect(4, 4, 100, 23, false);
	this->txtSAnsName = ui->NewTextBox(this->pnlSDetail, CSTR(""));
	this->txtSAnsName->SetRect(104, 4, 200, 23, false);
	this->txtSAnsName->SetReadOnly(true);
	this->lblSAnsType = ui->NewLabel(this->pnlSDetail, CSTR("Type"));
	this->lblSAnsType->SetRect(4, 28, 100, 23, false);
	this->txtSAnsType = ui->NewTextBox(this->pnlSDetail, CSTR(""));
	this->txtSAnsType->SetRect(104, 28, 100, 23, false);
	this->txtSAnsType->SetReadOnly(true);
	this->lblSAnsClass = ui->NewLabel(this->pnlSDetail, CSTR("Class"));
	this->lblSAnsClass->SetRect(4, 52, 100, 23, false);
	this->txtSAnsClass = ui->NewTextBox(this->pnlSDetail, CSTR(""));
	this->txtSAnsClass->SetRect(104, 52, 100, 23, false);
	this->txtSAnsClass->SetReadOnly(true);
	this->lblSAnsTTL = ui->NewLabel(this->pnlSDetail, CSTR("TTL"));
	this->lblSAnsTTL->SetRect(4, 76, 100, 23, false);
	this->txtSAnsTTL = ui->NewTextBox(this->pnlSDetail, CSTR(""));
	this->txtSAnsTTL->SetRect(104, 76, 100, 23, false);
	this->txtSAnsTTL->SetReadOnly(true);
	this->lblSAnsRD = ui->NewLabel(this->pnlSDetail, CSTR("RD"));
	this->lblSAnsRD->SetRect(4, 100, 100, 23, false);
	this->txtSAnsRD = ui->NewTextBox(this->pnlSDetail, CSTR(""));
	this->txtSAnsRD->SetRect(104, 100, 160, 23, false);
	this->txtSAnsRD->SetReadOnly(true);

	this->tpBlackList = this->tcMain->AddTabPage(CSTR("Blacklist"));
	NEW_CLASSNN(this->pnlBlackList, UI::GUIPanel(ui, this->tpBlackList));
	this->pnlBlackList->SetRect(0, 0, 100, 32, false);
	this->pnlBlackList->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lbBlackList, UI::GUIListBox(ui, this->tpBlackList, false));
	this->lbBlackList->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtBlackList = ui->NewTextBox(this->pnlBlackList, CSTR(""));
	this->txtBlackList->SetRect(4, 4, 200, 23, false);
	this->btnBlackList = ui->NewButton(this->pnlBlackList, CSTR("Add"));
	this->btnBlackList->SetRect(204, 4, 75, 23, false);
	this->btnBlackList->HandleButtonClick(OnBlackListClicked, this);

	this->tpLog = this->tcMain->AddTabPage(CSTR("Log"));
	this->txtLog = ui->NewTextBox(this->tpLog, CSTR(""), false);
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASSNN(this->lbLog, UI::GUIListBox(ui, this->tpLog, false));
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpClient = this->tcMain->AddTabPage(CSTR("Client"));
	NEW_CLASS(this->lbClientIP, UI::GUIListBox(ui, this->tpClient, false));
	this->lbClientIP->SetRect(0, 0, 150, 23, false);
	this->lbClientIP->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbClientIP->HandleSelectionChange(OnClientSelChg, this);
	this->hspClient = ui->NewHSplitter(this->tpClient, 3, false);
	NEW_CLASS(this->lvClient, UI::GUIListView(ui, this->tpClient, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvClient->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvClient->AddColumn(CSTR("Time"), 200);
	this->lvClient->AddColumn(CSTR("Count"), 100);

	this->cliChg = false;

	NEW_CLASSNN(this->logger, UI::ListBoxLogger(*this, this->lbLog, 500, true));
	this->log.AddLogHandler(this->logger, IO::LogHandler::LogLevel::Raw);

	NEW_CLASS(this->proxy, Net::DNSProxy(this->core->GetSocketFactory(), true, this->log));
	this->proxy->HandleDNSRequest(OnDNSRequest, this);
	this->AddTimer(1000, OnTimerTick, this);
	UInt32 svrIP = this->proxy->GetServerIP();
	sptr = Net::SocketUtil::GetIPv4Name(sbuff, svrIP);
	this->txtDNSServer2->SetText(CSTRP(sbuff, sptr));

	this->UpdateDNSList();
}

SSWR::AVIRead::AVIRDNSProxyForm::~AVIRDNSProxyForm()
{
	UOSInt i;
	UOSInt j;
	ClientInfo *cli;
	DEL_CLASS(this->proxy);
	Net::DNSClient::FreeAnswers(this->v4ansList);
	Net::DNSClient::FreeAnswers(this->v6ansList);
	Net::DNSClient::FreeAnswers(this->othansList);
	Net::DNSClient::FreeAnswers(this->v4sansList);
	i = this->cliInfos.GetCount();
	while (i-- > 0)
	{
		cli = this->cliInfos.GetItem(i);
		j = cli->hourInfos.GetCount();
		while (j-- > 0)
		{
			MemFree(cli->hourInfos.GetItem(j));
		}
		DEL_CLASS(cli);
	}
	this->log.RemoveLogHandler(this->logger);
	this->logger.Delete();
}

void SSWR::AVIRead::AVIRDNSProxyForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Bool SSWR::AVIRead::AVIRDNSProxyForm::IsError()
{
	return this->proxy->IsError();
}

void SSWR::AVIRead::AVIRDNSProxyForm::SetDNSList(Data::ArrayList<UInt32> *dnsList)
{
	UOSInt i;
	UOSInt j;
	j = dnsList->GetCount();
	if (j > 0)
	{
		this->proxy->SetServerIP(dnsList->GetItem(0));
		i = 1;
		while (i < j)
		{
			this->proxy->AddDNSIP(dnsList->GetItem(i));
			i++;
		}
		this->UpdateDNSList();
	}
}

void SSWR::AVIRead::AVIRDNSProxyForm::SetDisableV6(Bool disableV6)
{
	this->proxy->SetDisableV6(disableV6);
	this->chkDisableV6->SetChecked(disableV6);
}

void SSWR::AVIRead::AVIRDNSProxyForm::AddBlacklist(Text::CStringNN blackList)
{
	if (blackList.leng == 0)
		return;
	this->proxy->AddBlackList(blackList);
	this->UpdateBlackList();
}
