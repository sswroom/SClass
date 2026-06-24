#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "IO/StreamReader.h"
#include "IO/FileStream.h"
#include "Net/SocketFactory.h"
#include "Net/UDPSimulator.h"

Net::UDPSimulator::UDPSimulator(Text::CStringNN logFileName, Text::CStringNN rawFileName, UDPPacketHdlr hdlr, AnyType userObj)
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
	UnsafeArray<UTF8Char> sptr;
	Text::PString sarr[3];
	UInt8 buff[4096];
	UIntOS i;
	UIntOS j;
	UIntOS byteSize;
	NN<Data::DateTime> dt;
	UInt32 ip;
	UInt16 port;
	NN<IO::FileStream> logFS;
	NN<IO::FileStream> rawFS;
	NN<IO::StreamReader> reader;

	NEW_CLASSNN(dt, Data::DateTime());
	NEW_CLASSNN(logFS, IO::FileStream(this->logFileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASSNN(reader, IO::StreamReader(logFS, 65001));
	NEW_CLASSNN(rawFS, IO::FileStream(this->rawFileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	while (reader->ReadLine(sbuff, 2048).SetTo(sptr))
	{
		if (Text::StrSplitP(sarr, 3, {sbuff, (UIntOS)(sptr - sbuff)}, '\t') == 2)
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
					ip = Net::SocketUtil::GetIPAddr(sarr[2].ToCString());

					dt->ToLocalTime();
					dt->SetValue(sarr[0].ToCString());

					if (rawFS->Read(Data::ByteArray(buff, byteSize)) != byteSize)
					{
						break;
					}
					if (!this->hdlr(ip, port, buff, byteSize, this->userObj, dt))
						break;
				}
			}
		}
	}
	rawFS.Delete();
	reader.Delete();
	logFS.Delete();
	dt.Delete();
}
