#include "Stdafx.h"
#include "IO/FileAnalyse/SBFrameDetail.h"

IO::FileAnalyse::SBFrameDetail::SBFrameDetail(NN<Text::StringBuilderUTF8> sb)
{
	this->sb = sb;
}

IO::FileAnalyse::SBFrameDetail::~SBFrameDetail()
{

}

void IO::FileAnalyse::SBFrameDetail::AddHeader(Text::CStringNN header)
{
	sb->AppendC(UTF8STRC("\r\n"));
	sb->Append(header);
}

void IO::FileAnalyse::SBFrameDetail::AddField(UOSInt ofst, UOSInt size, Text::CStringNN name, Text::CString value)
{
	Text::CStringNN nnvalue;
	sb->AppendC(UTF8STRC("\r\n"));
	sb->Append(name);
	if (value.SetTo(nnvalue))
	{
		sb->AppendUTF8Char('=');
		sb->Append(nnvalue);
	}
}

void IO::FileAnalyse::SBFrameDetail::AddSubfield(UOSInt ofst, UOSInt size, Text::CStringNN name, Text::CString value)
{
	Text::CStringNN nnvalue;
	sb->AppendC(UTF8STRC("\r\n-"));
	sb->Append(name);
	if (value.SetTo(nnvalue))
	{
		sb->AppendUTF8Char('=');
		sb->Append(nnvalue);
	}
}

void IO::FileAnalyse::SBFrameDetail::AddFieldSeperstor(UOSInt ofst, Text::CStringNN name)
{
	sb->AppendC(UTF8STRC("\r\n\r\n"));
	sb->Append(name);
}

void IO::FileAnalyse::SBFrameDetail::AddText(UOSInt ofst, Text::CStringNN name)
{
	sb->AppendC(UTF8STRC("\r\n"));
	sb->Append(name);
}

void IO::FileAnalyse::SBFrameDetail::AddSubframe(UOSInt ofst, UOSInt size)
{
}
