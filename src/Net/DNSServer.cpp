#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Net/DNSServer.h"
#include "Text/StringBuilder.h"

void __stdcall Net::DNSServer::PacketHdlr(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData)
{
	NN<Net::DNSServer> me = userData.GetNN<Net::DNSServer>();
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	Bool found;

	sptr = sbuff;
	found = false;
	i = 12;
	while (i < data.GetSize())
	{
		j = data[i];
		if (j == 0)
			break;
		i++;
		if (found)
		{
			*sptr++ = '.';
		}
		while (j-- > 0)
		{
			*sptr++ = data[i++];
		}
		found = true;
	}
	*sptr = 0;
	i++;
	if (me->reqHdlr)
	{
		me->reqHdlr(me->reqObj, CSTRP(sbuff, sptr), ReadMUInt16(&data[i]), ReadMUInt16(&data[i + 2]), addr, port, ReadMUInt16(&data[0]));
	}
}

void Net::DNSServer::InitServer(NN<Net::SocketFactory> sockf, UInt16 port, NN<IO::LogTool> log)
{
	this->sockf = sockf;
	this->reqHdlr = 0;
	this->reqObj = 0;

	NEW_CLASS(this->svr, Net::UDPServer(sockf, 0, port, CSTR_NULL, PacketHdlr, this, log, CSTR_NULL, 4, false));
	this->svr->SetBuffSize(65536);
}

Net::DNSServer::DNSServer(NN<Net::SocketFactory> sockf, UInt16 port, NN<IO::LogTool> log)
{
	InitServer(sockf, port, log);
}

Net::DNSServer::DNSServer(NN<Net::SocketFactory> sockf, NN<IO::LogTool> log)
{
	InitServer(sockf, 53, log);
}

Net::DNSServer::~DNSServer()
{
	DEL_CLASS(this->svr);
}

Bool Net::DNSServer::IsError()
{
	return this->svr->IsError();
}

void Net::DNSServer::HandleRequest(DNSRequest hdlr, AnyType userObj)
{
	this->reqHdlr = hdlr;
	this->reqObj = userObj;
}

void Net::DNSServer::ReplyRequest(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, const UInt8 *buff, UOSInt dataSize)
{
	this->svr->SendTo(addr, port, buff, dataSize);
}
