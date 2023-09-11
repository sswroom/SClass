#include "Stdafx.h"
#include "Text/CodeProject.h"
#include "Text/MyString.h"

Text::CodeObject::~CodeObject()
{
}

Text::CodeContainer::~CodeContainer()
{
}

Text::CodeObject::ObjectType Text::CodeContainer::GetObjectType() const
{
	return Text::CodeObject::OT_CONTAINER;
}

Text::CodeFile::~CodeFile()
{
}

Text::CodeObject::ObjectType Text::CodeFile::GetObjectType() const
{
	return Text::CodeObject::OT_FILE;
}

Text::CodeProjectCfg::CodeProjectCfg(NotNullPtr<Text::String> name)
{
	this->cfgName = name->Clone();
}

Text::CodeProjectCfg::~CodeProjectCfg()
{
	this->cfgName->Release();
}

NotNullPtr<Text::String> Text::CodeProjectCfg::GetCfgName() const
{
	return this->cfgName;
}

Text::CodeProject::CodeProject(Text::CStringNN name) : IO::ParsedObject(name)
{
}

Text::CodeProject::~CodeProject()
{
	UOSInt i;
	CodeProjectCfg *cfg;
	i = this->cfgList.GetCount();
	while (i-- > 0)
	{
		cfg = this->cfgList.GetItem(i);
		DEL_CLASS(cfg);
	}
}

void Text::CodeProject::AddConfig(Text::CodeProjectCfg *cfg)
{
	this->cfgList.Add(cfg);
}

UOSInt Text::CodeProject::GetConfigCnt() const
{
	return this->cfgList.GetCount();
}

Text::CodeProjectCfg *Text::CodeProject::GetConfig(UOSInt index) const
{
	return this->cfgList.GetItem(index);
}

IO::ParserType Text::CodeProject::GetParserType() const
{
	return IO::ParserType::CodeProject;
}
