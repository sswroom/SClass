#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Net/DNSServer.h"
#include "Text/StringBuilder.h"

void __stdcall Net::DNSServer::PacketHdlr(NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, AnyType userData)
{
	NotNullPtr<Net::DNSServer> me = userData.GetNN<Net::DNSServer>();
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	Bool found;

	sptr = sbuff;
	found = false;
	i = 12;
	while (i < dataSize)
	{
		j = buff[i];
		if (j == 0)
			break;
		i++;
		if (found)
		{
			*sptr++ = '.';
		}
		while (j-- > 0)
		{
			*sptr++ = buff[i++];
		}
		found = true;
	}
	*sptr = 0;
	i++;
	if (me->reqHdlr)
	{
		me->reqHdlr(me->reqObj, CSTRP(sbuff, sptr), ReadMUInt16(&buff[i]), ReadMUInt16(&buff[i + 2]), addr, port, ReadMUInt16(buff));
	}
}

void Net::DNSServer::InitServer(NotNullPtr<Net::SocketFactory> sockf, UInt16 port, NotNullPtr<IO::LogTool> log)
{
	this->sockf = sockf;
	this->reqHdlr = 0;
	this->reqObj = 0;

	NEW_CLASS(this->svr, Net::UDPServer(sockf, 0, port, CSTR_NULL, PacketHdlr, this, log, CSTR_NULL, 4, false));
	this->svr->SetBuffSize(65536);
}

Net::DNSServer::DNSServer(NotNullPtr<Net::SocketFactory> sockf, UInt16 port, NotNullPtr<IO::LogTool> log)
{
	InitServer(sockf, port, log);
}

Net::DNSServer::DNSServer(NotNullPtr<Net::SocketFactory> sockf, NotNullPtr<IO::LogTool> log)
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

void Net::DNSServer::ReplyRequest(NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt16 port, const UInt8 *buff, UOSInt dataSize)
{
	this->svr->SendTo(addr, port, buff, dataSize);
}
