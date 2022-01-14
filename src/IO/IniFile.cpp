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

IO::ConfigFile *IO::IniFile::Parse(IO::Stream *stm, UInt32 codePage)
{
	IO::ConfigFile *cfg;
	IO::StreamReader *reader;
	NEW_CLASS(reader, IO::StreamReader(stm, codePage));
	cfg = ParseReader(reader);
	DEL_CLASS(reader);
	return cfg;
}

IO::ConfigFile *IO::IniFile::Parse(const UTF8Char *fileName, UInt32 codePage)
{
	IO::ConfigFile *cfg;
	IO::FileStream *fstm;
	IO::StreamReader *reader;
	NEW_CLASS(fstm, IO::FileStream(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential));
	NEW_CLASS(reader, IO::StreamReader(fstm, codePage));
	cfg = ParseReader(reader);
	DEL_CLASS(reader);
	DEL_CLASS(fstm);
	return cfg;
}

IO::ConfigFile *IO::IniFile::ParseProgConfig(UInt32 codePage)
{
	UTF8Char sbuff[512];
	IO::Path::GetProcessFileName(sbuff);
	IO::Path::ReplaceExt(sbuff, (const UTF8Char*)"cfg");
	return Parse(sbuff, codePage);
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
				cateEnd = Text::StrConcatC(cate, &buff[1], valueEnd - buff - 2);
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
					cfg->SetValue(cate, (UOSInt)(cateEnd - cate), name, (UOSInt)(nameEnd - name), sb.ToString(), sb.GetLength());
				}
				else
				{
					Text::StrTrim(value);
					cfg->SetValue(cate, (UOSInt)(cateEnd - cate), name, (UOSInt)(nameEnd - name), value, (UOSInt)(valueEnd - value));
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

Bool IO::IniFile::SaveConfig(IO::Stream *stm, UInt32 codePage, IO::ConfigFile *cfg)
{
	IO::StreamWriter *writer;
	Bool ret;
	NEW_CLASS(writer, IO::StreamWriter(stm, codePage));
	ret = SaveConfig(writer, cfg);
	DEL_CLASS(writer);
	return ret;
}

Bool IO::IniFile::SaveConfig(IO::Writer *writer, IO::ConfigFile *cfg)
{
	Data::ArrayList<Text::String *> cateList;
	Data::ArrayList<Text::String *> keyList;
	Text::String *s;
	Text::String *s2;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	cfg->GetKeys((Text::String*)0, &keyList);
	i = 0;
	j = keyList.GetCount();
	while (i < j)
	{
		s = keyList.GetItem(i);
		writer->WriteStrC(s->v, s->leng);
		writer->WriteStrC(UTF8STRC("="));
		s = cfg->GetValue(s);
		writer->WriteLineC(s->v, s->leng);
		i++;
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
		k = 0;
		l = keyList.GetCount();
		while (k < l)
		{
			s = keyList.GetItem(k);
			writer->WriteStrC(s->v, s->leng);
			writer->WriteStrC(UTF8STRC("="));
			s = cfg->GetValue(s2, s);
			writer->WriteLineC(s->v, s->leng);
			k++;
		}
		i++;
	}
	return true;
}
