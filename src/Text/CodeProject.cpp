#include "Stdafx.h"
#include "Text/CodeProject.h"
#include "Text/MyString.h"

Text::CodeObject::~CodeObject()
{
}

Text::CodeContainer::~CodeContainer()
{
}

Text::CodeObject::ObjectType Text::CodeContainer::GetObjectType()
{
	return Text::CodeObject::OT_CONTAINER;
}

Text::CodeFile::~CodeFile()
{
}

Text::CodeObject::ObjectType Text::CodeFile::GetObjectType()
{
	return Text::CodeObject::OT_FILE;
}

Text::CodeProjectCfg::CodeProjectCfg(Text::String *name)
{
	this->cfgName = name->Clone();
}

Text::CodeProjectCfg::~CodeProjectCfg()
{
	this->cfgName->Release();
}

Text::String *Text::CodeProjectCfg::GetCfgName()
{
	return this->cfgName;
}

Text::CodeProject::CodeProject(const UTF8Char *name) : IO::ParsedObject(name)
{
	NEW_CLASS(this->cfgList, Data::ArrayList<CodeProjectCfg*>());
}

Text::CodeProject::~CodeProject()
{
	UOSInt i;
	CodeProjectCfg *cfg;
	i = this->cfgList->GetCount();
	while (i-- > 0)
	{
		cfg = this->cfgList->GetItem(i);
		DEL_CLASS(cfg);
	}
	DEL_CLASS(this->cfgList);
}

void Text::CodeProject::AddConfig(Text::CodeProjectCfg *cfg)
{
	this->cfgList->Add(cfg);
}

UOSInt Text::CodeProject::GetConfigCnt()
{
	return this->cfgList->GetCount();
}

Text::CodeProjectCfg *Text::CodeProject::GetConfig(UOSInt index)
{
	return this->cfgList->GetItem(index);
}

IO::ParserType Text::CodeProject::GetParserType()
{
	return IO::ParserType::CodeProject;
}
