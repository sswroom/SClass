#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "UI/Win/WinCore.h"
#include "UI/Win/WinProgressBar.h"

#include <windows.h>
#include <commctrl.h>

UI::Win::WinProgressBar::WinProgressBar(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, UInt64 totalCnt) : UI::GUIProgressBar(ui, parent)
{
    INITCOMMONCONTROLSEX icex;

	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_PROGRESS_CLASS;
    InitCommonControlsEx(&icex);

	UInt32 style = WS_CHILD | PBS_SMOOTH;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	this->InitControl(((UI::Win::WinCore*)ui.Ptr())->GetHInst(), parent, PROGRESS_CLASSW, (const UTF8Char*)"", style, 0, 0, 0, 200, 24);
	this->totalCnt = totalCnt;
	SendMessage((HWND)this->hwnd, PBM_SETRANGE32, 0, 65536);
	SendMessage((HWND)this->hwnd, PBM_SETPOS, 0, 0);
}

UI::Win::WinProgressBar::~WinProgressBar()
{
}

OSInt UI::Win::WinProgressBar::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::Win::WinProgressBar::ProgressStart(Text::CString name, UInt64 count)
{
	this->totalCnt = count;
	SendMessage((HWND)this->hwnd, PBM_SETPOS, 0, 0);
}

void UI::Win::WinProgressBar::ProgressUpdate(UInt64 currCount, UInt64 newCount)
{
	UOSInt pos;
	if (totalCnt == 0)
	{
		pos = 0;
	}
	else
	{
		pos = (UOSInt)((currCount << 16) / this->totalCnt);
	}
	SendMessage((HWND)this->hwnd, PBM_SETPOS, pos, 0);
}

void UI::Win::WinProgressBar::ProgressEnd()
{
	SendMessage((HWND)this->hwnd, PBM_SETPOS, 0, 0);
}

