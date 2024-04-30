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

Text::CodeProjectCfg::CodeProjectCfg(NN<Text::String> name)
{
	this->cfgName = name->Clone();
}

Text::CodeProjectCfg::~CodeProjectCfg()
{
	this->cfgName->Release();
}

NN<Text::String> Text::CodeProjectCfg::GetCfgName() const
{
	return this->cfgName;
}

Text::CodeProject::CodeProject(Text::CStringNN name) : IO::ParsedObject(name)
{
}

Text::CodeProject::~CodeProject()
{
	this->cfgList.DeleteAll();
}

void Text::CodeProject::AddConfig(NN<Text::CodeProjectCfg> cfg)
{
	this->cfgList.Add(cfg);
}

UOSInt Text::CodeProject::GetConfigCnt() const
{
	return this->cfgList.GetCount();
}

Optional<Text::CodeProjectCfg> Text::CodeProject::GetConfig(UOSInt index) const
{
	return this->cfgList.GetItem(index);
}

IO::ParserType Text::CodeProject::GetParserType() const
{
	return IO::ParserType::CodeProject;
}
