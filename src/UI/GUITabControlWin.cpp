#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "UI/GUICoreWin.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include <windows.h>
#include <commctrl.h>

#ifdef _WIN32_WCE
#define GWLP_WNDPROC GWL_WNDPROC
#define WS_EX_CONTROLPARENT 0
#endif

#ifndef GWL_USERDATA
#define GWL_USERDATA GWLP_USERDATA
#endif

OSInt __stdcall UI::GUITabControl::TCWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam)
{
	UI::GUITabControl *me = (UI::GUITabControl*)UI::GUICoreWin::MSGetWindowObj((ControlHandle*)hWnd, GWL_USERDATA);
	UI::GUIControl*ctrl;
	NMHDR *nmhdr;
	switch (msg)
	{
	case WM_COMMAND:
		ctrl = (UI::GUIControl*)UI::GUICoreWin::MSGetWindowObj((ControlHandle*)lParam, GWL_USERDATA);
		if (ctrl)
		{
			ctrl->OnNotify(HIWORD(wParam), 0);
		}
		break;
	case WM_NOTIFY:
		nmhdr = (NMHDR*)lParam;
		ctrl = (UI::GUIControl*)UI::GUICoreWin::MSGetWindowObj((ControlHandle*)nmhdr->hwndFrom, GWL_USERDATA);
		if (ctrl)
		{
			ctrl->OnNotify(nmhdr->code, (void*)lParam);
		}
		break;
	case WM_ERASEBKGND:
		{
			RECT rc;
			OSInt tcLeft;
			OSInt tcTop;
			UOSInt tcWidth;
			UOSInt tcHeight;
			Math::Size2D<UOSInt> sz;
			me->GetTabPageRect(&tcLeft, &tcTop, &tcWidth, &tcHeight);
			sz = me->GetSizeP();
			rc.left = 0;
			rc.top = 0;
			rc.right = (LONG)sz.x;
			rc.bottom = (LONG)tcTop;
			FillRect((HDC)wParam, &rc, (HBRUSH)me->hbrBackground);
			rc.left = 0;
			rc.top = (LONG)tcTop;
			rc.right = (LONG)tcLeft;
			rc.bottom = (LONG)(tcTop + (OSInt)tcHeight);
			FillRect((HDC)wParam, &rc, (HBRUSH)me->hbrBackground);
			rc.left = (LONG)(tcLeft + (OSInt)tcWidth);
			rc.top = (LONG)tcTop;
			rc.right = (LONG)sz.x;
			rc.bottom = (LONG)(tcTop + (OSInt)tcHeight);
			FillRect((HDC)wParam, &rc, (HBRUSH)me->hbrBackground);
			rc.left = 0;
			rc.top = (LONG)(tcTop + (OSInt)tcHeight);
			rc.right = (LONG)sz.x;
			rc.bottom = (LONG)sz.y;
			FillRect((HDC)wParam, &rc, (HBRUSH)me->hbrBackground);
		}
		return 0;
	default:
		return CallWindowProc((WNDPROC)me->oriWndProc, (HWND)hWnd, msg, wParam, lParam);
	}
	return 0;
}

UI::GUITabControl::GUITabControl(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent) : UI::GUIControl(ui, parent)
{
    INITCOMMONCONTROLSEX icex;

	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_TAB_CLASSES;
    InitCommonControlsEx(&icex);

	UInt32 style = WS_CLIPSIBLINGS | WS_CHILD | WS_TABSTOP;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	Math::Size2DDbl sz = parent->GetClientSize();
	this->InitControl(((UI::GUICoreWin*)ui.Ptr())->GetHInst(), parent, WC_TABCONTROLW, (const UTF8Char*)"", style, WS_EX_CONTROLPARENT, 0, 0, sz.x, sz.y);
	this->oriWndProc = (void*)UI::GUICoreWin::MSSetWindowObj(this->hwnd, GWLP_WNDPROC, (OSInt)TCWndProc);
	this->selIndex = 0;

	WNDCLASSW wc;
	GetClassInfoW((HINSTANCE)((UI::GUICoreWin*)ui.Ptr())->GetHInst(), WC_TABCONTROLW, &wc);
	this->hbrBackground = wc.hbrBackground;
}

UI::GUITabControl::~GUITabControl()
{
	UI::GUICoreWin::MSSetWindowObj(this->hwnd, GWLP_WNDPROC, (OSInt)this->oriWndProc);
	this->tabPages.DeleteAll();
}

NotNullPtr<UI::GUITabPage> UI::GUITabControl::AddTabPage(NotNullPtr<Text::String> tabName)
{
	UOSInt index;
	TCITEMW item;
	item.mask = TCIF_TEXT;
	item.pszText = (LPWSTR)Text::StrToWCharNew(tabName->v);
	item.cchTextMax = 0;
	index = (UOSInt)SendMessageW((HWND)this->hwnd, TCM_INSERTITEMW, this->tabPages.GetCount(), (LPARAM)&item);
	Text::StrDelNew((const WChar*)item.pszText);
	NotNullPtr<UI::GUITabPage> page;
//		NEW_CLASS(page, UI::GUITabPage(this, index));
	NEW_CLASSNN(page, UI::GUITabPage(this->ui, 0, *this, index));
	page->SetDPI(this->hdpi, this->ddpi);
	this->tabPages.Add(page);
	OSInt x;
	OSInt y;
	UOSInt w;
	UOSInt h;
	GetTabPageRect(&x, &y, &w, &h);
	page->SetAreaP(x, y, x + (OSInt)w, y + (OSInt)h, false);
	if (this->tabPages.GetCount() > 1)
	{
		page->SetVisible(false);
	}
	return page;
}

NotNullPtr<UI::GUITabPage> UI::GUITabControl::AddTabPage(Text::CStringNN tabName)
{
	UOSInt index;
	TCITEMW item;
	item.mask = TCIF_TEXT;
	item.pszText = (LPWSTR)Text::StrToWCharNew(tabName.v);
	item.cchTextMax = 0;
	index = (UOSInt)SendMessageW((HWND)this->hwnd, TCM_INSERTITEMW, this->tabPages.GetCount(), (LPARAM)&item);
	Text::StrDelNew((const WChar*)item.pszText);
	NotNullPtr<UI::GUITabPage> page;
//		NEW_CLASS(page, UI::GUITabPage(this, index));
	NEW_CLASSNN(page, UI::GUITabPage(this->ui, 0, *this, index));
	page->SetDPI(this->hdpi, this->ddpi);
	this->tabPages.Add(page);
	OSInt x;
	OSInt y;
	UOSInt w;
	UOSInt h;
	GetTabPageRect(&x, &y, &w, &h);
	page->SetAreaP(x, y, x + (OSInt)w, y + (OSInt)h, false);
	if (this->tabPages.GetCount() > 1)
	{
		page->SetVisible(false);
	}
	return page;
}

void UI::GUITabControl::SetSelectedIndex(UOSInt index)
{
	if (this->selIndex != index)
	{
		NotNullPtr<UI::GUITabPage> page;
		if (this->tabPages.GetItem(this->selIndex).SetTo(page))
			page->SetVisible(false);
		this->selIndex = index;
		SendMessage((HWND)this->hwnd, TCM_SETCURSEL, index, 0);

		UOSInt i;
		i = this->selChgHdlrs.GetCount();
		while (i-- > 0)
		{
			this->selChgHdlrs.GetItem(i)(this->selChgObjs.GetItem(i));
		}
		if (this->tabPages.GetItem(this->selIndex).SetTo(page))
		{
			page->SetVisible(true);
			InvalidateRect((HWND)this->hwnd, 0, false);
		}
	}
}

void UI::GUITabControl::SetSelectedPage(NotNullPtr<UI::GUITabPage> page)
{
	UOSInt i = this->tabPages.GetCount();
	while (i-- > 0)
	{
		if (page.Ptr() == this->tabPages.GetItem(i).OrNull())
		{
			SetSelectedIndex(i);
		}
	}
}

UOSInt UI::GUITabControl::GetSelectedIndex()
{
	return this->selIndex;
}

Optional<UI::GUITabPage> UI::GUITabControl::GetSelectedPage()
{
	return this->tabPages.GetItem(this->selIndex);
}

void UI::GUITabControl::SetTabPageName(UOSInt index, Text::CStringNN name)
{
	TCITEMW item;
	item.mask = TCIF_TEXT;
	item.pszText = (LPWSTR)Text::StrToWCharNew(name.v);
	item.cchTextMax = 0;
	index = (UOSInt)SendMessageW((HWND)this->hwnd, TCM_SETITEMW, index, (LPARAM)&item);
	Text::StrDelNew((const WChar*)item.pszText);
}

UTF8Char *UI::GUITabControl::GetTabPageName(UOSInt index, UTF8Char *buff)
{
	WChar wbuff[512];
	TCITEMW item;
	item.mask = TCIF_TEXT;
	item.pszText = (LPWSTR)wbuff;
	item.cchTextMax = 512;
	wbuff[0] = 0;
	index = (UOSInt)SendMessageW((HWND)this->hwnd, TCM_GETITEMW, index, (LPARAM)&item);
	return Text::StrWChar_UTF8(buff, wbuff);
}

void UI::GUITabControl::GetTabPageRect(OSInt *x, OSInt *y, UOSInt *w, UOSInt *h)
{
	RECT rc;
	RECT rcTc;
	rc.left = 0;
	rc.top = 0;
	rc.right = 0;
	rc.bottom = 0;
	SendMessageW((HWND)this->hwnd, TCM_ADJUSTRECT, FALSE, (LPARAM)&rc);
	GetClientRect((HWND)this->hwnd, &rcTc);
	if (x)
		*x = rc.left;
	if (y)
		*y = rc.top;
	if (w)
		*w = (UOSInt)(rcTc.right + rc.right - rc.left);
	if (h)
		*h = (UOSInt)(rcTc.bottom + rc.bottom - rc.top);
}

void *UI::GUITabControl::GetTabPageFont()
{
	return this->GetFont();
}

Text::CStringNN UI::GUITabControl::GetObjectClass() const
{
	return CSTR("TabControl");
}

OSInt UI::GUITabControl::OnNotify(UInt32 code, void *lParam)
{
	UOSInt newIndex;
	UOSInt i;
	NotNullPtr<UI::GUITabPage> tp;
	switch (code)
	{
	case TCN_SELCHANGE:
		newIndex = (UOSInt)SendMessage((HWND)this->hwnd, TCM_GETCURSEL, 0, 0);
		if (newIndex != this->selIndex)
		{
			if (this->tabPages.GetItem(this->selIndex).SetTo(tp))
				tp->SetVisible(false);
			this->selIndex = newIndex;
			if (this->tabPages.GetItem(this->selIndex).SetTo(tp))
				tp->SetVisible(true);
			InvalidateRect((HWND)this->hwnd, 0, false);
			
			i = this->selChgHdlrs.GetCount();
			while (i-- > 0)
			{
				this->selChgHdlrs.GetItem(i)(this->selChgObjs.GetItem(i));
			}
		}
		return 0;
	}
	return 0;
}

void UI::GUITabControl::OnSizeChanged(Bool updateScn)
{
	UOSInt i = this->resizeHandlers.GetCount();
	while (i-- > 0)
	{
		this->resizeHandlers.GetItem(i)(this->resizeHandlersObjs.GetItem(i));
	}
	OSInt x;
	OSInt y;
	UOSInt w;
	UOSInt h;
	GetTabPageRect(&x, &y, &w, &h);

	Data::ArrayIterator<NotNullPtr<GUITabPage>> it = this->tabPages.Iterator();
	while (it.HasNext())
	{
		it.Next()->SetAreaP(x, y, x + (OSInt)w, y + (OSInt)h, false);
//		this->tabPages->GetItem(i)->UpdateChildrenSize(false);
	}
}

void UI::GUITabControl::HandleSelChanged(UIEvent hdlr, void *userObj)
{
	this->selChgHdlrs.Add(hdlr);
	this->selChgObjs.Add(userObj);
}

void UI::GUITabControl::SetDPI(Double hdpi, Double ddpi)
{
	this->hdpi = hdpi;
	this->ddpi = ddpi;
	this->UpdatePos(true);
	if (this->hFont)
	{
		this->InitFont();
	}
	else
	{
		this->UpdateFont();
	}

	Data::ArrayIterator<NotNullPtr<GUITabPage>> it = this->tabPages.Iterator();
	while (it.HasNext())
	{
		it.Next()->SetDPI(hdpi, ddpi);
	}

	OSInt x;
	OSInt y;
	UOSInt w;
	UOSInt h;
	GetTabPageRect(&x, &y, &w, &h);

	it = this->tabPages.Iterator();
	while (it.HasNext())
	{
		it.Next()->SetAreaP(x, y, x + (OSInt)w, y + (OSInt)h, false);
	}
}
