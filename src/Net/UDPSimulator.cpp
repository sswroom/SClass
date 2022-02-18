#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "IO/StreamReader.h"
#include "IO/FileStream.h"
#include "Net/SocketFactory.h"
#include "Net/UDPSimulator.h"

Net::UDPSimulator::UDPSimulator(Text::CString logFileName, Text::CString rawFileName, UDPPacketHdlr hdlr, void *userObj)
{
	this->logFileName = Text::String::New(logFileName);
	this->rawFileName = Text::String::New(rawFileName);
	this->hdlr = hdlr;
	this->userObj = userObj;
}

Net::UDPSimulator::~UDPSimulator()
{
	this->logFileName->Release();
	this->rawFileName->Release();
}

void Net::UDPSimulator::Run()
{
	UTF8Char sbuff[2048];
	UTF8Char *sptr;
	Text::PString sarr[3];
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
	NEW_CLASS(logFS, IO::FileStream(this->logFileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASS(reader, IO::StreamReader(logFS, 65001));
	NEW_CLASS(rawFS, IO::FileStream(this->rawFileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	while ((sptr = reader->ReadLine(sbuff, 2048)) != 0)
	{
		if (Text::StrSplitP(sarr, 3, {sbuff, (UOSInt)(sptr - sbuff)}, '\t') == 2)
		{
			i = sarr[1].IndexOf(UTF8STRC("Received "));
			if (i != INVALID_INDEX)
			{
				j = Text::StrIndexOfC(&sarr[1].v[i + 9], sarr[1].leng - i - 9, UTF8STRC(" bytes from "));
				if (j != INVALID_INDEX && j >= 1)
				{
					sarr[1].v[i + 9 + j] = 0;
					sarr[2].leng = sarr[1].leng;
					sarr[1].leng = i + 9 + j;
					byteSize = Text::StrToInt32(&sarr[1].v[i + 9]);
					Text::StrInt32(sarr[1].v, byteSize);
					sarr[2].v = &sarr[1].v[i + j + 21];
					sarr[2].leng -= i + j + 21;

					port = 0;
					i = Text::StrIndexOfCharC(sarr[2].v, sarr[2].leng, ':');
					if (i != INVALID_INDEX)
					{
						sarr[2].v[i] = 0;
						sarr[2].leng = i;
						port = (UInt16)Text::StrToInt32(&sarr[2].v[i + 1]);
					}
					ip = Net::SocketUtil::GetIPAddr(sarr[2].v, sarr[2].leng);

					dt->ToLocalTime();
					dt->SetValue(sarr[0].v, sarr[0].leng);

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
