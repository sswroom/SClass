#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/JavaProperties.h"
#include "IO/Path.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

Optional<IO::ConfigFile> IO::JavaProperties::ParseAppProp()
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("application.properties"));
	return Parse(CSTRP(sbuff, sptr));
}

Optional<IO::ConfigFile> IO::JavaProperties::Parse(Text::CStringNN fileName)
{
	Optional<IO::ConfigFile> cfg;
	IO::FileStream fstm(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential);
	if (fstm.IsError())
	{
		cfg = nullptr;
	}
	else
	{
		Text::UTF8Reader reader(fstm);
		cfg = ParseReader(reader);
	}
	return cfg;
}

Optional<IO::ConfigFile> IO::JavaProperties::ParseReader(NN<Text::UTF8Reader> reader)
{
	UTF8Char buff[1024];
	UnsafeArray<UTF8Char> name;
	UnsafeArray<UTF8Char> nameEnd;
	UnsafeArray<UTF8Char> value;
	UnsafeArray<UTF8Char> valueEnd;
	IO::ConfigFile *cfg;
	UOSInt i;
	NEW_CLASS(cfg, IO::ConfigFile());
	while (reader->ReadLine(buff, 1023).SetTo(valueEnd))
	{
		i = 0;
		while (buff[i] == ' ' || buff[i] == '\t')
		{
			i++;
		}
		name = &buff[i];
		i = Text::StrIndexOfCharC(name, (UOSInt)(valueEnd - name), '=');
		if (i == INVALID_INDEX)
		{
			i = Text::StrIndexOfCharC(name, (UOSInt)(valueEnd - name), ':');
		}
		if (i != INVALID_INDEX)
		{
			value = &name[i + 1];
			nameEnd = &name[i];
			while (nameEnd[-1] == ' ' || nameEnd[-1] == '\t')
			{
				nameEnd--;
			}
			while (value < valueEnd && (*value == ' ' || *value == '\t'))
			{
				value++;
			}
			nameEnd = EscapeStr(name, nameEnd);
			valueEnd = EscapeStr(value, valueEnd);
			cfg->SetValue(CSTR(""), CSTRP(name, nameEnd), CSTRP(value, valueEnd));
		}
	}
	return cfg;
}

UnsafeArray<UTF8Char> IO::JavaProperties::EscapeStr(UnsafeArray<UTF8Char> str, UnsafeArray<UTF8Char> strEnd)
{
	UnsafeArray<UTF8Char> dest = str;
	UTF8Char c;
	while (str < strEnd)
	{
		c = *str++;
		if (c == '\\')
		{
			if (str >= strEnd)
			{
				*dest++ = c;
			}
			else if (*str == '\\')
			{
				*dest++ = c;
				str++;
			}
			else if (*str == 'u' && str + 5 <= strEnd)
			{
				UTF16Char v =  Text::StrHex2UInt16C(str + 1);
				dest = Text::StrWriteChar(dest, v);
				str += 5;
			}
			else
			{
				*dest++ = c;
			}
		}
		else
		{
			*dest++ = c;
		}
	}
	*dest = 0;
	return dest;
}
