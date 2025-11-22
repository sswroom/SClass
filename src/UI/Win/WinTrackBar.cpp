#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.hpp"
#include "UI/Win/WinCore.h"
#include "UI/Win/WinTrackBar.h"

#include <windows.h>
#include <commctrl.h>

UI::Win::WinTrackBar::WinTrackBar(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, UOSInt minVal, UOSInt maxVal, UOSInt currVal) : UI::GUITrackBar(ui, parent)
{
    INITCOMMONCONTROLSEX icex;

	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_BAR_CLASSES;
    InitCommonControlsEx(&icex);

	UInt32 style = WS_CHILD | TBS_NOTICKS;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	this->InitControl(((UI::Win::WinCore*)ui.Ptr())->GetHInst(), parent, TRACKBAR_CLASSW, (const UTF8Char*)"", style, 0, 0, 0, 200, 24);
	SendMessage((HWND)this->hwnd.OrNull(), TBM_SETRANGEMIN, 0, (LPARAM)minVal);
	SendMessage((HWND)this->hwnd.OrNull(), TBM_SETRANGEMAX, 0, (LPARAM)maxVal);
	SendMessage((HWND)this->hwnd.OrNull(), TBM_SETPOS, 0, (LPARAM)currVal);
}

UI::Win::WinTrackBar::~WinTrackBar()
{
}

OSInt UI::Win::WinTrackBar::OnNotify(UInt32 code, void *lParam)
{
	switch (code)
	{
	case WM_HSCROLL:
	case WM_VSCROLL:
		break;
	case NM_CUSTOMDRAW:
		break;
	case NM_RELEASEDCAPTURE:
		EventScrolled(GetPos());
		break;
	case TB_PAGEUP:
		break;
	default:
		EventScrolled(GetPos());
		break;
	}
	return 0;
}

void UI::Win::WinTrackBar::SetPos(UOSInt pos)
{
	SendMessage((HWND)this->hwnd.OrNull(), TBM_SETPOS, TRUE, (LPARAM)pos);
	EventScrolled(GetPos());
//	InvalidateRect((HWND)this->hwnd, 0, false);
}

void UI::Win::WinTrackBar::SetRange(UOSInt minVal, UOSInt maxVal)
{
	SendMessage((HWND)this->hwnd.OrNull(), TBM_SETRANGEMIN, 0, (LPARAM)minVal);
	SendMessage((HWND)this->hwnd.OrNull(), TBM_SETRANGEMAX, 0, (LPARAM)maxVal);
}

UOSInt UI::Win::WinTrackBar::GetPos()
{
	return (UOSInt)SendMessage((HWND)this->hwnd.OrNull(), TBM_GETPOS, 0, 0);
}
