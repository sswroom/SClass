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
	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		return 0;
	}
	Text::StringBuilderUTF8 sb;
	OSInt i;
	UTF8Char *sarr[2];
	NEW_CLASS(reader, Text::UTF8Reader(fs));
	NEW_CLASS(cfg, IO::ConfigFile());
	while (reader->ReadLine(&sb, 4096))
	{
		i = sb.IndexOf('#');
		if (i >= 0)
		{
			sb.RemoveChars(sb.GetLength() - (UOSInt)i);
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
