#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/GUICoreWin.h"
#include "UI/GUIGroupBox.h"
#include <windows.h>
#define BGBRUSH ((HBRUSH)COLOR_BTNSHADOW)

#ifdef _WIN32_WCE
#define WS_EX_CONTROLPARENT 0
#define GWLP_WNDPROC GWL_WNDPROC
#endif

#ifndef GWL_USERDATA
#define GWL_USERDATA GWLP_USERDATA
#endif


OSInt __stdcall UI::GUIGroupBox::GBWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam)
{
	UI::GUIGroupBox *me = (UI::GUIGroupBox*)UI::GUICoreWin::MSGetWindowObj((ControlHandle*)hWnd, GWL_USERDATA);
	UI::GUIControl*ctrl;
	NMHDR *nmhdr;
	switch (msg)
	{
	case WM_COMMAND:
		ctrl = (UI::GUIControl*)UI::GUICoreWin::MSGetWindowObj((ControlHandle*)lParam, GWL_USERDATA);
		if (ctrl)
		{
			return ctrl->OnNotify(HIWORD(wParam), 0);
		}
		break;
	case WM_NOTIFY:
		nmhdr = (NMHDR*)lParam;
		ctrl = (UI::GUIControl*)UI::GUICoreWin::MSGetWindowObj((ControlHandle*)nmhdr->hwndFrom, GWL_USERDATA);
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

UI::GUIGroupBox::GUIGroupBox(UI::GUICore *ui, UI::GUIClientControl *parent, Text::CString text) : UI::GUIClientControl(ui, parent)
{
	UInt32 style = WS_CLIPSIBLINGS | WS_CHILD | BS_GROUPBOX;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	this->InitControl(((UI::GUICoreWin*)ui)->GetHInst(), parent, L"BUTTON", text.v, style, WS_EX_CONTROLPARENT, 0, 0, 200, 200);
	this->oriWndProc = (void*)UI::GUICoreWin::MSSetWindowObj(this->hwnd, GWLP_WNDPROC, (OSInt)GBWndProc);
}

void UI::GUIGroupBox::GetClientOfst(Double *x, Double *y)
{
	if (x)
		*x = 3;
	if (y)
		*y = 15;
}

void UI::GUIGroupBox::GetClientSize(Double *w, Double *h)
{
	Double width;
	Double height;
	this->GetSize(&width, &height);
	if (w)
		*w = width - 6;
	if (h)
		*h = height - 18;
}

Bool UI::GUIGroupBox::IsChildVisible()
{
	return true;
}

UI::GUIGroupBox::~GUIGroupBox()
{
	UI::GUICoreWin::MSSetWindowObj(this->hwnd, GWLP_WNDPROC, (OSInt)this->oriWndProc);
}

Text::CString UI::GUIGroupBox::GetObjectClass()
{
	return CSTR("GroupBox");
}

OSInt UI::GUIGroupBox::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}


void UI::GUIGroupBox::OnSizeChanged(Bool updateScn)
{
	this->UpdateChildrenSize(updateScn);
	UOSInt i = this->resizeHandlers->GetCount();
	while (i-- > 0)
	{
		this->resizeHandlers->GetItem(i)(this->resizeHandlersObjs->GetItem(i));
	}
}
