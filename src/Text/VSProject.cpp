#include "Stdafx.h"
#include "IO/Path.h"
#include "Text/MyString.h"
#include "Text/VSProject.h"

Text::VSFile::VSFile(NN<Text::String> fileName)
{
	this->fileName = fileName->Clone();
}

Text::VSFile::VSFile(Text::CStringNN fileName)
{
	this->fileName = Text::String::New(fileName);
}

Text::VSFile::~VSFile()
{
	this->fileName->Release();
}

NN<Text::String> Text::VSFile::GetFileName() const
{
	return this->fileName;
}

Text::VSContainer::VSContainer(NN<Text::String> contName)
{
	this->contName = contName->Clone();
}

Text::VSContainer::VSContainer(Text::CStringNN contName)
{
	this->contName = Text::String::New(contName);
}

Text::VSContainer::~VSContainer()
{
	this->contName->Release();
	this->childList.DeleteAll();
}

void Text::VSContainer::SetContainerName(Text::CString contName)
{
	if (contName.leng > 0)
	{
		this->contName->Release();
		this->contName = Text::String::New(contName);
	}
}

NN<Text::String> Text::VSContainer::GetContainerName() const
{
	return this->contName;
}

UOSInt Text::VSContainer::GetChildCount() const
{
	return this->childList.GetCount();
}

NN<Text::CodeObject> Text::VSContainer::GetChildNoCheck(UOSInt index) const
{
	return this->childList.GetItemNoCheck(index);
}

Optional<Text::CodeObject> Text::VSContainer::GetChildObj(UOSInt index) const
{
	return this->childList.GetItem(index);
}

void Text::VSContainer::AddChild(NN<Text::CodeObject> obj)
{
	this->childList.Add(obj);
}

Text::VSProject::VSProject(Text::CStringNN name, VisualStudioVersion ver) : Text::CodeProject(name)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
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
	this->projName->Release();
	this->childList.DeleteAll();
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

NN<Text::String> Text::VSProject::GetContainerName() const
{
	return this->projName;
}

UOSInt Text::VSProject::GetChildCount() const
{
	return this->childList.GetCount();
}

NN<Text::CodeObject> Text::VSProject::GetChildNoCheck(UOSInt index) const
{
	return this->childList.GetItemNoCheck(index);
}

Optional<Text::CodeObject> Text::VSProject::GetChildObj(UOSInt index) const
{
	return this->childList.GetItem(index);
}

void Text::VSProject::AddChild(NN<Text::CodeObject> obj)
{
	this->childList.Add(obj);
}

Text::VSProject::VisualStudioVersion Text::VSProject::GetVSVersion() const
{
	return this->ver;
}
