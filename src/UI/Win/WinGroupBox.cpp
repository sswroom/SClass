#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/Win/WinCore.h"
#include "UI/Win/WinGroupBox.h"
#include <windows.h>
#define BGBRUSH ((HBRUSH)COLOR_BTNSHADOW)

#ifdef _WIN32_WCE
#define WS_EX_CONTROLPARENT 0
#define GWLP_WNDPROC GWL_WNDPROC
#endif

#ifndef GWL_USERDATA
#define GWL_USERDATA GWLP_USERDATA
#endif


IntOS __stdcall UI::Win::WinGroupBox::GBWndProc(void *hWnd, UInt32 msg, UIntOS wParam, IntOS lParam)
{
	UI::Win::WinGroupBox *me = (UI::Win::WinGroupBox*)UI::Win::WinCore::MSGetWindowObj((ControlHandle*)hWnd, GWL_USERDATA);
	UI::GUIControl*ctrl;
	NMHDR *nmhdr;
	switch (msg)
	{
	case WM_COMMAND:
		ctrl = (UI::GUIControl*)UI::Win::WinCore::MSGetWindowObj((ControlHandle*)lParam, GWL_USERDATA);
		if (ctrl)
		{
			return ctrl->OnNotify(HIWORD(wParam), 0);
		}
		break;
	case WM_NOTIFY:
		nmhdr = (NMHDR*)lParam;
		ctrl = (UI::GUIControl*)UI::Win::WinCore::MSGetWindowObj((ControlHandle*)nmhdr->hwndFrom, GWL_USERDATA);
		if (ctrl)
		{
			return ctrl->OnNotify(nmhdr->code, (void*)lParam);
		}
		break;
	case WM_ERASEBKGND:
		{
			RECT rc;
			GetClientRect((HWND)hWnd, &rc);
			FillRect((HDC)wParam, &rc, BGBRUSH);
		}
		break;
	default:
		return CallWindowProc((WNDPROC)me->oriWndProc, (HWND)hWnd, msg, wParam, lParam);
	}
	return 0;
}

UI::Win::WinGroupBox::WinGroupBox(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN text) : UI::GUIGroupBox(ui, parent)
{
	UInt32 style = WS_CLIPSIBLINGS | WS_CHILD | BS_GROUPBOX;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	this->InitControl(((UI::Win::WinCore*)ui.Ptr())->GetHInst(), parent, L"BUTTON", text.v, style, WS_EX_CONTROLPARENT, 0, 0, 200, 200);
	this->oriWndProc = (void*)UI::Win::WinCore::MSSetWindowObj(this->hwnd, GWLP_WNDPROC, (IntOS)GBWndProc);
}

Math::Coord2DDbl UI::Win::WinGroupBox::GetClientOfst()
{
	return Math::Coord2DDbl(3, 15);
}

Math::Size2DDbl UI::Win::WinGroupBox::GetClientSize()
{
	return this->GetSize() - Math::Double2D(6, 18);
}

UI::Win::WinGroupBox::~WinGroupBox()
{
	UI::Win::WinCore::MSSetWindowObj(this->hwnd, GWLP_WNDPROC, (IntOS)this->oriWndProc);
}
