#include "Stdafx.h"
#include "IO/Path.h"
#include "Text/MyString.h"
#include "Text/VSProject.h"

Text::VSFile::VSFile(const UTF8Char *fileName)
{
	this->fileName = Text::StrCopyNew(fileName);
}

Text::VSFile::~VSFile()
{
	Text::StrDelNew(this->fileName);
}

const UTF8Char *Text::VSFile::GetFileName()
{
	return this->fileName;
}

Text::VSContainer::VSContainer(const UTF8Char *contName)
{
	this->contName = Text::StrCopyNew(contName);
	NEW_CLASS(this->childList, Data::ArrayList<Text::CodeObject*>());
}

Text::VSContainer::~VSContainer()
{
	Text::CodeObject *child;
	UOSInt i;
	Text::StrDelNew(this->contName);
	i = this->childList->GetCount();
	while (i-- > 0)
	{
		child = this->childList->GetItem(i);
		DEL_CLASS(child);
	}
	DEL_CLASS(this->childList);
}

void Text::VSContainer::SetContainerName(const UTF8Char *contName)
{
	if (contName)
	{
		Text::StrDelNew(this->contName);
		this->contName = Text::StrCopyNew(contName);
	}
}

const UTF8Char *Text::VSContainer::GetContainerName()
{
	return this->contName;
}

UOSInt Text::VSContainer::GetChildCount()
{
	return this->childList->GetCount();
}

Text::CodeObject *Text::VSContainer::GetChildObj(UOSInt index)
{
	return this->childList->GetItem(index);
}

void Text::VSContainer::AddChild(Text::CodeObject *obj)
{
	this->childList->Add(obj);
}

Text::VSProject::VSProject(const UTF8Char *name, VisualStudioVersion ver) : Text::CodeProject(name)
{
	UTF8Char sbuff[512];
	OSInt i;
	i = Text::StrLastIndexOf(name, IO::Path::PATH_SEPERATOR);
	Text::StrConcat(sbuff, &name[i + 1]);
	i = Text::StrLastIndexOf(sbuff, '.');
	if (i >= 0)
	{
		sbuff[i] = 0;
	}
	this->ver = ver;
	this->projName = Text::StrCopyNew(sbuff);
	NEW_CLASS(this->childList, Data::ArrayList<Text::CodeObject*>());
}

Text::VSProject::~VSProject()
{
	UOSInt i;
	Text::CodeObject *child;
	SDEL_TEXT(this->projName);
	i = this->childList->GetCount();
	while (i-- > 0)
	{
		child = this->childList->GetItem(i);
		DEL_CLASS(child);
	}
	DEL_CLASS(this->childList);
}

Text::CodeProject::ProjectType Text::VSProject::GetProjectType()
{
	return Text::CodeProject::PROJT_VSPROJECT;
}

void Text::VSProject::SetProjectName(const UTF8Char *projName)
{
	if (this->projName)
	{
		SDEL_TEXT(this->projName);
		this->projName = Text::StrCopyNew(projName);
	}
}

const UTF8Char *Text::VSProject::GetContainerName()
{
	return this->projName;
}

UOSInt Text::VSProject::GetChildCount()
{
	return this->childList->GetCount();
}

Text::CodeObject *Text::VSProject::GetChildObj(UOSInt index)
{
	return this->childList->GetItem(index);
}

void Text::VSProject::AddChild(Text::CodeObject *obj)
{
	this->childList->Add(obj);
}

Text::VSProject::VisualStudioVersion Text::VSProject::GetVSVersion()
{
	return this->ver;
}
