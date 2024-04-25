#include "Stdafx.h"
#include "UI/GUIFolderDialog.h"

UI::GUIFolderDialog::GUIFolderDialog()
{
	this->dirName = 0;
	this->message = 0;
}

UI::GUIFolderDialog::~GUIFolderDialog()
{
	OPTSTR_DEL(this->dirName);
	OPTSTR_DEL(this->message);
}

void UI::GUIFolderDialog::SetFolder(Text::CString dirName)
{
	OPTSTR_DEL(this->dirName);
	this->dirName = Text::String::New(dirName);
}

NN<Text::String> UI::GUIFolderDialog::GetFolder() const
{
	return Text::String::OrEmpty(this->dirName);
}

void UI::GUIFolderDialog::SetMessage(Text::CStringNN message)
{
	OPTSTR_DEL(this->message);
	this->message = Text::String::New(message);
}
