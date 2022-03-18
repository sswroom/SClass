#include "Stdafx.h"
#include "IO/Path.h"
#include "Text/MyString.h"
#include "Text/VSProject.h"

Text::VSFile::VSFile(Text::String *fileName)
{
	this->fileName = fileName->Clone();
}

Text::VSFile::VSFile(Text::CString fileName)
{
	this->fileName = Text::String::New(fileName);
}

Text::VSFile::~VSFile()
{
	this->fileName->Release();
}

Text::String *Text::VSFile::GetFileName()
{
	return this->fileName;
}

Text::VSContainer::VSContainer(Text::String *contName)
{
	this->contName = contName->Clone();
	NEW_CLASS(this->childList, Data::ArrayList<Text::CodeObject*>());
}

Text::VSContainer::VSContainer(Text::CString contName)
{
	this->contName = Text::String::New(contName);
	NEW_CLASS(this->childList, Data::ArrayList<Text::CodeObject*>());
}

Text::VSContainer::~VSContainer()
{
	Text::CodeObject *child;
	UOSInt i;
	this->contName->Release();
	i = this->childList->GetCount();
	while (i-- > 0)
	{
		child = this->childList->GetItem(i);
		DEL_CLASS(child);
	}
	DEL_CLASS(this->childList);
}

void Text::VSContainer::SetContainerName(Text::CString contName)
{
	if (contName.leng > 0)
	{
		this->contName->Release();
		this->contName = Text::String::New(contName);
	}
}

Text::String *Text::VSContainer::GetContainerName()
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

Text::VSProject::VSProject(Text::CString name, VisualStudioVersion ver) : Text::CodeProject(name)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UOSInt i;
	i = name.LastIndexOf(IO::Path::PATH_SEPERATOR);
	sptr = Text::StrConcatC(sbuff, &name.v[i + 1], name.leng - i - 1);
	i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '.');
	if (i != INVALID_INDEX)
	{
		sbuff[i] = 0;
		sptr = &sbuff[i];
	}
	this->ver = ver;
	this->projName = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
	NEW_CLASS(this->childList, Data::ArrayList<Text::CodeObject*>());
}

Text::VSProject::~VSProject()
{
	UOSInt i;
	Text::CodeObject *child;
	SDEL_STRING(this->projName);
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

void Text::VSProject::SetProjectName(Text::String *projName)
{
	if (projName)
	{
		SDEL_STRING(this->projName);
		this->projName = projName->Clone();
	}
}

void Text::VSProject::SetProjectName(Text::CString projName)
{
	if (projName.leng > 0)
	{
		SDEL_STRING(this->projName);
		this->projName = Text::String::New(projName);
	}
}

Text::String *Text::VSProject::GetContainerName()
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
