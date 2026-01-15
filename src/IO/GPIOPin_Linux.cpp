#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/GPIOPin.h"
#include "IO/Path.h"
#include "IO/StreamReader.h"
#include "Text/MyString.h"
#include "Text/UTF8Reader.h"

Bool GPIOPin_EchoFile(Text::CString fileName, Text::CString msg)
{
	IO::FileStream fs(fileName, IO::FileMode::CreateWrite, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
	{
		return false;
	}
	fs.Write(msg.v, msg.leng);
	return true;
}

IO::GPIOPin::GPIOPin(IO::GPIOControl *gpio, UInt16 pinNum)
{
	this->pinNum = pinNum;

	UTF8Char sbuff[128];
	UTF8Char *sptr = Text::StrConcatC(sbuff, UTF8STRC("/sys/class/gpio/gpio"));
	sptr = Text::StrInt32(sptr, this->pinNum);
	if (IO::Path::GetPathType(CSTRP(sbuff, sptr)) != IO::Path::PathType::Directory)
	{
		UTF8Char sbuff2[12];
		UTF8Char *sptr2;
		sptr2 = Text::StrInt32(sbuff2, this->pinNum);
		GPIOPin_EchoFile(CSTR("/sys/class/gpio/export"), CSTRP(sbuff2, sptr2));
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
	return IO::Path::GetPathType(CSTRP(sbuff, sptr)) != IO::Path::PathType::Directory;
}

Bool IO::GPIOPin::IsPinHigh()
{
	UTF8Char sbuff[128];
	UTF8Char *sptr = Text::StrConcatC(sbuff, UTF8STRC("/sys/class/gpio/gpio"));
	sptr = Text::StrInt32(sptr, this->pinNum);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/value"));

	Bool isHigh = false;
	IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (!fs.IsError())
	{
		Text::UTF8Reader reader(&fs);
		if (reader.ReadLine(sbuff, 120))
		{
			isHigh = Text::StrToInt32(sbuff) != 0;
		}
	}

	return isHigh;
}

Bool IO::GPIOPin::IsPinOutput()
{
	UTF8Char sbuff[128];
	UTF8Char *sptr = Text::StrConcatC(sbuff, UTF8STRC("/sys/class/gpio/gpio"));
	sptr = Text::StrInt32(sptr, this->pinNum);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/direction"));

	Bool isOutput = false;
	IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (!fs.IsError())
	{
		Text::UTF8Reader reader(&fs);
		if ((sptr = reader.ReadLine(sbuff, 120)) != 0)
		{
			isOutput = Text::StrEqualsC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("out"));
		}
	}
	return isOutput;
}

void IO::GPIOPin::SetPinOutput(Bool isOutput)
{
	UTF8Char sbuff[128];
	UTF8Char *sptr = Text::StrConcatC(sbuff, UTF8STRC("/sys/class/gpio/gpio"));
	sptr = Text::StrInt32(sptr, this->pinNum);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/direction"));
	GPIOPin_EchoFile(CSTRP(sbuff, sptr), isOutput?CSTR("out"):CSTR("in"));
}

void IO::GPIOPin::SetPinState(Bool isHigh)
{
	UTF8Char sbuff[128];
	UTF8Char *sptr = Text::StrConcatC(sbuff, UTF8STRC("/sys/class/gpio/gpio"));
	sptr = Text::StrInt32(sptr, this->pinNum);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/value"));
	GPIOPin_EchoFile(CSTRP(sbuff, sptr), isHigh?CSTR("1"):CSTR("0"));
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
