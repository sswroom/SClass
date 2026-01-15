#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/UnixConfigFile.h"

Optional<IO::ConfigFile> IO::UnixConfigFile::Parse(Text::CStringNN fileName)
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

Optional<IO::ConfigFile> IO::UnixConfigFile::ParseReader(NN<Text::UTF8Reader> reader)
{
	UTF8Char buff[1024];
	UnsafeArray<UTF8Char> name;
	UnsafeArray<UTF8Char> nameEnd;
	UnsafeArray<UTF8Char> value;
	UnsafeArray<UTF8Char> valueEnd;
	UnsafeArray<UTF8Char> src;
	UTF8Char c;
	IO::ConfigFile *cfg;
	UIntOS i;
	NEW_CLASS(cfg, IO::ConfigFile());
	while (reader->ReadLine(buff, 1023).SetTo(valueEnd))
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
							cfg->SetValue(CSTR(""), CSTRP(name, nameEnd), CSTRP(value, valueEnd));
							break;
						}
					}
				}
				else
				{
					cfg->SetValue(CSTR(""), CSTRP(name, nameEnd), CSTRP(src, valueEnd));
				}
			}
		}
	}
	if (cfg->GetCateCount() == 0)
	{
		DEL_CLASS(cfg);
		return nullptr;
	}
	return cfg;
}

