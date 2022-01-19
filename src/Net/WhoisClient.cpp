#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/TCPClient.h"
#include "Net/WhoisClient.h"
#include "Text/MyString.h"
#include "Text/UTF8Reader.h"

Net::WhoisClient::WhoisClient(Net::SocketFactory *sockf, UInt32 whoisIP, const Char *prefix)
{
	this->sockf = sockf;
	this->whoisIP = whoisIP;
	if (prefix)
	{
		this->prefix = Text::StrCopyNew(prefix);
	}
	else
	{
		this->prefix = 0;
	}
}

Net::WhoisClient::~WhoisClient()
{
	SDEL_TEXT(this->prefix);
}

Net::WhoisRecord *Net::WhoisClient::RequestIP(UInt32 ip)
{
	return RequestIP(ip, this->whoisIP, this->prefix);
}

Net::WhoisRecord *Net::WhoisClient::RequestIP(UInt32 ip, UInt32 whoisIP, const Char *prefix)
{
	Net::TCPClient *cli;
	Text::UTF8Reader *reader;
	Net::WhoisRecord *rec;
	UTF8Char sbuff[512];
	UTF8Char *sptr;

	UInt8 *ipAddr = (UInt8*)&ip;
	Char *cptr;
	Char cbuff[32];
	UOSInt i;
	if (prefix)
	{
		cptr = Text::StrConcat(cbuff, prefix);
	}
	else
	{
		cptr = cbuff;
	}
	cptr = Text::StrConcat(Text::StrInt32(cptr, ipAddr[0]), ".");
	cptr = Text::StrConcat(Text::StrInt32(cptr, ipAddr[1]), ".");
	cptr = Text::StrConcat(Text::StrInt32(cptr, ipAddr[2]), ".");
	cptr = Text::StrConcat(Text::StrInt32(cptr, ipAddr[3]), "\r\n");

	NEW_CLASS(rec, Net::WhoisRecord(ip));
	NEW_CLASS(cli, Net::TCPClient(sockf, whoisIP, 43));
	cli->Write((UInt8*)cbuff, (UOSInt)(cptr - cbuff));
	NEW_CLASS(reader, Text::UTF8Reader(cli));
	while ((sptr = reader->ReadLine(sbuff, 511)) != 0)
	{
		if (sbuff[0] == '%')
		{

		}
		else
		{
			i = Text::StrIndexOf(sbuff, '#');
			if (i != INVALID_INDEX)
			{
				sbuff[i] = 0;
				sptr = &sbuff[i];
			}
			rec->AddItem(sbuff, (UOSInt)(sptr - sbuff));
		}
	}
	DEL_CLASS(reader);
	DEL_CLASS(cli);
	return rec;
}
