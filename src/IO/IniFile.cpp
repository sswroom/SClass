#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "IO/FileStream.h"
#include "IO/IniFile.h"
#include "IO/Path.h"
#include "IO/StreamWriter.h"
#include "Sync/Event.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

IO::ConfigFile *IO::IniFile::Parse(NotNullPtr<IO::Stream> stm, UInt32 codePage)
{
	IO::ConfigFile *cfg;
	IO::StreamReader reader(stm, codePage);
	cfg = ParseReader(&reader);
	return cfg;
}

IO::ConfigFile *IO::IniFile::Parse(Text::CString fileName, UInt32 codePage)
{
	IO::ConfigFile *cfg;
	IO::FileStream fstm(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential);
	IO::StreamReader reader(fstm, codePage);
	cfg = ParseReader(&reader);
	return cfg;
}

IO::ConfigFile *IO::IniFile::ParseProgConfig(UInt32 codePage)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::ReplaceExt(sbuff, UTF8STRC("cfg"));
	return Parse(CSTRP(sbuff, sptr), codePage);
}

IO::ConfigFile *IO::IniFile::ParseReader(IO::StreamReader *reader)
{
	UTF8Char cate[128];
	UTF8Char *cateEnd;
	UTF8Char buff[1024];
	UTF8Char *name;
	UTF8Char *nameEnd;
	UTF8Char *value;
	UTF8Char *valueEnd;
	UTF8Char *src;
	UTF8Char lbrk[3];
	IO::ConfigFile *cfg;
	NEW_CLASS(cfg, IO::ConfigFile());
	cate[0] = 0;
	cateEnd = cate;
	while ((valueEnd = reader->ReadLine(buff, 1023)) != 0)
	{
		if (buff[0] == '[' && valueEnd[-1] == ']')
		{
			if (valueEnd - buff < 128)
			{
				cateEnd = Text::StrConcatC(cate, &buff[1], (UOSInt)(valueEnd - buff - 2));
			}
		}
		else
		{
			name = buff;
			value = 0;
			src = buff;
			while (*src != '=')
			{
				if (*src == 0)
					break;
				src++;
			}
			if (*src == '=')
			{
				*src = 0;
				nameEnd = Text::StrTrimC(name, (UOSInt)(src - name));
				src++;
				value = src;
				if (reader->GetLastLineBreak(lbrk) == lbrk)
				{
					Text::StringBuilderUTF8 sb;
					sb.AppendC(value, (UOSInt)(valueEnd - value));
					while ((valueEnd = reader->ReadLine(src, 1023 - (UOSInt)(src - buff))) != 0)
					{
						sb.AppendC(src, (UOSInt)(valueEnd - src));
						if (reader->GetLastLineBreak(lbrk) != lbrk)
							break;
					}
					sb.Trim();
					cfg->SetValue(CSTRP(cate, cateEnd), CSTRP(name, nameEnd), sb.ToCString());
				}
				else
				{
					valueEnd = Text::StrTrim(value);
					cfg->SetValue(CSTRP(cate, cateEnd), CSTRP(name, nameEnd), CSTRP(value, valueEnd));
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

Bool IO::IniFile::SaveConfig(NotNullPtr<IO::Stream> stm, UInt32 codePage, IO::ConfigFile *cfg)
{
	Bool ret;
	IO::StreamWriter writer(stm, codePage);
	ret = SaveConfig(&writer, cfg);
	return ret;
}

Bool IO::IniFile::SaveConfig(IO::Writer *writer, IO::ConfigFile *cfg)
{
	Data::ArrayListNN<Text::String> cateList;
	Data::ArrayListNN<Text::String> keyList;
	NotNullPtr<Text::String> s;
	Text::String *s2;
	Text::String *s3;
	UOSInt i;
	UOSInt j;
	cfg->GetKeys((Text::String*)0, &keyList);
	Data::ArrayIterator<NotNullPtr<Text::String>> it = keyList.Iterator();
	while (it.HasNext())
	{
		s = it.Next();
		writer->WriteStrC(s->v, s->leng);
		writer->WriteStrC(UTF8STRC("="));
		s2 = cfg->GetValue(s);
		if (s2)
			writer->WriteLineC(s2->v, s2->leng);
		else
			writer->WriteLine();
	}
	cfg->GetCateList(&cateList, false);
	i = 0;
	j = cateList.GetCount();
	while (i < j)
	{
		s2 = cateList.GetItem(i);
		writer->WriteLine();
		writer->WriteStrC(UTF8STRC("["));
		writer->WriteStrC(s2->v, s2->leng);
		writer->WriteLineC(UTF8STRC("]"));

		keyList.Clear();
		cfg->GetKeys(s2, &keyList);
		it = keyList.Iterator();
		while (it.HasNext())
		{
			s = it.Next();
			writer->WriteStrC(s->v, s->leng);
			writer->WriteStrC(UTF8STRC("="));
			s3 = cfg->GetCateValue(s2, s);
			if (s3)
				writer->WriteLineC(s3->v, s3->leng);
			else
				writer->WriteLine();
		}
		i++;
	}
	return true;
}
