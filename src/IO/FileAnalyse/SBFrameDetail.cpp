#include "Stdafx.h"
#include "IO/FileAnalyse/SBFrameDetail.h"

IO::FileAnalyse::SBFrameDetail::SBFrameDetail(Text::StringBuilderUTF *sb)
{
	this->sb = sb;
}

IO::FileAnalyse::SBFrameDetail::~SBFrameDetail()
{

}

void IO::FileAnalyse::SBFrameDetail::AddHeader(const UTF8Char *header)
{
	sb->Append((const UTF8Char *)"\r\n");
	sb->Append(header);
}

void IO::FileAnalyse::SBFrameDetail::AddField(UOSInt ofst, UOSInt size, const UTF8Char *name, const UTF8Char *value)
{
	sb->Append((const UTF8Char *)"\r\n");
	sb->Append(name);
	if (value)
	{
		sb->AppendChar('=', 1);
		sb->Append(value);
	}
}

void IO::FileAnalyse::SBFrameDetail::AddSubfield(UOSInt ofst, UOSInt size, const UTF8Char *name, const UTF8Char *value)
{
	sb->Append((const UTF8Char *)"\r\n-");
	sb->Append(name);
	if (value)
	{
		sb->AppendChar('=', 1);
		sb->Append(value);
	}
}

void IO::FileAnalyse::SBFrameDetail::AddFieldSeperstor(UOSInt ofst, const UTF8Char *name)
{
	sb->Append((const UTF8Char *)"\r\n\r\n");
	sb->Append(name);
}

void IO::FileAnalyse::SBFrameDetail::AddText(UOSInt ofst, const UTF8Char *name)
{
	sb->Append((const UTF8Char *)"\r\n");
	sb->Append(name);
}

void IO::FileAnalyse::SBFrameDetail::AddSubframe(UOSInt ofst, UOSInt size)
{
}