#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/WSConfigFile.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"

IO::ConfigFile *IO::WSConfigFile::Parse(Text::CString fileName)
{
	IO::ConfigFile *cfg;
	IO::FileStream *fs;
	Text::UTF8Reader *reader;
	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		return 0;
	}
	Text::StringBuilderUTF8 sb;
	UOSInt i;
	Text::PString sarr[2];
	NEW_CLASS(reader, Text::UTF8Reader(fs));
	NEW_CLASS(cfg, IO::ConfigFile());
	while (reader->ReadLine(&sb, 4096))
	{
		i = sb.IndexOf('#');
		if (i != INVALID_INDEX)
		{
			sb.RemoveChars(sb.GetLength() - i);
		}
		sb.TrimWSCRLF();
		if (Text::StrSplitWSP(sarr, 2, sb) == 2)
		{
			cfg->SetValue(0, 0, sarr[0].v, sarr[0].leng, sarr[1].v, sarr[1].leng);
		}
	}
	DEL_CLASS(reader);
	DEL_CLASS(fs);
	return cfg;
}
