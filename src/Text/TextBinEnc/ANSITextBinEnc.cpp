#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/TextBinEnc/ANSITextBinEnc.h"

Text::TextBinEnc::ANSITextBinEnc::ANSITextBinEnc() : Text::TextBinEnc::CodePageTextBinEnc(0)
{
}

Text::TextBinEnc::ANSITextBinEnc::~ANSITextBinEnc()
{
}

Text::CString Text::TextBinEnc::ANSITextBinEnc::GetName()
{
	return CSTR("ANSI Text");
}
