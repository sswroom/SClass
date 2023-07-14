#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/WSConfigFile.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"

IO::ConfigFile *IO::WSConfigFile::Parse(Text::CString fileName)
{
	IO::ConfigFile *cfg;
	IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
	{
		return 0;
	}
	Text::StringBuilderUTF8 sb;
	UOSInt i;
	Text::PString sarr[2];
	Text::UTF8Reader reader(fs);
	NEW_CLASS(cfg, IO::ConfigFile());
	while (reader.ReadLine(sb, 4096))
	{
		i = sb.IndexOf('#');
		if (i != INVALID_INDEX)
		{
			sb.RemoveChars(sb.GetLength() - i);
		}
		sb.TrimWSCRLF();
		if (Text::StrSplitWSP(sarr, 2, sb) == 2)
		{
			cfg->SetValue(CSTR_NULL, sarr[0].ToCString(), sarr[1].ToCString());
		}
	}
	return cfg;
}
