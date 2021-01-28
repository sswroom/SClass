#include "stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "IO/StreamReader.h"
#include "IO/FileStream.h"
#include "Net/SocketFactory.h"
#include "Net/UDPSimulator.h"

Net::UDPSimulator::UDPSimulator(const WChar *logFileName, const WChar *rawFileName, UDPPacketHdlr hdlr, void *userObj)
{
	this->logFileName = Text::StrCopyNew(logFileName);
	this->rawFileName = Text::StrCopyNew(rawFileName);
	this->hdlr = hdlr;
	this->userObj = userObj;
}

Net::UDPSimulator::~UDPSimulator()
{
	Text::StrDelNew(this->logFileName);
	Text::StrDelNew(this->rawFileName);
}

void Net::UDPSimulator::Run()
{
	WChar sbuff[2048];
	WChar *sptr;
	WChar *sarr[3];
	UInt8 buff[4096];
	OSInt i;
	OSInt j;
	OSInt byteSize;
	Data::DateTime *dt;
	UInt32 ip;
	UInt16 port;
	IO::FileStream *logFS;
	IO::FileStream *rawFS;
	IO::StreamReader *reader;

	NEW_CLASS(dt, Data::DateTime());
	NEW_CLASS(logFS, IO::FileStream(this->logFileName, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	NEW_CLASS(reader, IO::StreamReader(logFS, 65001));
	NEW_CLASS(rawFS, IO::FileStream(this->rawFileName, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	while ((sptr = reader->ReadLine(sbuff, 2048)) != 0)
	{
		if (Text::StrSplit(sarr, 3, sbuff, '\t') == 2)
		{
			i = Text::StrIndexOf(sarr[1], L"Received ");
			if (i >= 0)
			{
				j = Text::StrIndexOf(&(sarr[1])[i + 9], L" bytes from ");
				if (j >= 1)
				{
					sarr[1][i + 9 + j] = 0;
					byteSize = Text::StrToInt32(&sarr[1][i + 9]);
					Text::StrInt32(sarr[1], byteSize);
					sarr[2] = &sarr[1][i + j + 21];

					port = 0;
					i = Text::StrIndexOf(sarr[2], ':');
					if (i >= 0)
					{
						sarr[2][i] = 0;
						port = (UInt16)Text::StrToInt32(&sarr[2][i + 1]);
					}
					ip = Net::SocketFactory::GetIPAddr(sarr[2]);

					dt->ToLocalTime();
					dt->SetValue(sarr[0]);

					if (rawFS->Read(buff, byteSize) != byteSize)
					{
						break;
					}
					if (!this->hdlr(ip, port, buff, byteSize, this->userObj, dt))
						break;
				}
			}
		}
	}
	DEL_CLASS(rawFS);
	DEL_CLASS(reader);
	DEL_CLASS(logFS);
	DEL_CLASS(dt);
}
