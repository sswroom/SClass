#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "UI/GUICoreWin.h"
#include "UI/GUITrackBar.h"

#include <windows.h>
#include <commctrl.h>

void UI::GUITrackBar::EventScrolled()
{
	UOSInt i;
	OSInt pos;
	pos = SendMessage((HWND)this->hwnd, TBM_GETPOS, 0, 0);
	i = this->scrollHandlers.GetCount();
	while (i-- > 0)
	{
		this->scrollHandlers.GetItem(i)(this->scrollHandlersObj.GetItem(i), (UOSInt)pos);
	}
}

UI::GUITrackBar::GUITrackBar(NotNullPtr<UI::GUICore> ui, UI::GUIClientControl *parent, UOSInt minVal, UOSInt maxVal, UOSInt currVal) : UI::GUIControl(ui, parent)
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
	this->InitControl(((UI::GUICoreWin*)ui.Ptr())->GetHInst(), parent, TRACKBAR_CLASSW, (const UTF8Char*)"", style, 0, 0, 0, 200, 24);
	SendMessage((HWND)this->hwnd, TBM_SETRANGEMIN, 0, (LPARAM)minVal);
	SendMessage((HWND)this->hwnd, TBM_SETRANGEMAX, 0, (LPARAM)maxVal);
	SendMessage((HWND)this->hwnd, TBM_SETPOS, 0, (LPARAM)currVal);
}

UI::GUITrackBar::~GUITrackBar()
{
}

Text::CString UI::GUITrackBar::GetObjectClass()
{
	return CSTR("TrackBar");
}

OSInt UI::GUITrackBar::OnNotify(UInt32 code, void *lParam)
{
	switch (code)
	{
	case WM_HSCROLL:
	case WM_VSCROLL:
		break;
	case NM_CUSTOMDRAW:
		break;
	case NM_RELEASEDCAPTURE:
		EventScrolled();
		break;
	case TB_PAGEUP:
		break;
	default:
		EventScrolled();
		break;
	}
	return 0;
}

void UI::GUITrackBar::SetPos(UOSInt pos)
{
	SendMessage((HWND)this->hwnd, TBM_SETPOS, TRUE, (LPARAM)pos);
	EventScrolled();
//	InvalidateRect((HWND)this->hwnd, 0, false);
}

void UI::GUITrackBar::SetRange(UOSInt minVal, UOSInt maxVal)
{
	SendMessage((HWND)this->hwnd, TBM_SETRANGEMIN, 0, (LPARAM)minVal);
	SendMessage((HWND)this->hwnd, TBM_SETRANGEMAX, 0, (LPARAM)maxVal);
}

UOSInt UI::GUITrackBar::GetPos()
{
	return (UOSInt)SendMessage((HWND)this->hwnd, TBM_GETPOS, 0, 0);
}

void UI::GUITrackBar::HandleScrolled(ScrollEvent hdlr, void *userObj)
{
	this->scrollHandlers.Add(hdlr);
	this->scrollHandlersObj.Add(userObj);
}
