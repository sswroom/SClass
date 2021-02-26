#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Device/SIM7000.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

Bool __stdcall IO::Device::SIM7000::CheckATCommand(void *userObj, const Char *cmd)
{
	UTF8Char sbuff[256];
	UTF8Char *sarr[4];
	OSInt i;
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
	if (Text::StrStartsWith(cmd, "+CDNSGIP: "))
	{
		Text::StrConcat(sbuff, (const UTF8Char*)&cmd[10]);
		i = Text::StrSplit(sarr, 4, sbuff, ',');
		if (sarr[0][0] == '1' && i >= 3)
		{
			if (sarr[1][0] == '"')
			{
				i = Text::StrIndexOf(&sarr[1][1], '"');
				sarr[1][i + 1] = 0;
				sarr[1]++;
			}
			if (sarr[2][0] == '"')
			{
				i = Text::StrIndexOf(&sarr[2][1], '"');
				sarr[2][i + 1] = 0;
				sarr[2]++;
			}
			Sync::MutexUsage mutUsage(me->dnsMut);
			if (me->dnsReq && me->dnsResp)
			{
				if (Text::StrEquals(me->dnsReq, sarr[1]) && Net::SocketUtil::GetIPAddr(sarr[2], me->dnsResp))
				{
					me->dnsResult = true;
					me->respEvt->Set();
				}
			}
			mutUsage.EndUse();
		}
		return true;
	}
	if (Text::StrStartsWith(cmd, "+RECEIVE,"))
	{
		me->nextReceive = true;
		Text::StrConcat(sbuff, (const UTF8Char*)&cmd[9]);
		if (Text::StrSplit(sarr, 4, sbuff, ',') == 3)
		{
			me->recvIndex = Text::StrToInt32(sarr[0]);
			me->recvSize = Text::StrToInt32(sarr[1]);
			i = Text::StrIndexOf(sarr[2], ':');
			if (i >= 0)
			{
				sarr[2][i] = 0;
				me->recvIP = Net::SocketUtil::GetIPAddr(sarr[2]);
				me->recvPort = 0;
				Text::StrToUInt16(&sarr[2][i + 1], &me->recvPort);
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
		i = cmd[0] - 48;
		if (Text::StrEquals(&cmd[3], "CONNECT OK"))
		{
			if (i == me->connInd)
			{
				me->connResult = 1;
				me->respEvt->Set();
			}
			return true;
		}
		else if (Text::StrEquals(&cmd[3], "ALREADY CONNECT"))
		{
			if (i == me->connInd)
			{
				me->connResult = 2;
				me->respEvt->Set();
			}
			return true;
		}
		else if (Text::StrEquals(&cmd[3], "CONNECT FAIL"))
		{
			if (i == me->connInd)
			{
				me->connResult = 3;
				me->respEvt->Set();
			}
			return true;
		}
		return false;
	}
	return false;
}

IO::Device::SIM7000::SIM7000(IO::ATCommandChannel *channel, Bool needRelease) : IO::GSMModemController(channel, needRelease)
{
	NEW_CLASS(this->respEvt, Sync::Event(true, (const UTF8Char*)"IO.Device.SIM7000.respEvt"));
	NEW_CLASS(this->dnsMut, Sync::Mutex());
	this->dnsReq = 0;
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
	DEL_CLASS(this->dnsMut);
	DEL_CLASS(this->respEvt);
}

void IO::Device::SIM7000::SetReceiveHandler(ReceiveHandler recvHdlr, void *userObj)
{
	this->recvHdlrObj = userObj;
	this->recvHdlr = recvHdlr;
}

Bool IO::Device::SIM7000::SIMCOMPowerDown()
{
	Sync::MutexUsage mutUsage(this->cmdMut);
	this->channel->SendATCommand(this->cmdResults, "AT+CPOWD=1", 2000);
	OSInt i = this->cmdResults->GetCount();
	const Char *val;
	if (i > 1)
	{
		val = this->cmdResults->GetItem(i - 1);
		if (Text::StrEquals(val, "OK") || Text::StrEquals(val, "NORMAL POWER DOWN"))
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

Bool IO::Device::SIM7000::SIMCOMReadADC(Int32 *adc)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr = this->SendStringCommand(sbuff, "AT+CADC?", 3000);
	if (sptr && Text::StrStartsWith(sbuff, (const UTF8Char*)"+CADC: "))
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
	return this->SendStringCommand(ccid, "AT+CCID", 3000);
}

Bool IO::Device::SIM7000::SIMCOMGetFlashDeviceType(Text::StringBuilderUTF *sb)
{
	return this->SendStringListCommand(sb, "AT+CDEVICE?");
}

Bool IO::Device::SIM7000::SIMCOMGetDeviceProductID(Text::StringBuilderUTF *sb)
{
	return this->SendStringListCommand(sb, "AT+GSV");
}

UTF8Char *IO::Device::SIM7000::SIMCOMGetUESysInfo(UTF8Char *buff)
{
	return this->SendStringCommand(buff, "AT+CPSI?", 3000);
}

UTF8Char *IO::Device::SIM7000::SIMCOMGetNetworkAPN(UTF8Char *apn)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr = this->SendStringCommand(sbuff, "AT+CGNAPN", 3000);
	if (sptr && Text::StrStartsWith(sbuff, (const UTF8Char*)"+CGNAPN: "))
	{
		if (sbuff[9] == '1')
		{
			if (sbuff[11] == '"')
			{
				sptr = &sbuff[12];
				OSInt i = Text::StrCharCnt(sptr);
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
	return this->SendBoolCommand(multiIP?"AT+CIPMUX=1":"AT+CIPMUX=0");
}

Bool IO::Device::SIM7000::NetIPStartTCP(OSInt index, UInt32 ip, UInt16 port)
{
	Char sbuff[256];
	Char *sptr = Text::StrConcat(sbuff, "AT+CIPSTART=");
	sptr = Text::StrOSInt(sptr, index);
	sptr = Text::StrConcat(sptr, ",\"TCP\",\"");
	sptr = (Char*)Net::SocketUtil::GetIPv4Name((UTF8Char*)sptr, ip);
	sptr = Text::StrConcat(sptr, "\",");
	sptr = Text::StrUInt16(sptr, port);
	this->connInd = index;
	this->connResult = 0;
	this->respEvt->Clear();
	if (this->SendBoolCommand(sbuff))
	{
		this->respEvt->Wait(30000);
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

Bool IO::Device::SIM7000::NetIPStartUDP(OSInt index, UInt32 ip, UInt16 port)
{
	Char sbuff[256];
	Char *sptr = Text::StrConcat(sbuff, "AT+CIPSTART=");
	sptr = Text::StrOSInt(sptr, index);
	sptr = Text::StrConcat(sptr, ",\"UDP\",\"");
	sptr = (Char*)Net::SocketUtil::GetIPv4Name((UTF8Char*)sptr, ip);
	sptr = Text::StrConcat(sptr, "\",");
	sptr = Text::StrUInt16(sptr, port);
	this->connInd = index;
	this->connResult = 0;
	this->respEvt->Clear();
	if (this->SendBoolCommand(sbuff))
	{
		this->respEvt->Wait(30000);
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

Bool IO::Device::SIM7000::NetIPSend(OSInt index, const UInt8 *buff, OSInt buffSize)
{
	Text::StringBuilderUTF8 sb;
	Sync::MutexUsage mutUsage;
	const Char *cmdRes;
	if (!this->channel->UseCmd(&mutUsage))
		return false;
	sb.Append((const UTF8Char*)"AT+CIPSEND=");
	sb.AppendOSInt(index);
	sb.AppendChar(',', 1);
	sb.AppendOSInt(buffSize);
	sb.AppendChar('\r', 1);
	this->channel->CmdSend(sb.ToString(), sb.GetCharCnt());
	Sync::Thread::Sleep(1000);
	sb.ClearStr();
	sb.AppendHex(buff, buffSize, 0, Text::LBT_NONE);
	sb.AppendChar('\r', 1);
	this->channel->CmdSend(sb.ToString(), sb.GetCharCnt());
	Bool ret = false;
	while ((cmdRes = this->channel->CmdGetNextResult(5000)) != 0)
	{
		if (Text::StrEndsWith(cmdRes, ", SEND OK"))
		{
			ret = true;
		}
		Text::StrDelNew(cmdRes);
		if (ret)
			break;
	}
	return ret;
}

Bool IO::Device::SIM7000::NetCloseSocket(OSInt index)
{
	UTF8Char sbuff[256];
	Char *sptr = Text::StrConcat((Char*)sbuff, "AT+CIPCLOSE=");
	sptr = Text::StrOSInt(sptr, index);
	if (this->SendStringCommandDirect(sbuff, (const Char*)sbuff, 1000) == 0)
	{
		return false;
	}
	return Text::StrEndsWith(sbuff, (const UTF8Char*)"OK");
}

Bool IO::Device::SIM7000::NetSetLocalPortTCP(OSInt index, UInt16 port)
{
	Char sbuff[256];
	Char *sptr = Text::StrConcat(sbuff, "AT+CLPORT=");
	sptr = Text::StrOSInt(sptr, index);
	sptr = Text::StrConcat(sptr, ",\"TCP\",");
	sptr = Text::StrUInt16(sptr, port);
	return this->SendBoolCommand(sbuff);
}

Bool IO::Device::SIM7000::NetSetLocalPortUDP(OSInt index, UInt16 port)
{
	Char sbuff[256];
	Char *sptr = Text::StrConcat(sbuff, "AT+CLPORT=");
	sptr = Text::StrOSInt(sptr, index);
	sptr = Text::StrConcat(sptr, ",\"UDP\",");
	sptr = Text::StrUInt16(sptr, port);
	return this->SendBoolCommand(sbuff);
}

Bool IO::Device::SIM7000::NetSetAPN(const UTF8Char *apn)
{
	Char sbuff[256];
	Char *sptr = Text::StrConcat(sbuff, "AT+CSTT=\"");
	sptr = Text::StrConcat(sptr, (const Char*)apn);
	*sptr++ = '"';
	*sptr = 0;
	return this->SendBoolCommand(sbuff);
}

Bool IO::Device::SIM7000::NetDataStart()
{
	return this->SendBoolCommand("AT+CIICR");
}

UTF8Char *IO::Device::SIM7000::NetGetIFAddr(UTF8Char *addr)
{
	return this->SendStringCommandDirect(addr, "AT+CIFSR", 1000);
}

Bool IO::Device::SIM7000::NetGetDNSList(Data::ArrayList<UInt32> *dnsList)
{
	Data::ArrayList<const Char*> resList;
	if (this->SendStringCommand(&resList, "AT+CDNSCFG?", 3000))
	{
		OSInt i = 0;
		OSInt j = resList.GetCount();
		OSInt k;
		while (i < j)
		{
			const Char *sptr;
			sptr = resList.GetItem(i);
			k = Text::StrIndexOf(sptr, "Dns: ");
			if (k > 0)
			{
				dnsList->Add(Net::SocketUtil::GetIPAddr(&sptr[k + 5]));
			}
			Text::StrDelNew(sptr);
			i++;
		}
		return true;
	}
	return false;
}

Bool IO::Device::SIM7000::NetDNSResolveIP(const UTF8Char *domain, Net::SocketUtil::AddressInfo *addr)
{
	UTF8Char sbuff[256];
	Char *sptr = Text::StrConcat((Char*)sbuff, "AT+CDNSGIP=\"");
	sptr = Text::StrConcat(sptr, (const Char*)domain);
	*sptr++ = '"';
	*sptr = 0;
	Sync::MutexUsage mutUsage(this->dnsMut);
	while (this->dnsReq)
	{
		mutUsage.EndUse();
		Sync::Thread::Sleep(100);
		mutUsage.BeginUse();
	}
	this->dnsResult = false;
	this->dnsReq = domain;
	this->dnsResp = addr;
	this->respEvt->Clear();
	mutUsage.EndUse();
	if (!this->SendBoolCommand((const Char*)sbuff, 3000))
	{
		return false;
	}
	this->respEvt->Wait(3000);
	Bool ret;
	mutUsage.BeginUse();
	ret = this->dnsResult;
	this->dnsReq = 0;
	this->dnsResp = 0;
	mutUsage.EndUse();
	return ret;
}

Bool IO::Device::SIM7000::NetShowRemoteIP(Bool show)
{
	return this->SendBoolCommand(show?"AT+CIPSRIP=1":"AT+CIPSRIP=0");
}

Bool IO::Device::SIM7000::NetSetSendHex(Bool hexSend)
{
	return this->SendBoolCommand(hexSend?"AT+CIPSENDHEX=1":"AT+CIPSENDHEX=0");
}

Bool IO::Device::SIM7000::NetSetOutputHex(Bool hexOutput)
{
	return this->SendBoolCommand(hexOutput?"AT+CIPHEXS=2":"AT+CIPHEXS=1");
}

Bool IO::Device::SIM7000::NetSetDisableNagle(Bool disable)
{
	return this->SendBoolCommand(disable?"AT+CIPOPTION=1":"AT+CIPOPTION=0");
}

Bool IO::Device::SIM7000::NetPing(const UTF8Char *addr, Int32 *respTime, Int32 *ttl)
{
	UTF8Char sbuff[256];
	Char *sptr = (Char*)sbuff;
	sptr = Text::StrConcat(sptr, "AT+CIPPING=\"");
	sptr = Text::StrConcat(sptr, (const Char*)addr);
	sptr = Text::StrConcat(sptr, "\",1");
	if (this->SendStringCommand(sbuff, (const Char*)sbuff, 10000) == 0)
	{
		return false;
	}
	if (!Text::StrStartsWith(sbuff, (const UTF8Char*)"+CIPPING: "))
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
		*respTime = Text::StrToInt32(sarr[2]);
	}
	if (ttl)
	{
		*ttl = Text::StrToInt32(sarr[3]);
	}
	return true;
}
