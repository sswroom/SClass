#include "Stdafx.h"
#include "IO/IniFile.h"
#include "IO/LangFile.h"

IO::LangFile::LangFile(Text::CStringNN fileName, UInt32 codePage)
{
	this->cfg = IO::IniFile::Parse(fileName, codePage);
}

IO::LangFile::~LangFile()
{
	this->cfg.Delete();
}

Text::CStringNN IO::LangFile::GetValue(Text::CStringNN name)
{
	NN<IO::ConfigFile> cfg;
	if (this->cfg.SetTo(cfg))
	{
		NN<Text::String> ret;
		if (cfg->GetValue(name).SetTo(ret))
			return ret->ToCString();
	}
	return name;
}

Text::CStringNN IO::LangFile::GetValue(Text::CStringNN category, Text::CStringNN name)
{
	NN<IO::ConfigFile> cfg;
	if (this->cfg.SetTo(cfg))
	{
		NN<Text::String> ret;
		if (cfg->GetCateValue(category, name).SetTo(ret))
			return ret->ToCString();
	}
	return name;
}
