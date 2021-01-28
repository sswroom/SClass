#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/TextBinEnc/ANSITextBinEnc.h"

Text::TextBinEnc::ANSITextBinEnc::ANSITextBinEnc() : Text::TextBinEnc::CodePageTextBinEnc(0)
{
}

Text::TextBinEnc::ANSITextBinEnc::~ANSITextBinEnc()
{
}

const UTF8Char *Text::TextBinEnc::ANSITextBinEnc::GetName()
{
	return (const UTF8Char*)"ANSI Text";
}
