#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/TextBinEnc/ANSITextBinEnc.h"

Text::TextBinEnc::ANSITextBinEnc::ANSITextBinEnc() : Text::TextBinEnc::CodePageTextBinEnc(0)
{
}

Text::TextBinEnc::ANSITextBinEnc::~ANSITextBinEnc()
{
}

Text::CStringNN Text::TextBinEnc::ANSITextBinEnc::GetName() const
{
	return CSTR("ANSI Text");
}
