#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/GPIOPin.h"
#include "IO/Path.h"
#include "IO/StreamReader.h"
#include "Text/MyString.h"

Bool GPIOPin_EchoFile(const UTF8Char *fileName, const Char *msg)
{
	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileMode::CreateWrite, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		return false;
	}
	UOSInt fileSize = Text::StrCharCnt(msg);
	fs->Write((const UInt8*)msg, fileSize);
	DEL_CLASS(fs);
	return true;
}

IO::GPIOPin::GPIOPin(UOSInt pinNum)
{
	this->pinNum = pinNum;

	UTF8Char sbuff[128];
	UTF8Char *sptr = Text::StrConcatC(sbuff, UTF8STRC("/sys/class/gpio/gpio"));
	sptr = Text::StrInt32(sptr, this->pinNum);
	if (IO::Path::GetPathType(sbuff) != IO::Path::PathType::Directory)
	{
		Char sbuff2[12];
		Text::StrInt32(sbuff2, this->pinNum);
		GPIOPin_EchoFile((const UTF8Char*)"/sys/class/gpio/export", sbuff2);
	}
}

IO::GPIOPin::~GPIOPin()
{
}

Bool IO::GPIOPin::IsError()
{
	UTF8Char sbuff[128];
	UTF8Char *sptr = Text::StrConcatC(sbuff, UTF8STRC("/sys/class/gpio/gpio"));
	sptr = Text::StrInt32(sptr, this->pinNum);
	return IO::Path::GetPathType(sbuff) != IO::Path::PathType::Directory;
}

Bool IO::GPIOPin::IsPinHigh()
{
	UTF8Char sbuff[128];
	UTF8Char *sptr = Text::StrConcatC(sbuff, UTF8STRC("/sys/class/gpio/gpio"));
	sptr = Text::StrInt32(sptr, this->pinNum);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/value"));

	Bool isHigh = false;
	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (!fs->IsError())
	{
		IO::StreamReader *reader;
		NEW_CLASS(reader, IO::StreamReader(fs, 65001));
		if (reader->ReadLine(sbuff, 120))
		{
			isHigh = Text::StrToInt32(sbuff) != 0;
		}
		DEL_CLASS(reader);
	}
	DEL_CLASS(fs);

	return isHigh;
}

Bool IO::GPIOPin::IsPinOutput()
{
	UTF8Char sbuff[128];
	UTF8Char *sptr = Text::StrConcatC(sbuff, UTF8STRC("/sys/class/gpio/gpio"));
	sptr = Text::StrInt32(sptr, this->pinNum);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/direction"));

	Bool isOutput = false;
	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (!fs->IsError())
	{
		IO::StreamReader *reader;
		NEW_CLASS(reader, IO::StreamReader(fs, 65001));
		if (reader->ReadLine(sbuff, 120))
		{
			isOutput = Text::StrEquals(sbuff, (const UTF8Char*)"out");
		}
		DEL_CLASS(reader);
	}
	DEL_CLASS(fs);

	return isOutput;
}

void IO::GPIOPin::SetPinOutput(Bool isOutput)
{
	UTF8Char sbuff[128];
	UTF8Char *sptr = Text::StrConcatC(sbuff, UTF8STRC("/sys/class/gpio/gpio"));
	sptr = Text::StrInt32(sptr, this->pinNum);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/direction"));
	GPIOPin_EchoFile(sbuff, isOutput?"out":"in");
}

void IO::GPIOPin::SetPinState(Bool isHigh)
{
	UTF8Char sbuff[128];
	UTF8Char *sptr = Text::StrConcatC(sbuff, UTF8STRC("/sys/class/gpio/gpio"));
	sptr = Text::StrInt32(sptr, this->pinNum);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/value"));
	GPIOPin_EchoFile(sbuff, isHigh?"1":"0");
}

Bool IO::GPIOPin::SetPullType(PullType pt)
{
	return false;
}

UTF8Char *IO::GPIOPin::GetName(UTF8Char *buff)
{
	return Text::StrInt32(Text::StrConcatC(buff, UTF8STRC("GPIO")), this->pinNum);
}

void IO::GPIOPin::SetEventOnHigh(Bool enable)
{
}

void IO::GPIOPin::SetEventOnLow(Bool enable)
{
}

void IO::GPIOPin::SetEventOnRaise(Bool enable)
{
}

void IO::GPIOPin::SetEventOnFall(Bool enable)
{
}

Bool IO::GPIOPin::HasEvent()
{
	return false;
}

void IO::GPIOPin::ClearEvent()
{
}
