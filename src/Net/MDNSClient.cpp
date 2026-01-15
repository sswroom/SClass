#include "Stdafx.h"
#include "Net/MDNSClient.h"

/*
Known service:
_services._dns-sd._udp.local
lb._dns-sd._udp.local
_airplay._tcp.local
_alexa._tcp.local
_device-info._tcp.local
_dosvc._tcp.local
_http._tcp.local
_http-alt._tcp.local
_ipp._tcp.local
_pdl-datastream._tcp.local
_printer._tcp.local
_qdiscover._tcp.local
_raop._tcp.local
_remotepairing._tcp.local
_scanner._tcp.local
_smb._tcp.local
_workstation._tcp.local
_sleep-proxy._udp.local

*/
void __stdcall Net::MDNSClient::OnUDPPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData)
{
	NN<Net::MDNSClient> me = userData.GetNN<Net::MDNSClient>();
	Data::ArrayListNN<Net::DNSClient::RequestAnswer> ans;
	if (ParseAnswers(data.Arr(), data.GetSize(), ans) > 0)
	{
		me->hdlr(me->hdlrObj, addr, ans);
		Net::DNSClient::FreeAnswers(ans);
	}
}

Net::MDNSClient::MDNSClient(NN<Net::SocketFactory> sockf, DNSAnswerHandler hdlr, AnyType hdlrObj)
{
	this->sockf = sockf;
	this->hdlr = hdlr;
	this->hdlrObj = hdlrObj;
	NEW_CLASSNN(this->udp, Net::UDPServer(sockf, nullptr, 5353, 0, OnUDPPacket, this, log, 0, 4, true));
	this->udp->AddMulticastIP(Net::SocketUtil::GetIPAddr(CSTR("224.0.0.251")));
}

Net::MDNSClient::~MDNSClient()
{
	this->udp.Delete();
}

Bool Net::MDNSClient::IsError()
{
	return this->udp->IsError();
}

Bool Net::MDNSClient::SendQuery(Text::CStringNN domain)
{
	UInt8 buff[512];
	UOSInt i;
	UOSInt j;
	WriteNUInt16(&buff[0], 0);
	WriteNUInt16(&buff[2], 0);
	WriteMUInt16(&buff[4], 1);
	WriteNUInt16(&buff[6], 0);
	WriteNUInt16(&buff[8], 0);
	WriteNUInt16(&buff[10], 0);
	i = 12;
	while (true)
	{
		j = domain.IndexOf('.');
		if (j == INVALID_INDEX)
		{
			buff[i] = (UInt8)domain.leng;
			domain.ConcatTo(&buff[i + 1]);
			i += 2 + domain.leng;
			break;
		}
		else
		{
			buff[i] = (UInt8)j;
			MemCopyNO(&buff[i + 1], domain.v.Ptr(), j);
			i += j + 1;
			domain = domain.Substring(j + 1);
		}
	}
	WriteMUInt16(&buff[i], 12);
	WriteMUInt16(&buff[i + 2], 1);
	i += 4;
	Net::SocketUtil::AddressInfo addr;
	Net::SocketUtil::SetAddrInfo(addr, CSTR("224.0.0.251"));
	return this->udp->SendTo(addr, 5353, buff, i);
}

UOSInt Net::MDNSClient::ParseAnswers(UnsafeArray<const UInt8> buff, UOSInt dataSize, NN<Data::ArrayListNN<Net::DNSClient::RequestAnswer>> answers)
{
	NN<Net::DNSClient::RequestAnswer> ans;
	UOSInt ansCount = ReadMUInt16(&buff[6]);
	UOSInt cnt2 = ReadMUInt16(&buff[8]);
	UOSInt cnt3 = ReadMUInt16(&buff[10]);
	ansCount += cnt2 + cnt3;
	UOSInt i;
	UOSInt j;
	i = 12;

	j = 0;
	while (j < ansCount && i < dataSize)
	{
		ans = Net::DNSClient::ParseAnswer(buff, dataSize, i);
		answers->Add(ans);

		j++;
	}
	return ansCount;
}

