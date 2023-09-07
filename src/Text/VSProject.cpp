#include "Stdafx.h"
#include "IO/Path.h"
#include "Text/MyString.h"
#include "Text/VSProject.h"

Text::VSFile::VSFile(NotNullPtr<Text::String> fileName)
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

NotNullPtr<Text::String> Text::VSFile::GetFileName() const
{
	return this->fileName;
}

Text::VSContainer::VSContainer(NotNullPtr<Text::String> contName)
{
	this->contName = contName->Clone();
}

Text::VSContainer::VSContainer(Text::CString contName)
{
	this->contName = Text::String::New(contName);
}

Text::VSContainer::~VSContainer()
{
	Text::CodeObject *child;
	UOSInt i;
	this->contName->Release();
	i = this->childList.GetCount();
	while (i-- > 0)
	{
		child = this->childList.GetItem(i);
		DEL_CLASS(child);
	}
}

void Text::VSContainer::SetContainerName(Text::CString contName)
{
	if (contName.leng > 0)
	{
		this->contName->Release();
		this->contName = Text::String::New(contName);
	}
}

NotNullPtr<Text::String> Text::VSContainer::GetContainerName() const
{
	return this->contName;
}

UOSInt Text::VSContainer::GetChildCount() const
{
	return this->childList.GetCount();
}

Text::CodeObject *Text::VSContainer::GetChildObj(UOSInt index) const
{
	return this->childList.GetItem(index);
}

void Text::VSContainer::AddChild(Text::CodeObject *obj)
{
	this->childList.Add(obj);
}

Text::VSProject::VSProject(Text::CStringNN name, VisualStudioVersion ver) : Text::CodeProject(name)
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
}

Text::VSProject::~VSProject()
{
	UOSInt i;
	Text::CodeObject *child;
	this->projName->Release();
	i = this->childList.GetCount();
	while (i-- > 0)
	{
		child = this->childList.GetItem(i);
		DEL_CLASS(child);
	}
}

Text::CodeProject::ProjectType Text::VSProject::GetProjectType() const
{
	return Text::CodeProject::PROJT_VSPROJECT;
}

void Text::VSProject::SetProjectName(Text::String *projName)
{
	if (projName)
	{
		this->projName->Release();
		this->projName = projName->Clone();
	}
}

void Text::VSProject::SetProjectName(Text::CString projName)
{
	if (projName.leng > 0)
	{
		this->projName->Release();
		this->projName = Text::String::New(projName);
	}
}

NotNullPtr<Text::String> Text::VSProject::GetContainerName() const
{
	return this->projName;
}

UOSInt Text::VSProject::GetChildCount() const
{
	return this->childList.GetCount();
}

Text::CodeObject *Text::VSProject::GetChildObj(UOSInt index) const
{
	return this->childList.GetItem(index);
}

void Text::VSProject::AddChild(Text::CodeObject *obj)
{
	this->childList.Add(obj);
}

Text::VSProject::VisualStudioVersion Text::VSProject::GetVSVersion() const
{
	return this->ver;
}
