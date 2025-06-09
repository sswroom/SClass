#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "UI/Win/WinFolderDialog.h"
#include <windows.h>
#include <shlobj.h>

#define MAXFILENAMESIZE 512

Int32 __stdcall UI::Win::WinFolderDialog::BrowseCB(void *hwnd, UInt32 uMsg, OSInt lParam, OSInt lpData)
{
	UI::Win::WinFolderDialog *me = (UI::Win::WinFolderDialog*)lpData;
	NN<Text::String> s;
	switch (uMsg)
	{
	case BFFM_INITIALIZED:
		if (me->dirName.SetTo(s))
		{
			SendMessage((HWND)hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)s->v.Ptr());
		}
		break;
	case BFFM_VALIDATEFAILED:
		return 1;
	}
	return 0;
}

UI::Win::WinFolderDialog::WinFolderDialog()
{
	CoInitializeEx(0, COINIT_APARTMENTTHREADED);
}

UI::Win::WinFolderDialog::~WinFolderDialog()
{
	CoUninitialize();
}

Bool UI::Win::WinFolderDialog::ShowDialog(Optional<ControlHandle> ownerHandle)
{
	WChar wbuff[MAX_PATH];
	BROWSEINFOW info;
	info.hwndOwner = (HWND)ownerHandle.OrNull();
	info.pidlRoot = 0;
	NN<Text::String> s;
	if (this->dirName.SetTo(s))
	{
		Text::StrUTF8_WChar(wbuff, s->v, 0);
	}
	else
	{
		wbuff[0] = 0;
	}
	info.pszDisplayName = wbuff;
	UnsafeArrayOpt<const WChar> wptr = 0;
	UnsafeArray<const WChar> nnwptr;
	if (this->message.SetTo(s))
	{
		wptr = nnwptr = Text::StrToWCharNew(s->v);
		info.lpszTitle = nnwptr.Ptr();
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
	if (wptr.SetTo(nnwptr)) Text::StrDelNew(nnwptr);
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
		OPTSTR_DEL(this->dirName);
		this->dirName = Text::String::NewNotNull(wbuff);
		return true;
	}
}
