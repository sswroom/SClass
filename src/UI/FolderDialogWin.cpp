#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "UI/FolderDialog.h"
#include <windows.h>
#include <shlobj.h>

#define MAXFILENAMESIZE 512

/*		IO::Registry *reg;
		WChar *dialogName;
		WChar *lastName;
		WChar *fileName;
		Data::ArrayList *patterns;
		Data::ArrayList *names;*/

Int32 __stdcall UI::FolderDialog::BrowseCB(void *hwnd, UInt32 uMsg, OSInt lParam, OSInt lpData)
{
	UI::FolderDialog *me = (UI::FolderDialog*)lpData;
	switch (uMsg)
	{
	case BFFM_INITIALIZED:
		if (me->dirName)
		{
			SendMessage((HWND)hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)me->dirName);
		}
		break;
	case BFFM_VALIDATEFAILED:
		return 1;
	}
	return 0;
}

UI::FolderDialog::FolderDialog(const WChar *compName, const WChar *appName, const WChar *dialogName)
{
	this->reg = 0;
	this->dirName = 0;
	this->message = 0;
	CoInitializeEx(0, COINIT_APARTMENTTHREADED);

/*	OSInt i;
	WChar buff[256];
	WChar *wptr;
	this->reg = IO::Registry::OpenSoftware(IO::Registry::REG_USER_THIS, compName, appName);
	this->isSave = isSave;
	i = Text::StrCharCnt(dialogName);
	this->dialogName = MemAlloc(WChar, i + 7);
	wptr = Text::StrConcat(this->dialogName, dialogName);
	wptr = Text::StrConcat(wptr, L"Dialog");

	this->fileName = 0;
	this->lastName = 0;
	wptr = this->reg->GetValueStr(this->dialogName, buff);
	if (wptr)
	{
		this->lastName = Text::StrCopyNew(buff);
	}
	NEW_CLASS(this->names, Data::ArrayListStr());
	NEW_CLASS(this->patterns, Data::ArrayListStr());*/
}

UI::FolderDialog::~FolderDialog()
{
//	IO::Registry::CloseRegistry(this->reg);
	SDEL_STRING(this->dirName);
	SDEL_TEXT(this->message);
	CoUninitialize();
}

void UI::FolderDialog::SetFolder(Text::CString dirName)
{
	SDEL_STRING(this->dirName);
	this->dirName = Text::String::New(dirName).Ptr();
}

NotNullPtr<Text::String> UI::FolderDialog::GetFolder() const
{
	return Text::String::OrEmpty(this->dirName);
}

void UI::FolderDialog::SetMessage(const UTF8Char *message)
{
	SDEL_TEXT(this->message);
	this->message = Text::StrCopyNew(message).Ptr();
}

Bool UI::FolderDialog::ShowDialog(ControlHandle *ownerHandle)
{
	WChar wbuff[MAX_PATH];
	BROWSEINFOW info;
	info.hwndOwner = (HWND)ownerHandle;
	info.pidlRoot = 0;
	if (this->dirName)
	{
		Text::StrUTF8_WChar(wbuff, this->dirName->v, 0);
	}
	else
	{
		wbuff[0] = 0;
	}
	info.pszDisplayName = wbuff;
	const WChar *wptr = 0;
	if (this->message)
	{
		wptr = Text::StrToWCharNew(this->message);
		info.lpszTitle = wptr;
	}
	else
	{
		info.lpszTitle = L"";
	}
	info.ulFlags = BIF_USENEWUI | BIF_EDITBOX;
	info.lpfn = (BFFCALLBACK)BrowseCB;
	info.lParam = (LPARAM)this;
	info.iImage = 0;

	PIDLIST_ABSOLUTE idList = SHBrowseForFolderW(&info);
	SDEL_TEXT(wptr);
	if (info.pidlRoot)
	{
		CoTaskMemFree((LPVOID)info.pidlRoot);
	}

	if (idList == 0)
	{
		return false;
	}
	else
	{
		SHGetPathFromIDListW(idList, wbuff);
		CoTaskMemFree(idList);
		if (this->dirName)
		{
			this->dirName->Release();
			this->dirName = 0;
		}
		this->dirName = Text::String::NewNotNull(wbuff).Ptr();
		return true;
	}
}
