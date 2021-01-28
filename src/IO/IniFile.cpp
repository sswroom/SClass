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

IO::ConfigFile *IO::IniFile::Parse(IO::Stream *stm, Int32 codePage)
{
	IO::ConfigFile *cfg;
	IO::StreamReader *reader;
	NEW_CLASS(reader, IO::StreamReader(stm, codePage));
	cfg = ParseReader(reader);
	DEL_CLASS(reader);
	return cfg;
}

IO::ConfigFile *IO::IniFile::Parse(const UTF8Char *fileName, Int32 codePage)
{
	IO::ConfigFile *cfg;
	IO::FileStream *fstm;
	IO::StreamReader *reader;
	NEW_CLASS(fstm, IO::FileStream(fileName, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_SEQUENTIAL));
	NEW_CLASS(reader, IO::StreamReader(fstm, codePage));
	cfg = ParseReader(reader);
	DEL_CLASS(reader);
	DEL_CLASS(fstm);
	return cfg;
}

IO::ConfigFile *IO::IniFile::ParseProgConfig(Int32 codePage)
{
	UTF8Char sbuff[512];
	IO::Path::GetProcessFileName(sbuff);
	IO::Path::ReplaceExt(sbuff, (const UTF8Char*)"cfg");
	return Parse(sbuff, codePage);
}

IO::ConfigFile *IO::IniFile::ParseReader(IO::StreamReader *reader)
{
	UTF8Char cate[128];
	UTF8Char buff[1024];
	UTF8Char *name;
	UTF8Char *value;
	UTF8Char *src;
	UTF8Char lbrk[3];
	IO::ConfigFile *cfg;
	NEW_CLASS(cfg, IO::ConfigFile());
	cate[0] = 0;
	while ((name = reader->ReadLine(buff, 1023)) != 0)
	{
		if (buff[0] == '[' && name[-1] == ']')
		{
			if (name - buff < 128)
			{
				Text::StrConcat(cate, &buff[1])[-1] = 0;
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
				*src++ = 0;
				Text::StrTrim(name);
				value = src;
				if (reader->GetLastLineBreak(lbrk) == lbrk)
				{
					Text::StringBuilderUTF8 sb;
					sb.Append(value);
					while (reader->ReadLine(src, 1023 - (src - buff)))
					{
						sb.Append(src);
						if (reader->GetLastLineBreak(lbrk) != lbrk)
							break;
					}
					sb.Trim();
					cfg->SetValue(cate, name, sb.ToString());
				}
				else
				{
					Text::StrTrim(value);
					cfg->SetValue(cate, name, value);
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

Bool IO::IniFile::SaveConfig(IO::Stream *stm, Int32 codePage, IO::ConfigFile *cfg)
{
	IO::StreamWriter *writer;
	Bool ret;
	NEW_CLASS(writer, IO::StreamWriter(stm, codePage));
	ret = SaveConfig(writer, cfg);
	DEL_CLASS(writer);
	return ret;
}

Bool IO::IniFile::SaveConfig(IO::IWriter *writer, IO::ConfigFile *cfg)
{
	Data::ArrayList<const UTF8Char *> *cateList;
	Data::ArrayList<const UTF8Char *> *keyList;
	const UTF8Char *csptr;
	const UTF8Char *csptr2;
	OSInt i;
	OSInt j;
	OSInt k;
	OSInt l;
	NEW_CLASS(keyList, Data::ArrayList<const UTF8Char *>());
	cfg->GetKeys(0, keyList);
	i = 0;
	j = keyList->GetCount();
	while (i < j)
	{
		csptr = keyList->GetItem(i);
		writer->Write(csptr);
		writer->Write((const UTF8Char*)"=");
		writer->WriteLine(cfg->GetValue(csptr));
		i++;
	}
	NEW_CLASS(cateList, Data::ArrayList<const UTF8Char*>());
	cfg->GetCateList(cateList);
	i = 0;
	j = cateList->GetCount();
	while (i < j)
	{
		csptr2 = cateList->GetItem(i);
		writer->WriteLine();
		writer->Write((const UTF8Char*)"[");
		writer->Write(csptr2);
		writer->WriteLine((const UTF8Char*)"]");

		keyList->Clear();
		cfg->GetKeys(csptr2, keyList);
		k = 0;
		l = keyList->GetCount();
		while (k < l)
		{
			csptr = keyList->GetItem(k);
			writer->Write(csptr);
			writer->Write((const UTF8Char*)"=");
			writer->WriteLine(cfg->GetValue(csptr2, csptr));
			k++;
		}
		i++;
	}
	DEL_CLASS(cateList);
	DEL_CLASS(keyList);
	return true;
}
