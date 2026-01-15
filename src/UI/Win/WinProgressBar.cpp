#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.hpp"
#include "UI/Win/WinCore.h"
#include "UI/Win/WinProgressBar.h"

#include <windows.h>
#include <commctrl.h>

UI::Win::WinProgressBar::WinProgressBar(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, UInt64 totalCnt) : UI::GUIProgressBar(ui, parent)
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
	SendMessage((HWND)this->hwnd.OrNull(), PBM_SETRANGE32, 0, 65536);
	SendMessage((HWND)this->hwnd.OrNull(), PBM_SETPOS, 0, 0);
}

UI::Win::WinProgressBar::~WinProgressBar()
{
}

IntOS UI::Win::WinProgressBar::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::Win::WinProgressBar::ProgressStart(Text::CStringNN name, UInt64 count)
{
	this->totalCnt = count;
	SendMessage((HWND)this->hwnd.OrNull(), PBM_SETPOS, 0, 0);
}

void UI::Win::WinProgressBar::ProgressUpdate(UInt64 currCount, UInt64 newCount)
{
	UIntOS pos;
	if (totalCnt == 0)
	{
		pos = 0;
	}
	else
	{
		pos = (UIntOS)((currCount << 16) / this->totalCnt);
	}
	SendMessage((HWND)this->hwnd.OrNull(), PBM_SETPOS, pos, 0);
}

void UI::Win::WinProgressBar::ProgressEnd()
{
	SendMessage((HWND)this->hwnd.OrNull(), PBM_SETPOS, 0, 0);
}

