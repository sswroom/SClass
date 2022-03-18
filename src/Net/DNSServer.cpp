#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Net/DNSServer.h"
#include "Text/StringBuilder.h"

void __stdcall Net::DNSServer::PacketHdlr(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData)
{
	Net::DNSServer *me = (Net::DNSServer*)userData;
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

void Net::DNSServer::InitServer(Net::SocketFactory *sockf, UInt16 port)
{
	this->sockf = sockf;
	this->reqHdlr = 0;
	this->reqObj = 0;

	NEW_CLASS(this->svr, Net::UDPServer(sockf, 0, port, CSTR_NULL, PacketHdlr, this, 0, CSTR_NULL, 4, false));
	this->svr->SetBuffSize(65536);
}

Net::DNSServer::DNSServer(Net::SocketFactory *sockf, UInt16 port)
{
	InitServer(sockf, port);
}

Net::DNSServer::DNSServer(Net::SocketFactory *sockf)
{
	InitServer(sockf, 53);
}

Net::DNSServer::~DNSServer()
{
	DEL_CLASS(this->svr);
}

Bool Net::DNSServer::IsError()
{
	return this->svr->IsError();
}

void Net::DNSServer::HandleRequest(DNSRequest hdlr, void *userObj)
{
	this->reqHdlr = hdlr;
	this->reqObj = userObj;
}

void Net::DNSServer::ReplyRequest(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize)
{
	this->svr->SendTo(addr, port, buff, dataSize);
}
