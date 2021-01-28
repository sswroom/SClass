#include "Stdafx.h"
#include "IO/IniFile.h"
#include "IO/LangFile.h"

IO::LangFile::LangFile(const UTF8Char *fileName, Int32 codePage)
{
	this->cfg = IO::IniFile::Parse(fileName, codePage);
}

IO::LangFile::~LangFile()
{
	SDEL_CLASS(this->cfg);
}

const UTF8Char *IO::LangFile::GetValue(const UTF8Char *name)
{
	if (this->cfg)
	{
		const UTF8Char *ret = this->cfg->GetValue(name);
		if (ret)
			return ret;
	}
	return name;
}

const UTF8Char *IO::LangFile::GetValue(const UTF8Char *category, const UTF8Char *name)
{
	if (this->cfg)
	{
		const UTF8Char *ret = this->cfg->GetValue(category, name);
		if (ret)
			return ret;
	}
	return name;
}
