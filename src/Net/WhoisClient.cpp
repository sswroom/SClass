#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/TCPClient.h"
#include "Net/WhoisClient.h"
#include "Text/MyString.h"
#include "Text/UTF8Reader.h"

Net::WhoisClient::WhoisClient(Net::SocketFactory *sockf, UInt32 whoisIP, Text::CString prefix)
{
	this->sockf = sockf;
	this->whoisIP = whoisIP;
	if (prefix.leng > 0)
	{
		this->prefix = Text::String::New(prefix);
	}
	else
	{
		this->prefix = 0;
	}
}

Net::WhoisClient::~WhoisClient()
{
	SDEL_STRING(this->prefix);
}

Net::WhoisRecord *Net::WhoisClient::RequestIP(UInt32 ip, Data::Duration timeout)
{
	return RequestIP(ip, this->whoisIP, STR_CSTR(this->prefix), timeout);
}

Net::WhoisRecord *Net::WhoisClient::RequestIP(UInt32 ip, UInt32 whoisIP, Text::CString prefix, Data::Duration timeout)
{
	Net::TCPClient *cli;
	Text::UTF8Reader *reader;
	Net::WhoisRecord *rec;
	UTF8Char sbuff[512];
	UTF8Char *sptr;

	UInt8 *ipAddr = (UInt8*)&ip;
	UOSInt i;
	if (prefix.v)
	{
		sptr = prefix.ConcatTo(sbuff);
	}
	else
	{
		sptr = sbuff;
	}
	sptr = Text::StrConcatC(Text::StrUInt32(sptr, ipAddr[0]), UTF8STRC("."));
	sptr = Text::StrConcatC(Text::StrUInt32(sptr, ipAddr[1]), UTF8STRC("."));
	sptr = Text::StrConcatC(Text::StrUInt32(sptr, ipAddr[2]), UTF8STRC("."));
	sptr = Text::StrConcatC(Text::StrUInt32(sptr, ipAddr[3]), UTF8STRC("\r\n"));

	NEW_CLASS(rec, Net::WhoisRecord(ip));
	NEW_CLASS(cli, Net::TCPClient(sockf, whoisIP, 43, timeout));
	cli->Write((UInt8*)sbuff, (UOSInt)(sptr - sbuff));
	NEW_CLASS(reader, Text::UTF8Reader(cli));
	while ((sptr = reader->ReadLine(sbuff, 511)) != 0)
	{
		if (sbuff[0] == '%')
		{

		}
		else
		{
			i = Text::StrIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '#');
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
