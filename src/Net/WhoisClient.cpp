#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/TCPClient.h"
#include "Net/WhoisClient.h"
#include "Text/MyString.h"
#include "Text/UTF8Reader.h"

Net::WhoisClient::WhoisClient(NN<Net::SocketFactory> sockf, UInt32 whoisIP, Text::CString prefix)
{
	this->sockf = sockf;
	this->whoisIP = whoisIP;
	this->prefix = Text::String::NewOrNull(prefix);
}

Net::WhoisClient::~WhoisClient()
{
	OPTSTR_DEL(this->prefix);
}

NN<Net::WhoisRecord> Net::WhoisClient::RequestIP(UInt32 ip, Data::Duration timeout)
{
	return RequestIP(ip, this->whoisIP, OPTSTR_CSTR(this->prefix), timeout);
}

NN<Net::WhoisRecord> Net::WhoisClient::RequestIP(UInt32 ip, UInt32 whoisIP, Text::CString prefix, Data::Duration timeout)
{
	NN<Net::WhoisRecord> rec;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;

	UInt8 *ipAddr = (UInt8*)&ip;
	UOSInt i;
	Text::CStringNN nnprefix;
	if (prefix.SetTo(nnprefix))
	{
		sptr = nnprefix.ConcatTo(sbuff);
	}
	else
	{
		sptr = sbuff;
	}
	sptr = Text::StrConcatC(Text::StrUInt32(sptr, ipAddr[0]), UTF8STRC("."));
	sptr = Text::StrConcatC(Text::StrUInt32(sptr, ipAddr[1]), UTF8STRC("."));
	sptr = Text::StrConcatC(Text::StrUInt32(sptr, ipAddr[2]), UTF8STRC("."));
	sptr = Text::StrConcatC(Text::StrUInt32(sptr, ipAddr[3]), UTF8STRC("\r\n"));

	NEW_CLASSNN(rec, Net::WhoisRecord(ip));
	Net::TCPClient cli(sockf, whoisIP, 43, timeout);
	cli.Write(Data::ByteArrayR((UInt8*)sbuff, (UOSInt)(sptr - sbuff)));
	Text::UTF8Reader reader(cli);
	while (reader.ReadLine(sbuff, 511).SetTo(sptr))
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
	return rec;
}
