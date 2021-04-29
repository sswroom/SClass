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

	UInt8 *ipAddr = (UInt8*)&ip;
	Char *sptr;
	Char cbuff[32];
	OSInt i;
	if (prefix)
	{
		sptr = Text::StrConcat(cbuff, prefix);
	}
	else
	{
		sptr = cbuff;
	}
	sptr = Text::StrConcat(Text::StrInt32(sptr, ipAddr[0]), ".");
	sptr = Text::StrConcat(Text::StrInt32(sptr, ipAddr[1]), ".");
	sptr = Text::StrConcat(Text::StrInt32(sptr, ipAddr[2]), ".");
	sptr = Text::StrConcat(Text::StrInt32(sptr, ipAddr[3]), "\r\n");

	NEW_CLASS(rec, Net::WhoisRecord(ip));
	NEW_CLASS(cli, Net::TCPClient(sockf, whoisIP, 43));
	cli->Write((UInt8*)cbuff, (UOSInt)(sptr - cbuff));
	NEW_CLASS(reader, Text::UTF8Reader(cli));
	while (reader->ReadLine(sbuff, 511))
	{
		if (Text::StrStartsWith(sbuff, (const UTF8Char*)"%"))
		{

		}
		else
		{
			i = Text::StrIndexOf(sbuff, '#');
			if (i >= 0)
			{
				sbuff[i] = 0;
			}
			rec->AddItem(sbuff);
		}
	}
	DEL_CLASS(reader);
	DEL_CLASS(cli);
	return rec;
}
