#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/JavaProperties.h"
#include "IO/Path.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

IO::ConfigFile *IO::JavaProperties::ParseAppProp()
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("application.properties"));
	return Parse(CSTRP(sbuff, sptr));
}

IO::ConfigFile *IO::JavaProperties::Parse(Text::CString fileName)
{
	IO::ConfigFile *cfg;
	IO::FileStream fstm(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential);
	if (fstm.IsError())
	{
		cfg = 0;
	}
	else
	{
		Text::UTF8Reader reader(fstm);
		cfg = ParseReader(&reader);
	}
	return cfg;
}

IO::ConfigFile *IO::JavaProperties::ParseReader(Text::UTF8Reader *reader)
{
	UTF8Char buff[1024];
	UTF8Char *name;
	UTF8Char *nameEnd;
	UTF8Char *value;
	UTF8Char *valueEnd;
	IO::ConfigFile *cfg;
	UOSInt i;
	NEW_CLASS(cfg, IO::ConfigFile());
	while ((valueEnd = reader->ReadLine(buff, 1023)) != 0)
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
			cfg->SetValue(0, CSTRP(name, nameEnd), CSTRP(value, valueEnd));
		}
	}
	return cfg;
}

UTF8Char *IO::JavaProperties::EscapeStr(UTF8Char *str, UTF8Char *strEnd)
{
	UTF8Char *dest = str;
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
