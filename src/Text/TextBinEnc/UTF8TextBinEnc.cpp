#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/TextBinEnc/UTF8TextBinEnc.h"

Text::TextBinEnc::UTF8TextBinEnc::UTF8TextBinEnc() : Text::TextBinEnc::CodePageTextBinEnc(65001)
{
}

Text::TextBinEnc::UTF8TextBinEnc::~UTF8TextBinEnc()
{
}

const UTF8Char *Text::TextBinEnc::UTF8TextBinEnc::GetName()
{
	return (const UTF8Char*)"UTF-8 Text";
}
