#include "Stdafx.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "UI/GUIFileDialog.h"

void UI::GUIFileDialog::ClearFileNames()
{
	this->fileNames.FreeAll();
}

UI::GUIFileDialog::GUIFileDialog(const WChar *compName, const WChar *appName, const WChar *dialogName, Bool isSave)
{
	UOSInt i;
	WChar buff[256];
	UnsafeArray<WChar> wptr;
	this->reg = IO::Registry::OpenSoftware(IO::Registry::REG_USER_THIS, compName, appName);
	this->isSave = isSave;
	this->filterIndex = (UOSInt)-1;
	this->allowMulti = false;
	i = Text::StrCharCnt(dialogName);
	this->dialogName = MemAlloc(WChar, i + 7);
	wptr = Text::StrConcat(this->dialogName, dialogName);
	wptr = Text::StrConcat(wptr, L"Dialog");

	this->fileName = 0;
	this->lastName = 0;
	if (this->reg->GetValueStr(this->dialogName, buff).SetTo(wptr))
	{
		this->lastName = Text::StrCopyNew(buff);
	}
}

UI::GUIFileDialog::~GUIFileDialog()
{
	IO::Registry::CloseRegistry(this->reg);
	MemFree(this->dialogName);
	UnsafeArray<const WChar> lastName;
	if (this->lastName.SetTo(lastName)) Text::StrDelNew(lastName);
	OPTSTR_DEL(this->fileName);
	this->patterns.FreeAll();
	this->names.FreeAll();
	this->ClearFileNames();
}

void UI::GUIFileDialog::AddFilter(Text::CStringNN pattern, Text::CStringNN name)
{
	this->patterns.Add(Text::String::New(pattern));
	this->names.Add(Text::String::New(name));
}

UOSInt UI::GUIFileDialog::GetFilterIndex()
{
	return this->filterIndex;
}

void UI::GUIFileDialog::SetFileName(Text::CString fileName)
{
	OPTSTR_DEL(this->fileName);
	this->fileName = Text::String::NewOrNull(fileName);
}

NN<Text::String> UI::GUIFileDialog::GetFileName() const
{
	return Text::String::OrEmpty(this->fileName);
}

UOSInt UI::GUIFileDialog::GetFileNameCount()
{
	UOSInt cnt = this->fileNames.GetCount();
	if (cnt)
		return cnt;
	if (!this->fileName.IsNull())
		return 1;
	return 0;
}

Optional<Text::String> UI::GUIFileDialog::GetFileNames(UOSInt index)
{
	if (index == 0 && this->fileNames.GetCount() == 0)
		return this->fileName;
	return this->fileNames.GetItem(index);
}

void UI::GUIFileDialog::SetAllowMultiSel(Bool allowMulti)
{
	this->allowMulti = allowMulti;
}
