#include "Stdafx.h"
#include "IO/IniFile.h"
#include "IO/LangFile.h"

IO::LangFile::LangFile(Text::CStringNN fileName, UInt32 codePage)
{
	this->cfg = IO::IniFile::Parse(fileName, codePage);
}

IO::LangFile::~LangFile()
{
	SDEL_CLASS(this->cfg);
}

Text::CStringNN IO::LangFile::GetValue(Text::CStringNN name)
{
	if (this->cfg)
	{
		NN<Text::String> ret;
		if (this->cfg->GetValue(name).SetTo(ret))
			return ret->ToCString();
	}
	return name;
}

Text::CStringNN IO::LangFile::GetValue(Text::CStringNN category, Text::CStringNN name)
{
	if (this->cfg)
	{
		NN<Text::String> ret;
		if (this->cfg->GetCateValue(category, name).SetTo(ret))
			return ret->ToCString();
	}
	return name;
}
