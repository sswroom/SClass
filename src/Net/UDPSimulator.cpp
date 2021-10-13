#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "IO/StreamReader.h"
#include "IO/FileStream.h"
#include "Net/SocketFactory.h"
#include "Net/UDPSimulator.h"

Net::UDPSimulator::UDPSimulator(const UTF8Char *logFileName, const UTF8Char *rawFileName, UDPPacketHdlr hdlr, void *userObj)
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
	UTF8Char sbuff[2048];
	UTF8Char *sptr;
	UTF8Char *sarr[3];
	UInt8 buff[4096];
	UOSInt i;
	UOSInt j;
	UOSInt byteSize;
	Data::DateTime *dt;
	UInt32 ip;
	UInt16 port;
	IO::FileStream *logFS;
	IO::FileStream *rawFS;
	IO::StreamReader *reader;

	NEW_CLASS(dt, Data::DateTime());
	NEW_CLASS(logFS, IO::FileStream(this->logFileName, IO::FileStream::FileMode::ReadOnly, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASS(reader, IO::StreamReader(logFS, 65001));
	NEW_CLASS(rawFS, IO::FileStream(this->rawFileName, IO::FileStream::FileMode::ReadOnly, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	while ((sptr = reader->ReadLine(sbuff, 2048)) != 0)
	{
		if (Text::StrSplit(sarr, 3, sbuff, '\t') == 2)
		{
			i = Text::StrIndexOf(sarr[1], (const UTF8Char*)"Received ");
			if (i != INVALID_INDEX)
			{
				j = Text::StrIndexOf(&(sarr[1])[i + 9], (const UTF8Char*)" bytes from ");
				if (j != INVALID_INDEX && j >= 1)
				{
					sarr[1][i + 9 + j] = 0;
					byteSize = Text::StrToInt32(&sarr[1][i + 9]);
					Text::StrInt32(sarr[1], byteSize);
					sarr[2] = &sarr[1][i + j + 21];

					port = 0;
					i = Text::StrIndexOf(sarr[2], ':');
					if (i != INVALID_INDEX)
					{
						sarr[2][i] = 0;
						port = (UInt16)Text::StrToInt32(&sarr[2][i + 1]);
					}
					ip = Net::SocketUtil::GetIPAddr(sarr[2]);

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
