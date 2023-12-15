#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Device/SIM7000.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

Bool __stdcall IO::Device::SIM7000::CheckATCommand(void *userObj, const UTF8Char *cmd, UOSInt cmdLen)
{
	UTF8Char sbuff[256];
	Text::PString sarr[4];
	UOSInt i;
	IO::Device::SIM7000 *me = (IO::Device::SIM7000*)userObj;
	if (me->nextReceive)
	{
		me->nextReceive = false;
		i = Text::StrHex2Bytes(cmd, sbuff);
		if (i == (me->recvSize >> 1))
		{
			if (me->recvHdlr)
			{
				me->recvHdlr(me->recvHdlrObj, me->recvIndex, me->recvIP, me->recvPort, sbuff, i);
			}
		}
		return true;
	}
	if (Text::StrStartsWithC(cmd, cmdLen, UTF8STRC("+CDNSGIP: ")))
	{
		Text::StrConcatC(sbuff, &cmd[10], cmdLen - 10);
		i = Text::StrSplitP(sarr, 4, {sbuff, cmdLen - 10}, ',');
		if (sarr[0].v[0] == '1' && i >= 3)
		{
			if (sarr[1].v[0] == '"')
			{
				i = Text::StrIndexOfCharC(&sarr[1].v[1], sarr[1].leng - 1, '"');
				sarr[1].v[i + 1] = 0;
				sarr[1].v++;
				sarr[1].leng = i;
			}
			if (sarr[2].v[0] == '"')
			{
				i = Text::StrIndexOfCharC(&sarr[2].v[1], sarr[2].leng - 1, '"');
				sarr[2].v[i + 1] = 0;
				sarr[2].v++;
				sarr[2].leng = i;
			}
			Sync::MutexUsage mutUsage(me->dnsMut);
			NotNullPtr<Net::SocketUtil::AddressInfo> dnsResp;
			if (me->dnsReq.v && dnsResp.Set(me->dnsResp))
			{
				if (sarr[1].Equals(me->dnsReq.v, me->dnsReq.leng) && Net::SocketUtil::GetIPAddr(sarr[2].ToCString(), dnsResp))
				{
					me->dnsResult = true;
					me->respEvt.Set();
				}
			}
			mutUsage.EndUse();
		}
		return true;
	}
	if (Text::StrStartsWithC(cmd, cmdLen, UTF8STRC("+RECEIVE,")))
	{
		me->nextReceive = true;
		Text::StrConcatC(sbuff, &cmd[9], cmdLen - 9);
		if (Text::StrSplitP(sarr, 4, {sbuff, cmdLen - 9}, ',') == 3)
		{
			me->recvIndex = Text::StrToUInt32(sarr[0].v);
			me->recvSize = Text::StrToUInt32(sarr[1].v);
			i = sarr[2].IndexOf(':');
			if (i != INVALID_INDEX)
			{
				sarr[2].v[i] = 0;
				sarr[2].leng = i;
				me->recvIP = Net::SocketUtil::GetIPAddr(sarr[2].ToCString());
				me->recvPort = 0;
				Text::StrToUInt16(&sarr[2].v[i + 1], me->recvPort);
			}
			else
			{
				me->recvIP = 0;
				me->recvPort = 0;
			}
		}
		else
		{
			me->recvIndex = 0;
			me->recvSize = 0;
			me->recvIP = 0;
			me->recvPort = 0;
		}
		
		return true;
	}
	if (cmd[0] >= '0' && cmd[0] <= '7' && cmd[1] == ',' && cmd[2] == ' ')
	{
		i = (UOSInt)cmd[0] - 48;
		if (Text::StrEqualsC(&cmd[3], cmdLen - 3, UTF8STRC("CONNECT OK")))
		{
			if (i == me->connInd)
			{
				me->connResult = 1;
				me->respEvt.Set();
			}
			return true;
		}
		else if (Text::StrEqualsC(&cmd[3], cmdLen - 3, UTF8STRC("ALREADY CONNECT")))
		{
			if (i == me->connInd)
			{
				me->connResult = 2;
				me->respEvt.Set();
			}
			return true;
		}
		else if (Text::StrEqualsC(&cmd[3], cmdLen - 3, UTF8STRC("CONNECT FAIL")))
		{
			if (i == me->connInd)
			{
				me->connResult = 3;
				me->respEvt.Set();
			}
			return true;
		}
		return false;
	}
	return false;
}

IO::Device::SIM7000::SIM7000(IO::ATCommandChannel *channel, Bool needRelease) : IO::GSMModemController(channel, needRelease)
{
	this->dnsReq = CSTR_NULL;
	this->dnsResp = 0;
	this->nextReceive = false;
	this->recvHdlr = 0;
	this->recvHdlrObj = 0;
	channel->SetCommandHandler(CheckATCommand, this);
}

IO::Device::SIM7000::~SIM7000()
{
	if (!this->needRelease)
	{
		this->channel->SetCommandHandler(0, 0);
	}
}

void IO::Device::SIM7000::SetReceiveHandler(ReceiveHandler recvHdlr, void *userObj)
{
	this->recvHdlrObj = userObj;
	this->recvHdlr = recvHdlr;
}

Bool IO::Device::SIM7000::SIMCOMPowerDown()
{
	Sync::MutexUsage mutUsage(this->cmdMut);
	this->channel->SendATCommand(this->cmdResults, UTF8STRC("AT+CPOWD=1"), 2000);
	UOSInt i = this->cmdResults.GetCount();
	Text::String *val;
	if (i > 1)
	{
		val = this->cmdResults.GetItem(i - 1);
		if (val->Equals(UTF8STRC("OK")) || val->Equals(UTF8STRC("NORMAL POWER DOWN")))
		{
			ClearCmdResult();
			return true;
		}
		else
		{
			ClearCmdResult();
			return false;
		}
	}
	else
	{
		ClearCmdResult();
		return false;
	}
}

Bool IO::Device::SIM7000::SIMCOMReadADC(OutParam<Int32> adc)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr = this->SendStringCommand(sbuff, UTF8STRC("AT+CADC?"), 3000);
	if (sptr && Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("+CADC: ")))
	{
		if (sbuff[7] == '1')
		{
			return Text::StrToInt32(&sbuff[9], adc);
		}
	}
	return false;
}

UTF8Char *IO::Device::SIM7000::SIMCOMGetICCID(UTF8Char *ccid)
{
	return this->SendStringCommand(ccid, UTF8STRC("AT+CCID"), 3000);
}

Bool IO::Device::SIM7000::SIMCOMGetFlashDeviceType(NotNullPtr<Text::StringBuilderUTF8> sb)
{
	return this->SendStringListCommand(sb, UTF8STRC("AT+CDEVICE?"));
}

Bool IO::Device::SIM7000::SIMCOMGetDeviceProductID(NotNullPtr<Text::StringBuilderUTF8> sb)
{
	return this->SendStringListCommand(sb, UTF8STRC("AT+GSV"));
}

UTF8Char *IO::Device::SIM7000::SIMCOMGetUESysInfo(UTF8Char *buff)
{
	return this->SendStringCommand(buff, UTF8STRC("AT+CPSI?"), 3000);
}

UTF8Char *IO::Device::SIM7000::SIMCOMGetNetworkAPN(UTF8Char *apn)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr = this->SendStringCommand(sbuff, UTF8STRC("AT+CGNAPN"), 3000);
	if (sptr && Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("+CGNAPN: ")))
	{
		if (sbuff[9] == '1')
		{
			if (sbuff[11] == '"')
			{
				sptr = &sbuff[12];
				UOSInt i = Text::StrCharCnt(sptr);
				if (sptr[i - 1] == '"')
				{
					sptr[i - 1] = 0;					
				}
				return Text::StrConcat(apn, sptr);
			}
			else
			{
				return Text::StrConcat(apn, &sbuff[11]);
			}
		}
	}
	return 0;
}

Bool IO::Device::SIM7000::NetSetMultiIP(Bool multiIP)
{
	if (multiIP)
	{
		return this->SendBoolCommandC(UTF8STRC("AT+CIPMUX=1"));
	}
	else
	{
		return this->SendBoolCommandC(UTF8STRC("AT+CIPMUX=0"));
	}
}

Bool IO::Device::SIM7000::NetIPStartTCP(UOSInt index, UInt32 ip, UInt16 port)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr = Text::StrConcatC(sbuff, UTF8STRC("AT+CIPSTART="));
	sptr = Text::StrUOSInt(sptr, index);
	sptr = Text::StrConcatC(sptr, UTF8STRC(",\"TCP\",\""));
	sptr = Net::SocketUtil::GetIPv4Name(sptr, ip);
	sptr = Text::StrConcatC(sptr, UTF8STRC("\","));
	sptr = Text::StrUInt16(sptr, port);
	this->connInd = index;
	this->connResult = 0;
	this->respEvt.Clear();
	if (this->SendBoolCommandC(sbuff, (UOSInt)(sptr - sbuff)))
	{
		this->respEvt.Wait(30000);
		if (this->connResult == 1 || this->connResult == 2)
		{
			return true;
		}
		return false;
	}
	else
	{
		return false;
	}	
}

Bool IO::Device::SIM7000::NetIPStartUDP(UOSInt index, UInt32 ip, UInt16 port)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr = Text::StrConcatC(sbuff, UTF8STRC("AT+CIPSTART="));
	sptr = Text::StrUOSInt(sptr, index);
	sptr = Text::StrConcatC(sptr, UTF8STRC(",\"UDP\",\""));
	sptr = Net::SocketUtil::GetIPv4Name(sptr, ip);
	sptr = Text::StrConcatC(sptr, UTF8STRC("\","));
	sptr = Text::StrUInt16(sptr, port);
	this->connInd = index;
	this->connResult = 0;
	this->respEvt.Clear();
	if (this->SendBoolCommandC(sbuff, (UOSInt)(sptr - sbuff)))
	{
		this->respEvt.Wait(30000);
		if (this->connResult == 1 || this->connResult == 2)
		{
			return true;
		}
		return false;
	}
	else
	{
		return false;
	}	
}

Bool IO::Device::SIM7000::NetIPSend(UOSInt index, const UInt8 *buff, UOSInt buffSize)
{
	Text::StringBuilderUTF8 sb;
	Sync::MutexUsage mutUsage;
	NotNullPtr<Text::String> cmdRes;
	if (!this->channel->UseCmd(mutUsage))
		return false;
	sb.AppendC(UTF8STRC("AT+CIPSEND="));
	sb.AppendUOSInt(index);
	sb.AppendUTF8Char(',');
	sb.AppendUOSInt(buffSize);
	sb.AppendUTF8Char('\r');
	this->channel->CmdSend(sb.ToString(), sb.GetCharCnt());
	Sync::SimpleThread::Sleep(1000);
	sb.ClearStr();
	sb.AppendHexBuff(buff, buffSize, 0, Text::LineBreakType::None);
	sb.AppendUTF8Char('\r');
	this->channel->CmdSend(sb.ToString(), sb.GetCharCnt());
	Bool ret = false;
	while (this->channel->CmdGetNextResult(5000).SetTo(cmdRes))
	{
		if (cmdRes->EndsWith(UTF8STRC(", SEND OK")))
		{
			ret = true;
		}
		cmdRes->Release();
		if (ret)
			break;
	}
	return ret;
}

Bool IO::Device::SIM7000::NetCloseSocket(UOSInt index)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr = Text::StrConcatC(sbuff, UTF8STRC("AT+CIPCLOSE="));
	sptr = Text::StrUOSInt(sptr, index);
	if ((sptr = this->SendStringCommandDirect(sbuff, sbuff, (UOSInt)(sptr - sbuff), 1000)) == 0)
	{
		return false;
	}
	return Text::StrEndsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("OK"));
}

Bool IO::Device::SIM7000::NetSetLocalPortTCP(UOSInt index, UInt16 port)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr = Text::StrConcatC(sbuff, UTF8STRC("AT+CLPORT="));
	sptr = Text::StrUOSInt(sptr, index);
	sptr = Text::StrConcatC(sptr, UTF8STRC(",\"TCP\","));
	sptr = Text::StrUInt16(sptr, port);
	return this->SendBoolCommandC(sbuff, (UOSInt)(sptr - sbuff));
}

Bool IO::Device::SIM7000::NetSetLocalPortUDP(UOSInt index, UInt16 port)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr = Text::StrConcatC(sbuff, UTF8STRC("AT+CLPORT="));
	sptr = Text::StrUOSInt(sptr, index);
	sptr = Text::StrConcatC(sptr, UTF8STRC(",\"UDP\","));
	sptr = Text::StrUInt16(sptr, port);
	return this->SendBoolCommandC(sbuff, (UOSInt)(sptr - sbuff));
}

Bool IO::Device::SIM7000::NetSetAPN(Text::CString apn)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr = Text::StrConcatC(sbuff, UTF8STRC("AT+CSTT=\""));
	sptr = apn.ConcatTo(sptr);
	*sptr++ = '"';
	*sptr = 0;
	return this->SendBoolCommandC(sbuff, (UOSInt)(sptr - sbuff));
}

Bool IO::Device::SIM7000::NetDataStart()
{
	return this->SendBoolCommandC(UTF8STRC("AT+CIICR"));
}

UTF8Char *IO::Device::SIM7000::NetGetIFAddr(UTF8Char *addr)
{
	return this->SendStringCommandDirect(addr, UTF8STRC("AT+CIFSR"), 1000);
}

Bool IO::Device::SIM7000::NetGetDNSList(Data::ArrayList<UInt32> *dnsList)
{
	Data::ArrayListNN<Text::String> resList;
	if (this->SendStringCommand(&resList, UTF8STRC("AT+CDNSCFG?"), 3000))
	{
		UOSInt i = 0;
		UOSInt j = resList.GetCount();
		UOSInt k;
		while (i < j)
		{
			Text::String *s;
			s = resList.GetItem(i);
			k = s->IndexOf(UTF8STRC("Dns: "));
			if (k != INVALID_INDEX && k > 0)
			{
				dnsList->Add(Net::SocketUtil::GetIPAddr(s->ToCString().Substring(k + 5)));
			}
			s->Release();
			i++;
		}
		return true;
	}
	return false;
}

Bool IO::Device::SIM7000::NetDNSResolveIP(Text::CString domain, NotNullPtr<Net::SocketUtil::AddressInfo> addr)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr = Text::StrConcatC(sbuff, UTF8STRC("AT+CDNSGIP=\""));
	sptr = domain.ConcatTo(sptr);
	*sptr++ = '"';
	*sptr = 0;
	Sync::MutexUsage mutUsage(this->dnsMut);
	while (this->dnsReq.v)
	{
		mutUsage.EndUse();
		Sync::SimpleThread::Sleep(100);
		mutUsage.BeginUse();
	}
	this->dnsResult = false;
	this->dnsReq = domain;
	this->dnsResp = addr.Ptr();
	this->respEvt.Clear();
	mutUsage.EndUse();
	if (!this->SendBoolCommandC(sbuff, (UOSInt)(sptr - sbuff), 3000))
	{
		return false;
	}
	this->respEvt.Wait(3000);
	Bool ret;
	mutUsage.BeginUse();
	ret = this->dnsResult;
	this->dnsReq = CSTR_NULL;
	this->dnsResp = 0;
	mutUsage.EndUse();
	return ret;
}

Bool IO::Device::SIM7000::NetShowRemoteIP(Bool show)
{
	if (show)
	{
		return this->SendBoolCommandC(UTF8STRC("AT+CIPSRIP=1"));
	}
	else
	{
		return this->SendBoolCommandC(UTF8STRC("AT+CIPSRIP=0"));
	}
}

Bool IO::Device::SIM7000::NetSetSendHex(Bool hexSend)
{
	if (hexSend)
	{
		return this->SendBoolCommandC(UTF8STRC("AT+CIPSENDHEX=1"));
	}
	else
	{
		return this->SendBoolCommandC(UTF8STRC("AT+CIPSENDHEX=0"));
	}
}

Bool IO::Device::SIM7000::NetSetOutputHex(Bool hexOutput)
{
	if (hexOutput)
	{
		return this->SendBoolCommandC(UTF8STRC("AT+CIPHEXS=2"));
	}
	else
	{
		return this->SendBoolCommandC(UTF8STRC("AT+CIPHEXS=1"));
	}
}

Bool IO::Device::SIM7000::NetSetDisableNagle(Bool disable)
{
	if (disable)
	{
		return this->SendBoolCommandC(UTF8STRC("AT+CIPOPTION=1"));
	}
	else
	{
		return this->SendBoolCommandC(UTF8STRC("AT+CIPOPTION=0"));
	}
}

Bool IO::Device::SIM7000::NetPing(const UTF8Char *addr, UInt32 *respTime, UInt32 *ttl)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr = sbuff;
	sptr = Text::StrConcatC(sptr, UTF8STRC("AT+CIPPING=\""));
	sptr = Text::StrConcat(sptr, addr);
	sptr = Text::StrConcatC(sptr, UTF8STRC("\",1"));
	if ((sptr = this->SendStringCommand(sbuff, sbuff, (UOSInt)(sptr - sbuff), 10000)) == 0)
	{
		return false;
	}
	if (!Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("+CIPPING: ")))
	{
		return false;
	}
	UTF8Char *sarr[5];
	if (Text::StrSplit(sarr, 5, &sbuff[10], ',') != 4)
	{
		return false;
	}

	if (respTime)
	{
		*respTime = Text::StrToUInt32(sarr[2]);
	}
	if (ttl)
	{
		*ttl = Text::StrToUInt32(sarr[3]);
	}
	return true;
}
