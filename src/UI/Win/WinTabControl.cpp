#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "UI/GUITabPage.h"
#include "UI/Win/WinCore.h"
#include "UI/Win/WinTabControl.h"
#include <commctrl.h>

#ifdef _WIN32_WCE
#define GWLP_WNDPROC GWL_WNDPROC
#define WS_EX_CONTROLPARENT 0
#endif

#ifndef GWL_USERDATA
#define GWL_USERDATA GWLP_USERDATA
#endif

OSInt __stdcall UI::Win::WinTabControl::TCWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam)
{
	UI::Win::WinTabControl *me = (UI::Win::WinTabControl*)UI::Win::WinCore::MSGetWindowObj((ControlHandle*)hWnd, GWL_USERDATA);
	UI::GUIControl*ctrl;
	NMHDR *nmhdr;
	switch (msg)
	{
	case WM_COMMAND:
		ctrl = (UI::GUIControl*)UI::Win::WinCore::MSGetWindowObj((ControlHandle*)lParam, GWL_USERDATA);
		if (ctrl)
		{
			ctrl->OnNotify(HIWORD(wParam), 0);
		}
		break;
	case WM_NOTIFY:
		nmhdr = (NMHDR*)lParam;
		ctrl = (UI::GUIControl*)UI::Win::WinCore::MSGetWindowObj((ControlHandle*)nmhdr->hwndFrom, GWL_USERDATA);
		if (ctrl)
		{
			ctrl->OnNotify(nmhdr->code, (void*)lParam);
		}
		break;
	case WM_ERASEBKGND:
		{
			RECT rc;
			Math::Size2D<UOSInt> sz;
			Math::RectArea<OSInt> tpRect = me->GetTabPageRect();
			sz = me->GetSizeP();
			rc.left = 0;
			rc.top = 0;
			rc.right = (LONG)sz.x;
			rc.bottom = (LONG)tpRect.min.y;
			FillRect((HDC)wParam, &rc, me->hbrBackground);
			rc.left = 0;
			rc.top = (LONG)tpRect.min.y;
			rc.right = (LONG)tpRect.min.x;
			rc.bottom = (LONG)tpRect.max.y;
			FillRect((HDC)wParam, &rc, me->hbrBackground);
			rc.left = (LONG)tpRect.max.x;
			rc.top = (LONG)tpRect.min.y;
			rc.right = (LONG)sz.x;
			rc.bottom = (LONG)tpRect.max.y;
			FillRect((HDC)wParam, &rc, me->hbrBackground);
			rc.left = 0;
			rc.top = (LONG)tpRect.max.y;
			rc.right = (LONG)sz.x;
			rc.bottom = (LONG)sz.y;
			FillRect((HDC)wParam, &rc, me->hbrBackground);
		}
		return 0;
	default:
		return CallWindowProc(me->oriWndProc, (HWND)hWnd, msg, wParam, lParam);
	}
	return 0;
}

UI::Win::WinTabControl::WinTabControl(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent) : UI::GUITabControl(ui, parent)
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
	this->InitControl(((UI::Win::WinCore*)ui.Ptr())->GetHInst(), parent, WC_TABCONTROLW, (const UTF8Char*)"", style, WS_EX_CONTROLPARENT, 0, 0, sz.x, sz.y);
	this->oriWndProc = (WNDPROC)UI::Win::WinCore::MSSetWindowObj(this->hwnd, GWLP_WNDPROC, (OSInt)TCWndProc);
	this->selIndex = 0;

	WNDCLASSW wc;
	GetClassInfoW((HINSTANCE)((UI::Win::WinCore*)ui.Ptr())->GetHInst(), WC_TABCONTROLW, &wc);
	this->hbrBackground = wc.hbrBackground;
}

UI::Win::WinTabControl::~WinTabControl()
{
	UI::Win::WinCore::MSSetWindowObj(this->hwnd, GWLP_WNDPROC, (OSInt)this->oriWndProc);
	this->tabPages.DeleteAll();
}

NotNullPtr<UI::GUITabPage> UI::Win::WinTabControl::AddTabPage(NotNullPtr<Text::String> tabName)
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
	Math::RectArea<OSInt> rect = GetTabPageRect();
	page->SetAreaP(rect.min.x, rect.min.y, rect.max.x, rect.max.y, false);
	if (this->tabPages.GetCount() > 1)
	{
		page->SetVisible(false);
	}
	return page;
}

NotNullPtr<UI::GUITabPage> UI::Win::WinTabControl::AddTabPage(Text::CStringNN tabName)
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
	Math::RectArea<OSInt> rect = GetTabPageRect();
	page->SetAreaP(rect.min.x, rect.min.y, rect.max.x, rect.max.y, false);
	if (this->tabPages.GetCount() > 1)
	{
		page->SetVisible(false);
	}
	return page;
}

void UI::Win::WinTabControl::SetSelectedIndex(UOSInt index)
{
	if (this->selIndex != index)
	{
		NotNullPtr<UI::GUITabPage> page;
		if (this->tabPages.GetItem(this->selIndex).SetTo(page))
			page->SetVisible(false);
		this->selIndex = index;
		SendMessage((HWND)this->hwnd, TCM_SETCURSEL, index, 0);

		this->EventSelChange();
		if (this->tabPages.GetItem(this->selIndex).SetTo(page))
		{
			page->SetVisible(true);
			InvalidateRect((HWND)this->hwnd, 0, false);
		}
	}
}

void UI::Win::WinTabControl::SetSelectedPage(NotNullPtr<UI::GUITabPage> page)
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

UOSInt UI::Win::WinTabControl::GetSelectedIndex()
{
	return this->selIndex;
}

Optional<UI::GUITabPage> UI::Win::WinTabControl::GetSelectedPage()
{
	return this->tabPages.GetItem(this->selIndex);
}

void UI::Win::WinTabControl::SetTabPageName(UOSInt index, Text::CStringNN name)
{
	TCITEMW item;
	item.mask = TCIF_TEXT;
	item.pszText = (LPWSTR)Text::StrToWCharNew(name.v);
	item.cchTextMax = 0;
	index = (UOSInt)SendMessageW((HWND)this->hwnd, TCM_SETITEMW, index, (LPARAM)&item);
	Text::StrDelNew((const WChar*)item.pszText);
}

UTF8Char *UI::Win::WinTabControl::GetTabPageName(UOSInt index, UTF8Char *buff)
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

Math::RectArea<OSInt> UI::Win::WinTabControl::GetTabPageRect()
{
	RECT rc;
	RECT rcTc;
	rc.left = 0;
	rc.top = 0;
	rc.right = 0;
	rc.bottom = 0;
	SendMessageW((HWND)this->hwnd, TCM_ADJUSTRECT, FALSE, (LPARAM)&rc);
	GetClientRect((HWND)this->hwnd, &rcTc);
	return Math::RectArea<OSInt>(rc.left, rc.top, rcTc.right + rc.right - rc.left, rcTc.bottom + rc.bottom - rc.top);
}

OSInt UI::Win::WinTabControl::OnNotify(UInt32 code, void *lParam)
{
	UOSInt newIndex;
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
			this->EventSelChange();
		}
		return 0;
	}
	return 0;
}

void UI::Win::WinTabControl::OnSizeChanged(Bool updateScn)
{
	UOSInt i = this->resizeHandlers.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<UI::UIEvent> cb = this->resizeHandlers.GetItem(i);
		cb.func(cb.userObj);
	}
	Math::RectArea<OSInt> rect = GetTabPageRect();

	Data::ArrayIterator<NotNullPtr<GUITabPage>> it = this->tabPages.Iterator();
	while (it.HasNext())
	{
		it.Next()->SetAreaP(rect.min.x, rect.min.y, rect.max.x, rect.max.y, false);
//		this->tabPages->GetItem(i)->UpdateChildrenSize(false);
	}
}

void UI::Win::WinTabControl::SetDPI(Double hdpi, Double ddpi)
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

	Math::RectArea<OSInt> rect = GetTabPageRect();
	it = this->tabPages.Iterator();
	while (it.HasNext())
	{
		it.Next()->SetAreaP(rect.min.x, rect.min.y, rect.max.x, rect.max.y, false);
	}
}
