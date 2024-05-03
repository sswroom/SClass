#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListStringNN.h"
#include "IO/FileStream.h"
#include "IO/IniFile.h"
#include "IO/Path.h"
#include "IO/StreamWriter.h"
#include "Sync/Event.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

IO::ConfigFile *IO::IniFile::Parse(NN<IO::Stream> stm, UInt32 codePage)
{
	IO::ConfigFile *cfg;
	IO::StreamReader reader(stm, codePage);
	cfg = ParseReader(&reader);
	return cfg;
}

IO::ConfigFile *IO::IniFile::Parse(Text::CStringNN fileName, UInt32 codePage)
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

Bool IO::IniFile::SaveConfig(NN<IO::Stream> stm, UInt32 codePage, IO::ConfigFile *cfg)
{
	Bool ret;
	IO::StreamWriter writer(stm, codePage);
	ret = SaveConfig(&writer, cfg);
	return ret;
}

Bool IO::IniFile::SaveConfig(IO::Writer *writer, IO::ConfigFile *cfg)
{
	Data::ArrayListStringNN cateList;
	Data::ArrayListStringNN keyList;
	NN<Text::String> s;
	NN<Text::String> s2;
	NN<Text::String> s3;
	cfg->GetKeys(CSTR(""), keyList);
	Data::ArrayIterator<NN<Text::String>> itCate;
	Data::ArrayIterator<NN<Text::String>> it = keyList.Iterator();
	while (it.HasNext())
	{
		s = it.Next();
		writer->Write(s->ToCString());
		writer->Write(CSTR("="));
		if (cfg->GetValue(s).SetTo(s2))
			writer->WriteLine(s2->ToCString());
		else
			writer->WriteLine();
	}
	cfg->GetCateList(cateList, false);
	itCate = cateList.Iterator();
	while (itCate.HasNext())
	{
		s2 = itCate.Next();
		writer->WriteLine();
		writer->Write(CSTR("["));
		writer->Write(s2->ToCString());
		writer->WriteLine(CSTR("]"));

		keyList.Clear();
		cfg->GetKeys(s2, keyList);
		it = keyList.Iterator();
		while (it.HasNext())
		{
			s = it.Next();
			writer->Write(s->ToCString());
			writer->Write(CSTR("="));
			if (cfg->GetCateValue(s2, s).SetTo(s3))
				writer->WriteLine(s3->ToCString());
			else
				writer->WriteLine();
		}
	}
	return true;
}
