#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/WSConfigFile.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"

IO::ConfigFile *IO::WSConfigFile::Parse(const UTF8Char *fileName)
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
	UTF8Char *sarr[2];
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
		if (Text::StrSplitWS(sarr, 2, sb.ToString()) == 2)
		{
			cfg->SetValue(0, sarr[0], sarr[1]);
		}
	}
	DEL_CLASS(reader);
	DEL_CLASS(fs);
	return cfg;
}
