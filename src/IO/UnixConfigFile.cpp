#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/UnixConfigFile.h"

IO::ConfigFile *IO::UnixConfigFile::Parse(Text::CStringNN fileName)
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
		cfg = ParseReader(reader);
	}
	return cfg;
}

IO::ConfigFile *IO::UnixConfigFile::ParseReader(NotNullPtr<Text::UTF8Reader> reader)
{
	UTF8Char buff[1024];
	UTF8Char *name;
	UTF8Char *nameEnd;
	UTF8Char *value;
	UTF8Char *valueEnd;
	UTF8Char *src;
	UTF8Char c;
	IO::ConfigFile *cfg;
	UOSInt i;
	NEW_CLASS(cfg, IO::ConfigFile());
	while ((valueEnd = reader->ReadLine(buff, 1023)) != 0)
	{
		while (valueEnd > buff)
		{
			if (valueEnd[-1] != ' ' && valueEnd[-1] != '\t')
			{
				break;
			}
			*--valueEnd = 0;
		}
		i = 0;
		while (true)
		{
			if (buff[i] != ' ' && buff[i] != '\t')
			{
				break;
			}
			i++;
		}
		if (buff[i] != 0 && buff[i] != '#')
		{
			name = &buff[i];
			value = 0;
			src = &buff[i];
			while (*src != '=')
			{
				if (*src == 0)
					break;
				src++;
			}
			if (*src == '=')
			{
				nameEnd = src;
				*src++ = 0;
				
				if (src[0] == '\"')
				{
					src++;
					value = src;
					while ((c = *src++) != 0)
					{
						if (c == '\"')
						{
							src[-1] = 0;
							valueEnd = &src[-1];
							cfg->SetValue(CSTR_NULL, CSTRP(name, nameEnd), CSTRP(value, valueEnd));
							break;
						}
					}
				}
				else
				{
					cfg->SetValue(CSTR_NULL, CSTRP(name, nameEnd), CSTRP(src, valueEnd));
				}
			}
		}
	}
	if (cfg->GetCateCount() == 0)
	{
		DEL_CLASS(cfg);
		return 0;
	}
	return cfg;
}

