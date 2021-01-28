#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Math/Math.h"
#include "Media/GDIEngine.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "UI/GUIClientControl.h"
#include "UI/GUIControl.h"
#include "UI/GUICoreWin.h"
#include "UI/GUIWindowDragDrop.h"
#include <windows.h>
#undef GetForm
#undef GetMonitorInfo
#undef CrateFontW

#ifndef GWL_USERDATA
#define GWL_USERDATA GWLP_USERDATA
#endif

#ifndef GCL_HCURSOR
#define GCL_HCURSOR GCLP_HCURSOR
#endif

#if defined(_WIN32_WCE)
#define GetWindowLongPtr(a, b) GetWindowLongW(a, b)
#define SetWindowLongPtr(a, b, c) SetWindowLongW(a, b, c)
#endif

void UI::GUIControl::InitControl(void *hInst, void *parentHWnd, const WChar *className, const UTF8Char *txt, Int32 style, Int32 exStyle, Double x, Double y, Double w, Double h)
{
	HDC hdc = GetDC((HWND)parentHWnd);
	this->hdpi = GetDeviceCaps(hdc, LOGPIXELSY);
	this->ddpi = 96.0;
	ReleaseDC((HWND)parentHWnd, hdc);

	const WChar *wptr = Text::StrToWCharNew(txt);
	this->hwnd = CreateWindowExW(exStyle, className, wptr, style, Math::Double2Int32(x * this->hdpi / this->ddpi), Math::Double2Int32(y * this->hdpi / this->ddpi), Math::Double2Int32(w * this->hdpi / this->ddpi), Math::Double2Int32(h * this->hdpi / this->ddpi), (HWND)parentHWnd, 0, (HINSTANCE)hInst, 0);
	Text::StrDelNew(wptr);
	this->lxPos = x;
	this->lyPos = y;
	this->lxPos2 = x + w;
	this->lyPos2 = y + h;
	this->currHMon = this->GetHMonitor();
	UI::GUICoreWin::MSSetWindowObj(hwnd, GWL_USERDATA, (OSInt)this);
	UpdateFont();
}

void UI::GUIControl::InitControl(void *hInst, UI::GUIClientControl *parent, const WChar *className, const UTF8Char *txt, Int32 style, Int32 exStyle, Double x, Double y, Double w, Double h)
{
	this->fontHeight = 0.0;
	if (parent)
	{
		Double xOfst = 0;
		Double yOfst = 0;
		parent->GetClientOfst(&xOfst, &yOfst);
		this->hdpi = parent->GetHDPI();
		this->ddpi = parent->GetDDPI();
		if (txt == 0)
		{
			this->hwnd = CreateWindowExW(exStyle, className, 0, style, Math::Double2Int32((x + xOfst) * this->hdpi / this->ddpi), Math::Double2Int32((y + yOfst) * this->hdpi / this->ddpi), Math::Double2Int32(w * this->hdpi / this->ddpi), Math::Double2Int32(h * this->hdpi / this->ddpi), (HWND)parent->GetHandle(), 0, (HINSTANCE)hInst, 0);
		}
		else
		{
			const WChar *wptr = Text::StrToWCharNew(txt);
			this->hwnd = CreateWindowExW(exStyle, className, wptr, style, Math::Double2Int32((x + xOfst) * this->hdpi / this->ddpi), Math::Double2Int32((y + yOfst) * this->hdpi / this->ddpi), Math::Double2Int32(w * this->hdpi / this->ddpi), Math::Double2Int32(h * this->hdpi / this->ddpi), (HWND)parent->GetHandle(), 0, (HINSTANCE)hInst, 0);
			Text::StrDelNew(wptr);
		}
	}
	else
	{
		HDC hdc = GetDC((HWND)this->hwnd);
		if (txt == 0)
		{
			this->hwnd = CreateWindowExW(exStyle, className, 0, style, Math::Double2Int32(x), Math::Double2Int32(y), Math::Double2Int32(w), Math::Double2Int32(h), 0, 0, (HINSTANCE)hInst, 0);
		}
		else
		{
			const WChar *wptr = Text::StrToWCharNew(txt);
			this->hwnd = CreateWindowExW(exStyle, className, wptr, style, Math::Double2Int32(x), Math::Double2Int32(y), Math::Double2Int32(w), Math::Double2Int32(h), 0, 0, (HINSTANCE)hInst, 0);
			Text::StrDelNew(wptr);
		}
		ReleaseDC((HWND)this->hwnd, hdc);
		
	}
	this->lxPos = x;
	this->lyPos = y;
	this->lxPos2 = x + w;
	this->lyPos2 = y + h;
	UI::GUICoreWin::MSSetWindowObj(hwnd, GWL_USERDATA, (OSInt)this);
	this->inited = true;
	UpdateFont();
//	UpdateBGColor();
}

UI::GUIControl::GUIControl(GUICore *ui, UI::GUIClientControl *parent)
{
	NEW_CLASS(this->resizeHandlers, Data::ArrayList<UIEvent>());
	NEW_CLASS(this->resizeHandlersObjs, Data::ArrayList<void*>());
	this->dropHdlr = 0;
	this->inited = false;
	this->selfResize = false;
	this->hwnd = 0;
	this->hFont = 0;
	this->hbrBackground = 0;
	this->parent = parent;
	this->ui = ui;
	this->dockType = UI::GUIControl::DOCK_NONE;
	this->fontName = 0;
	this->fontHeight = 0;
	this->fontIsBold = false;
	if (parent)
	{
		this->hdpi = parent->GetHDPI();
		this->ddpi = parent->GetDDPI();
		parent->AddChild(this);
	}
	else
	{
		this->hdpi = 96.0;
		this->ddpi = 96.0;
	}
	this->currHMon = MonitorFromWindow((HWND)this->hwnd, MONITOR_DEFAULTTONEAREST);
}

UI::GUIControl::~GUIControl()
{
	if (this->inited)
	{
		SetWindowLongPtr((HWND)hwnd, GWL_USERDATA, 0);
	}
	DEL_CLASS(this->resizeHandlers);
	DEL_CLASS(this->resizeHandlersObjs);
	if (this->hFont)
	{
		DeleteObject(hFont);
		hFont = 0;
	}
	SDEL_TEXT(this->fontName);
	if (this->hbrBackground)
	{
		DeleteObject((HBRUSH)this->hbrBackground);
	}
	if (this->dropHdlr)
	{
		UI::GUIWindowDragDrop *dragDrop = (UI::GUIWindowDragDrop *)this->dropHdlr;
		DEL_CLASS(dragDrop);
#if !defined(_WIN32_WCE)
		OleUninitialize();
#endif
	}
}

void *UI::GUIControl::GetHInst()
{
	return ((GUICoreWin*)ui)->GetHInst();
}

void *UI::GUIControl::GetFont()
{
	if (hFont)
	{
		return hFont;
	}
	else if (this->parent)
	{
		return this->parent->GetFont();
	}
	else
	{
		return 0;
	}
}

void UI::GUIControl::Show()
{
	ShowWindow((HWND)hwnd, SW_SHOW);
	this->OnShow();
}

void UI::GUIControl::Close()
{
	DestroyWindow((HWND)hwnd);
}

void UI::GUIControl::SetText(const UTF8Char *text)
{
	const WChar *wptr = Text::StrToWCharNew(text);
	SetWindowTextW((HWND)hwnd, wptr);
	Text::StrDelNew(wptr);
}

UTF8Char *UI::GUIControl::GetText(UTF8Char *buff)
{
	WChar sbuff[256];
	sbuff[0] = 0;
	GetWindowTextW((HWND)hwnd, sbuff, 256);
	return Text::StrWChar_UTF8(buff, sbuff, -1);
}

Bool UI::GUIControl::GetText(Text::StringBuilderUTF *sb)
{
	OSInt leng = GetWindowTextLengthW((HWND)hwnd);
	WChar *wptr = MemAlloc(WChar, leng + 1);
	GetWindowTextW((HWND)hwnd, wptr, (int)leng + 1);
	const UTF8Char *csptr = Text::StrToUTF8New(wptr);
	sb->Append(csptr);
	Text::StrDelNew(csptr);
	MemFree(wptr);
	return true;
}

void UI::GUIControl::SetSize(Double width, Double height)
{
	this->SetArea(this->lxPos, this->lyPos, this->lxPos + width, this->lyPos + height, true);
}

void UI::GUIControl::SetSizeP(UOSInt width, UOSInt height)
{
	this->SetArea(this->lxPos, this->lyPos, this->lxPos + width * this->ddpi / this->hdpi, this->lyPos + height * this->ddpi / this->hdpi, true);
}

void UI::GUIControl::GetSize(Double *width, Double *height)
{
	if (width)
		*width = this->lxPos2 - this->lxPos;
	if (height)
		*height = this->lyPos2 - this->lyPos;
}

void UI::GUIControl::GetSizeP(UOSInt *width, UOSInt *height)
{
	if (width)
		*width = Math::Double2Int32((this->lxPos2 - this->lxPos) * this->hdpi / this->ddpi);
	if (height)
		*height = Math::Double2Int32((this->lyPos2 - this->lyPos) * this->hdpi / this->ddpi);
}

void UI::GUIControl::SetPosition(Double x, Double y)
{
	SetArea(x, y, x + this->lxPos2 - this->lxPos, y + this->lyPos2 - this->lyPos, true);
}

void UI::GUIControl::GetPositionP(OSInt *x, OSInt *y)
{
	if (x)
		*x = Math::Double2Int32(this->lxPos * this->hdpi / this->ddpi);
	if (y)
		*y = Math::Double2Int32(this->lyPos * this->hdpi / this->ddpi);
}

void UI::GUIControl::GetScreenPosP(OSInt *x, OSInt *y)
{
	RECT rc;
	GetWindowRect((HWND)hwnd, &rc);
	if (x)
		*x = rc.left;
	if (y)
		*y = rc.top;
}

void UI::GUIControl::SetArea(Double left, Double top, Double right, Double bottom, Bool updateScn)
{
	if (left == this->lxPos && top == this->lyPos && right == this->lxPos2 && bottom == this->lyPos2)
		return;
	Double xOfst = 0;
	Double yOfst = 0;
	if (this->parent)
	{
		this->parent->GetClientOfst(&xOfst, &yOfst);
	}
	this->lxPos = left;
	this->lyPos = top;
	this->selfResize = true;
	MoveWindow((HWND)hwnd, Math::Double2Int32((left + xOfst) * this->hdpi / this->ddpi), Math::Double2Int32((top + yOfst) * this->hdpi / this->ddpi), Math::Double2Int32((right - left) * this->hdpi / this->ddpi), Math::Double2Int32((bottom - top) * this->hdpi / this->ddpi), updateScn?TRUE:FALSE);
	RECT rect;
	GetWindowRect((HWND)hwnd, &rect);
	this->lxPos2 = left + (rect.right - rect.left) * this->ddpi / this->hdpi;
	this->lyPos2 = top + (rect.bottom - rect.top) * this->ddpi / this->hdpi;
	this->selfResize = false;
	this->OnSizeChanged(updateScn);
}

void UI::GUIControl::SetAreaP(OSInt left, OSInt top, OSInt right, OSInt bottom, Bool updateScn)
{
	Double xOfst = 0;
	Double yOfst = 0;
	if (this->parent)
	{
		this->parent->GetClientOfst(&xOfst, &yOfst);
	}
	this->lxPos = left * this->ddpi / this->hdpi;
	this->lyPos = top * this->ddpi / this->hdpi;
	this->selfResize = true;
	MoveWindow((HWND)hwnd, Math::Double2Int32(left + xOfst * this->hdpi / this->ddpi), Math::Double2Int32(top + yOfst * this->hdpi / this->ddpi), (int)(right - left), (int)(bottom - top), updateScn?TRUE:FALSE);
	RECT rect;
	GetWindowRect((HWND)hwnd, &rect);
	this->lxPos2 = (left + rect.right - rect.left) * this->ddpi / this->hdpi;
	this->lyPos2 = (top + rect.bottom - rect.top) * this->ddpi / this->hdpi;
	this->selfResize = false;
	this->OnSizeChanged(updateScn);
}

void UI::GUIControl::SetRect(Double left, Double top, Double width, Double height, Bool updateScn)
{
	SetArea(left, top, left + width, top + height, updateScn);
}

void UI::GUIControl::SetFont(const UTF8Char *name, Double size, Bool isBold)
{
	SDEL_TEXT(this->fontName);
	if (name)
	{
		this->fontName = Text::StrCopyNew(name);
	}
	this->fontHeight = size;
	this->fontIsBold = isBold;
	InitFont();
}

void UI::GUIControl::InitFont()
{
	if (this->fontHeight <= 0)
		return;

	LOGFONTW lf;
	MemClear(&lf, sizeof(LOGFONTW));
	if (this->fontName)
	{
		Text::StrUTF8_WChar(lf.lfFaceName, this->fontName, -1, 0);
	}
	lf.lfHeight = Math::Double2Int32(this->fontHeight * this->hdpi / this->ddpi / -0.75);
	if (this->fontIsBold)
	{
		lf.lfWeight = FW_BOLD;
	}
	HFONT hFont = CreateFontIndirectW(&lf);
	if (hFont)
	{
		if (this->hFont)
		{
			DeleteObject(this->hFont);
		}
		this->hFont = hFont;
		UpdateFont();
		SendMessage((HWND)this->hwnd, WM_SETFONT, (WPARAM)hFont, TRUE);
	}
}

void UI::GUIControl::SetDockType(UI::GUIControl::DockType dockType)
{
	if (this->dockType != dockType)
	{
		this->dockType = dockType;
		if (this->parent)
		{
			this->parent->UpdateChildrenSize(true);
		}
	}
}

UI::GUIControl::DockType UI::GUIControl::GetDockType()
{
	return this->dockType;
}

void UI::GUIControl::SetVisible(Bool isVisible)
{
#if defined(_WIN32_WCE)
	ShowWindow((HWND)this->hwnd, isVisible?SW_SHOW:SW_HIDE);
	if (this->dockType != UI::GUIControl::DOCK_NONE)
	{
		if (this->parent)
		{
			this->parent->UpdateChildrenSize(true);
		}
	}
#else
	Bool changed = false;
	Int32 ws = GetWindowLong((HWND)this->hwnd, GWL_STYLE);
	if (isVisible)
	{
		if ((ws & WS_VISIBLE) == 0)
		{
			ws |= WS_VISIBLE;
			SetWindowLong((HWND)this->hwnd, GWL_STYLE, ws);
			InvalidateRect((HWND)this->hwnd, 0, false);
			changed = true;
		}
	}
	else
	{
		if (ws & WS_VISIBLE)
		{
			ws &= ~WS_VISIBLE;
			SetWindowLong((HWND)this->hwnd, GWL_STYLE, ws);
			InvalidateRect((HWND)this->hwnd, 0, false);
			changed = true;
		}
	}
	if (changed && this->dockType != UI::GUIControl::DOCK_NONE)
	{
		if (this->parent)
		{
			this->parent->UpdateChildrenSize(true);
		}
	}
#endif
}

Bool UI::GUIControl::GetVisible()
{
	Int32 ws = GetWindowLong((HWND)this->hwnd, GWL_STYLE);
	return (ws & WS_VISIBLE) != 0;
}

void UI::GUIControl::SetEnabled(Bool isEnable)
{
	if (this->hwnd)
	{
		EnableWindow((HWND)this->hwnd, isEnable?TRUE:FALSE);
	}
}

void UI::GUIControl::SetBGColor(Int32 bgColor)
{
	HBRUSH hbr = CreateSolidBrush(((bgColor & 0xff) << 16) | (bgColor & 0xff00) | ((bgColor & 0xff0000) >> 16));
	if (this->hbrBackground)
	{
		DeleteObject((HBRUSH)this->hbrBackground);
	}
	this->hbrBackground = hbr;
	UpdateBGColor();
}

void *UI::GUIControl::GetBGBrush()
{
	if (this->hbrBackground)
		return this->hbrBackground;
	if (this->parent)
		return this->parent->GetBGBrush();
	return 0;
}

Bool UI::GUIControl::IsFormFocused()
{
	if (this->parent)
	{
		return this->parent->IsFormFocused();
	}
	else
	{
		return this->hwnd == GetActiveWindow();
	}
}

void UI::GUIControl::Focus()
{
	SetFocus((HWND)this->hwnd);
}

OSInt UI::GUIControl::GetScrollHPos()
{
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_POS;
	if (GetScrollInfo((HWND)this->hwnd, SB_HORZ, &si))
	{
		return si.nPos;
	}
	return 0;
}

OSInt UI::GUIControl::GetScrollVPos()
{
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_POS;
	if (GetScrollInfo((HWND)this->hwnd, SB_VERT, &si))
	{
		return si.nPos;
	}
	return 0;
}

void UI::GUIControl::ScrollTo(OSInt x, OSInt y)
{
	SetScrollPos((HWND)this->hwnd, SB_HORZ, (int)x, TRUE);
	SetScrollPos((HWND)this->hwnd, SB_VERT, (int)y, TRUE);
}

void UI::GUIControl::OnSizeChanged(Bool updateScn)
{
	if (this->selfResize)
	{
		return;
	}
	HMONITOR hMon = MonitorFromWindow((HWND)this->hwnd, MONITOR_DEFAULTTONEAREST);
	if (hMon != this->currHMon)
	{
		this->currHMon = hMon;
		this->OnMonitorChanged();
	}
	OSInt i = this->resizeHandlers->GetCount();
	while (i-- > 0)
	{
		this->resizeHandlers->GetItem(i)(this->resizeHandlersObjs->GetItem(i));
	}
}

void UI::GUIControl::OnPosChanged(Bool updateScn)
{
	HMONITOR hMon = MonitorFromWindow((HWND)this->hwnd, MONITOR_DEFAULTTONEAREST);
	if (hMon != this->currHMon)
	{
		this->currHMon = hMon;
		this->OnMonitorChanged();
	}
}

void UI::GUIControl::OnMonitorChanged()
{
}

void UI::GUIControl::OnShow()
{
}

void UI::GUIControl::HandleSizeChanged(UIEvent handler, void *userObj)
{
	this->resizeHandlers->Add(handler);
	this->resizeHandlersObjs->Add(userObj);
}

void UI::GUIControl::UpdateFont()
{
	void *font = GetFont();
	if (font)
	{
		SendMessage((HWND)this->hwnd, WM_SETFONT, (WPARAM)font, TRUE);
	}
}

void UI::GUIControl::UpdatePos(Bool redraw)
{
#if !defined(_WIN32_WCE)
	WINDOWINFO wi;
	wi.cbSize = sizeof(WINDOWINFO);
	if (GetWindowInfo((HWND)hwnd, &wi))
	{
		if (wi.dwStyle & (WS_MAXIMIZE | WS_MINIMIZE))
		{
			return;
		}
	}
#endif

	if (this->parent)
	{
		Double xOfst = 0;
		Double yOfst = 0;
		this->parent->GetClientOfst(&xOfst, &yOfst);
		MoveWindow((HWND)hwnd, Math::Double2Int32((this->lxPos + xOfst) * this->hdpi / this->ddpi), Math::Double2Int32((this->lyPos + yOfst) * this->hdpi / this->ddpi), Math::Double2Int32((this->lxPos2 - this->lxPos) * this->hdpi / this->ddpi), Math::Double2Int32((this->lyPos2 - this->lyPos) * this->hdpi / this->ddpi), redraw?TRUE:FALSE);
	}
	else
	{
		Double newW = (this->lxPos2 - this->lxPos) * this->hdpi / this->ddpi;
		Double newH = (this->lyPos2 - this->lyPos) * this->hdpi / this->ddpi;
		Media::MonitorInfo *monInfo = this->GetMonitorInfo();
		Double newX;
		Double newY;
		if (monInfo)
		{
			Int32 maxW = monInfo->GetPixelWidth();
			Int32 maxH = monInfo->GetPixelHeight();
			if (newW > maxW)
				newW = maxW;
			if (newH > maxH)
				newH = maxH;
			RECT rc;
			GetWindowRect((HWND)this->hwnd, &rc);
			newX = (rc.left + rc.right - newW) * 0.5;
			newY = (rc.top + rc.bottom - newH) * 0.5;
			if (newY < monInfo->GetTop())
			{
				newY = monInfo->GetTop();
			}
			DEL_CLASS(monInfo);
		}
		else
		{
			int maxX = GetSystemMetrics(SM_CXSCREEN);
			int maxY = GetSystemMetrics(SM_CYSCREEN);
			if (newW > maxX)
				newW = maxX;
			if (newH > maxY)
				newH = maxY;
			RECT rc;
			GetWindowRect((HWND)this->hwnd, &rc);
			newX = (rc.left + rc.right - newW) * 0.5;
			newY = (rc.top + rc.bottom - newH) * 0.5;
			if (newY < 0)
			{
				newY = 0;
			}
		}

		MoveWindow((HWND)this->hwnd, Math::Double2Int32(newX), Math::Double2Int32(newY), Math::Double2Int32(newW), Math::Double2Int32(newH), redraw?TRUE:FALSE);
	}
}

void UI::GUIControl::UpdateBGColor()
{
	void *hbr = GetBGBrush();
#if defined(GCL_HBRBACKGROUND)
	if (hbr)
	{
		UI::GUICoreWin::MSSetClassObj(this->hwnd, GCL_HBRBACKGROUND, (OSInt)hbr);
	}
#elif defined(GCLP_HBRBACKGROUND)
	if (hbr)
	{
		UI::GUICoreWin::MSSetClassObj(this->hwnd, GCLP_HBRBACKGROUND, (OSInt)hbr);
	}
#endif
}


void UI::GUIControl::Redraw()
{
	InvalidateRect((HWND)this->hwnd, 0, TRUE);
}

void UI::GUIControl::SetCapture()
{
	::SetCapture((HWND)this->hwnd);
}
void UI::GUIControl::ReleaseCapture()
{
	::ReleaseCapture();
}

void UI::GUIControl::SetCursor(CursorType curType)
{
	HCURSOR cur;
	switch (curType)
	{
	case CT_ARROW:
		cur = LoadCursor(NULL, IDC_ARROW);
		break;
	case CT_HAND:
		cur = LoadCursor(NULL, IDC_HAND);
		break;
	case CT_SIZEH:
		cur = LoadCursor(NULL, IDC_SIZEWE);
		break;
	case CT_SIZEV:
		cur = LoadCursor(NULL, IDC_SIZENS);
		break;
	case CT_SIZENW:
		cur = LoadCursor(NULL, IDC_SIZENWSE);
		break;
	case CT_SIZENE:
		cur = LoadCursor(NULL, IDC_SIZENESW);
		break;
	default:
		return;
	}
	UI::GUICoreWin::MSSetClassObj(this->hwnd, GCL_HCURSOR, (OSInt)cur);
}

UI::GUIClientControl *UI::GUIControl::GetParent()
{
	return this->parent;
}

UI::GUIForm *UI::GUIControl::GetRootForm()
{
	UI::GUIControl *ctrl = this;
	const UTF8Char *objCls;
	while (ctrl)
	{
		objCls = ctrl->GetObjectClass();
		if (Text::StrEquals(objCls, (const UTF8Char*)"WinForm"))
		{
			if (ctrl->GetParent() == 0)
				return (UI::GUIForm*)ctrl;
		}
		ctrl = ctrl->GetParent();
	}
	return 0;
}

void *UI::GUIControl::GetHandle()
{
	return this->hwnd;
}

void *UI::GUIControl::GetHMonitor()
{
	return MonitorFromWindow((HWND)this->hwnd, MONITOR_DEFAULTTONEAREST);
}

Media::MonitorInfo *UI::GUIControl::GetMonitorInfo()
{
	HMONITOR hMon = MonitorFromWindow((HWND)this->hwnd, MONITOR_DEFAULTTONEAREST);
	if (hMon)
	{
		Media::MonitorInfo *info;
		NEW_CLASS(info, Media::MonitorInfo(hMon));
		return info;
	}
	else
	{
		return 0;
	}
}

void UI::GUIControl::SetDPI(Double hdpi, Double ddpi)
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
}

Double UI::GUIControl::GetHDPI()
{
	return this->hdpi;
}

Double UI::GUIControl::GetDDPI()
{
	return this->ddpi;
}

Media::DrawFont *UI::GUIControl::CreateDrawFont(Media::DrawImage *img)
{
	void *f = this->GetFont();
	if (f == 0)
		return 0;
	Media::GDIFont *fnt = (Media::GDIFont*)f;
	if (this->fontName == 0)
	{
		return img->CloneFont(fnt);
	}
	else
	{
		const WChar *wptr = Text::StrToWCharNew(this->fontName);
		NEW_CLASS(fnt, Media::GDIFont(((Media::GDIImage*)img)->hdcBmp, wptr, this->fontHeight * this->hdpi / this->ddpi / 0.75 * 72.0 / img->GetHDPI(), this->fontIsBold?Media::DrawEngine::DFS_BOLD:Media::DrawEngine::DFS_NORMAL, img, 0));
		Text::StrDelNew(wptr);
	}
	return fnt;
}


Int32 UI::GUIControl::GUIKey2OSKey(GUIKey guiKey)
{
	switch (guiKey)
	{
	case UI::GUIControl::GK_BACKSPACE:
		return VK_BACK;
	case UI::GUIControl::GK_TAB:
		return VK_TAB;
	case UI::GUIControl::GK_CLEAR:
		return VK_CLEAR;
	case UI::GUIControl::GK_ENTER:
		return VK_RETURN;
	case UI::GUIControl::GK_SHIFT:
		return VK_SHIFT;
	case UI::GUIControl::GK_CONTROL:
		return VK_CONTROL;
	case UI::GUIControl::GK_ALT:
		return VK_MENU;
	case UI::GUIControl::GK_PAUSE:
		return VK_PAUSE;
	case UI::GUIControl::GK_CAPITAL:
		return VK_CAPITAL;
	case UI::GUIControl::GK_KANA:
		return VK_KANA;
	case UI::GUIControl::GK_JUNJA:
		return VK_JUNJA;
	case UI::GUIControl::GK_FINAL:
		return VK_FINAL;
	case UI::GUIControl::GK_KANJI:
		return VK_KANJI;
	case UI::GUIControl::GK_ESCAPE:
		return VK_ESCAPE;
	case UI::GUIControl::GK_CONVERT:
		return VK_CONVERT;
#ifndef _WIN32_WCE
	case UI::GUIControl::GK_NONCONVERT:
		return VK_NONCONVERT;
	case UI::GUIControl::GK_ACCEPT:
		return VK_ACCEPT;
	case UI::GUIControl::GK_MODECHANGE:
		return VK_MODECHANGE;
#endif
	case UI::GUIControl::GK_SPACE:
		return VK_SPACE;
	case UI::GUIControl::GK_PAGEUP:
		return VK_PRIOR;
	case UI::GUIControl::GK_PAGEDOWN:
		return VK_NEXT;
	case UI::GUIControl::GK_END:
		return VK_END;
	case UI::GUIControl::GK_HOME:
		return VK_HOME;
	case UI::GUIControl::GK_LEFT:
		return VK_LEFT;
	case UI::GUIControl::GK_UP:
		return VK_UP;
	case UI::GUIControl::GK_RIGHT:
		return VK_RIGHT;
	case UI::GUIControl::GK_DOWN:
		return VK_DOWN;
	case UI::GUIControl::GK_SELECT:
		return VK_SELECT;
	case UI::GUIControl::GK_PRINT:
		return VK_PRINT;
	case UI::GUIControl::GK_EXECUTE:
		return VK_EXECUTE;
	case UI::GUIControl::GK_PRINTSCREEN:
		return VK_SNAPSHOT;
	case UI::GUIControl::GK_INSERT:
		return VK_INSERT;
	case UI::GUIControl::GK_DELETE:
		return VK_DELETE;
	case UI::GUIControl::GK_HELP:
		return VK_HELP;
	case UI::GUIControl::GK_0:
		return 0x30;
	case UI::GUIControl::GK_1:
		return 0x31;
	case UI::GUIControl::GK_2:
		return 0x32;
	case UI::GUIControl::GK_3:
		return 0x33;
	case UI::GUIControl::GK_4:
		return 0x34;
	case UI::GUIControl::GK_5:
		return 0x35;
	case UI::GUIControl::GK_6:
		return 0x36;
	case UI::GUIControl::GK_7:
		return  0x37;
	case UI::GUIControl::GK_8:
		return 0x38;
	case UI::GUIControl::GK_9:
		return 0x39;
	case UI::GUIControl::GK_A:
		return 'A';
	case UI::GUIControl::GK_B:
		return 'B';
	case UI::GUIControl::GK_C:
		return 'C';
	case UI::GUIControl::GK_D:
		return 'D';
	case UI::GUIControl::GK_E:
		return 'E';
	case UI::GUIControl::GK_F:
		return 'F';
	case UI::GUIControl::GK_G:
		return 'G';
	case UI::GUIControl::GK_H:
		return 'H';
	case UI::GUIControl::GK_I:
		return 'I';
	case UI::GUIControl::GK_J:
		return 'J';
	case UI::GUIControl::GK_K:
		return 'K';
	case UI::GUIControl::GK_L:
		return 'L';
	case UI::GUIControl::GK_M:
		return 'M';
	case UI::GUIControl::GK_N:
		return 'N';
	case UI::GUIControl::GK_O:
		return 'O';
	case UI::GUIControl::GK_P:
		return 'P';
	case UI::GUIControl::GK_Q:
		return 'Q';
	case UI::GUIControl::GK_R:
		return 'R';
	case UI::GUIControl::GK_S:
		return 'S';
	case UI::GUIControl::GK_T:
		return 'T';
	case UI::GUIControl::GK_U:
		return 'U';
	case UI::GUIControl::GK_V:
		return 'V';
	case UI::GUIControl::GK_W:
		return 'W';
	case UI::GUIControl::GK_X:
		return 'X';
	case UI::GUIControl::GK_Y:
		return 'Y';
	case UI::GUIControl::GK_Z:
		return 'Z';
	case UI::GUIControl::GK_LWIN:
		return VK_LWIN;
	case UI::GUIControl::GK_RWIN:
		return VK_RWIN;
	case UI::GUIControl::GK_APPS:
		return VK_APPS;
	case UI::GUIControl::GK_SLEEP:
		return VK_SLEEP;
	case UI::GUIControl::GK_NUMPAD0:
		return VK_NUMPAD0;
	case UI::GUIControl::GK_NUMPAD1:
		return VK_NUMPAD1;
	case UI::GUIControl::GK_NUMPAD2:
		return VK_NUMPAD2;
	case UI::GUIControl::GK_NUMPAD3:
		return VK_NUMPAD3;
	case UI::GUIControl::GK_NUMPAD4:
		return VK_NUMPAD4;
	case UI::GUIControl::GK_NUMPAD5:
		return VK_NUMPAD5;
	case UI::GUIControl::GK_NUMPAD6:
		return VK_NUMPAD6;
	case UI::GUIControl::GK_NUMPAD7:
		return VK_NUMPAD7;
	case UI::GUIControl::GK_NUMPAD8:
		return VK_NUMPAD8;
	case UI::GUIControl::GK_NUMPAD9:
		return VK_NUMPAD9;
	case UI::GUIControl::GK_MULTIPLY:
		return VK_MULTIPLY;
	case UI::GUIControl::GK_ADD:
		return VK_ADD;
	case UI::GUIControl::GK_SEPARATOR:
		return VK_SEPARATOR;
	case UI::GUIControl::GK_SUBTRACT:
		return VK_SUBTRACT;
	case UI::GUIControl::GK_DECIMAL:
		return VK_DECIMAL;
	case UI::GUIControl::GK_DIVIDE:
		return VK_DIVIDE;
	case UI::GUIControl::GK_F1:
		return VK_F1;
	case UI::GUIControl::GK_F2:
		return VK_F2;
	case UI::GUIControl::GK_F3:
		return VK_F3;
	case UI::GUIControl::GK_F4:
		return VK_F4;
	case UI::GUIControl::GK_F5:
		return VK_F5;
	case UI::GUIControl::GK_F6:
		return VK_F6;
	case UI::GUIControl::GK_F7:
		return VK_F7;
	case UI::GUIControl::GK_F8:
		return VK_F8;
	case UI::GUIControl::GK_F9:
		return VK_F9;
	case UI::GUIControl::GK_F10:
		return VK_F10;
	case UI::GUIControl::GK_F11:
		return VK_F11;
	case UI::GUIControl::GK_F12:
		return VK_F12;
	case UI::GUIControl::GK_F13:
		return VK_F13;
	case UI::GUIControl::GK_F14:
		return VK_F14;
	case UI::GUIControl::GK_F15:
		return VK_F15;
	case UI::GUIControl::GK_F16:
		return VK_F16;
	case UI::GUIControl::GK_F17:
		return VK_F17;
	case UI::GUIControl::GK_F18:
		return VK_F18;
	case UI::GUIControl::GK_F19:
		return VK_F19;
	case UI::GUIControl::GK_F20:
		return VK_F20;
	case UI::GUIControl::GK_F21:
		return VK_F21;
	case UI::GUIControl::GK_F22:
		return VK_F22;
	case UI::GUIControl::GK_F23:
		return VK_F23;
	case UI::GUIControl::GK_F24:
		return VK_F24;
	case UI::GUIControl::GK_NUMLOCK:
		return VK_NUMLOCK;
	case UI::GUIControl::GK_SCROLLLOCK:
		return VK_SCROLL;
#ifndef _WIN32_WCE
	case UI::GUIControl::GK_OEM_1:
		return VK_OEM_1;
	case UI::GUIControl::GK_OEM_PLUS:
		return VK_OEM_PLUS;
	case UI::GUIControl::GK_OEM_COMMA:
		return VK_OEM_COMMA;
	case UI::GUIControl::GK_OEM_MINUS:
		return VK_OEM_MINUS;
	case UI::GUIControl::GK_OEM_PERIOD:
		return VK_OEM_PERIOD;
	case UI::GUIControl::GK_OEM_2:
		return VK_OEM_2;
	case UI::GUIControl::GK_OEM_3:
		return VK_OEM_3;
	case UI::GUIControl::GK_OEM_4:
		return VK_OEM_4;
	case UI::GUIControl::GK_OEM_5:
		return VK_OEM_5;
	case UI::GUIControl::GK_OEM_6:
		return VK_OEM_6;
	case UI::GUIControl::GK_OEM_7:
		return VK_OEM_7;
#endif
	default:
		return 0;
	}
}

UI::GUIControl::GUIKey UI::GUIControl::OSKey2GUIKey(Int32 osKey)
{
	switch (osKey)
	{
	case VK_BACK:
		return UI::GUIControl::GK_BACKSPACE;
	case VK_TAB:
		return UI::GUIControl::GK_TAB;
	case VK_CLEAR:
		return UI::GUIControl::GK_CLEAR;
	case VK_RETURN:
		return UI::GUIControl::GK_ENTER;
	case VK_SHIFT:
		return UI::GUIControl::GK_SHIFT;
	case VK_CONTROL:
		return UI::GUIControl::GK_CONTROL;
	case VK_MENU:
		return UI::GUIControl::GK_ALT;
	case VK_PAUSE:
		return UI::GUIControl::GK_PAUSE;
	case VK_CAPITAL:
		return UI::GUIControl::GK_CAPITAL;
	case VK_KANA:
		return UI::GUIControl::GK_KANA;
	case VK_JUNJA:
		return UI::GUIControl::GK_JUNJA;
	case VK_FINAL:
		return UI::GUIControl::GK_FINAL;
	case VK_KANJI:
		return UI::GUIControl::GK_KANJI;
	case VK_ESCAPE:
		return UI::GUIControl::GK_ESCAPE;
	case VK_CONVERT:
		return UI::GUIControl::GK_CONVERT;
#ifndef _WIN32_WCE
	case VK_NONCONVERT:
		return UI::GUIControl::GK_NONCONVERT;
	case VK_ACCEPT:
		return UI::GUIControl::GK_ACCEPT;
	case VK_MODECHANGE:
		return UI::GUIControl::GK_MODECHANGE;
#endif
	case VK_SPACE:
		return UI::GUIControl::GK_SPACE;
	case VK_PRIOR:
		return UI::GUIControl::GK_PAGEUP;
	case VK_NEXT:
		return UI::GUIControl::GK_PAGEDOWN;
	case VK_END:
		return UI::GUIControl::GK_END;
	case VK_HOME:
		return UI::GUIControl::GK_HOME;
	case VK_LEFT:
		return UI::GUIControl::GK_LEFT;
	case VK_UP:
		return UI::GUIControl::GK_UP;
	case VK_RIGHT:
		return UI::GUIControl::GK_RIGHT;
	case VK_DOWN:
		return UI::GUIControl::GK_DOWN;
	case VK_SELECT:
		return UI::GUIControl::GK_SELECT;
	case VK_PRINT:
		return UI::GUIControl::GK_PRINT;
	case VK_EXECUTE:
		return UI::GUIControl::GK_EXECUTE;
	case VK_SNAPSHOT:
		return UI::GUIControl::GK_PRINTSCREEN;
	case VK_INSERT:
		return UI::GUIControl::GK_INSERT;
	case VK_DELETE:
		return UI::GUIControl::GK_DELETE;
	case VK_HELP:
		return UI::GUIControl::GK_HELP;
	case 0x30:
		return UI::GUIControl::GK_0;
	case 0x31:
		return UI::GUIControl::GK_1;
	case 0x32:
		return UI::GUIControl::GK_2;
	case 0x33:
		return UI::GUIControl::GK_3;
	case 0x34:
		return UI::GUIControl::GK_4;
	case 0x35:
		return UI::GUIControl::GK_5;
	case 0x36:
		return UI::GUIControl::GK_6;
	case 0x37:
		return UI::GUIControl::GK_7;
	case 0x38:
		return UI::GUIControl::GK_8;
	case 0x39:
		return UI::GUIControl::GK_9;
	case 'A':
		return UI::GUIControl::GK_A;
	case 'B':
		return UI::GUIControl::GK_B;
	case 'C':
		return UI::GUIControl::GK_C;
	case 'D':
		return UI::GUIControl::GK_D;
	case 'E':
		return UI::GUIControl::GK_E;
	case 'F':
		return UI::GUIControl::GK_F;
	case 'G':
		return UI::GUIControl::GK_G;
	case 'H':
		return UI::GUIControl::GK_H;
	case 'I':
		return UI::GUIControl::GK_I;
	case 'J':
		return UI::GUIControl::GK_J;
	case 'K':
		return UI::GUIControl::GK_K;
	case 'L':
		return UI::GUIControl::GK_L;
	case 'M':
		return UI::GUIControl::GK_M;
	case 'N':
		return UI::GUIControl::GK_N;
	case 'O':
		return UI::GUIControl::GK_O;
	case 'P':
		return UI::GUIControl::GK_P;
	case 'Q':
		return UI::GUIControl::GK_Q;
	case 'R':
		return UI::GUIControl::GK_R;
	case 'S':
		return UI::GUIControl::GK_S;
	case 'T':
		return UI::GUIControl::GK_T;
	case 'U':
		return UI::GUIControl::GK_U;
	case 'V':
		return UI::GUIControl::GK_V;
	case 'W':
		return UI::GUIControl::GK_W;
	case 'X':
		return UI::GUIControl::GK_X;
	case 'Y':
		return UI::GUIControl::GK_Y;
	case 'Z':
		return UI::GUIControl::GK_Z;
	case VK_LWIN:
		return UI::GUIControl::GK_LWIN;
	case VK_RWIN:
		return UI::GUIControl::GK_RWIN;
	case VK_APPS:
		return UI::GUIControl::GK_APPS;
	case VK_SLEEP:
		return UI::GUIControl::GK_SLEEP;
	case VK_NUMPAD0:
		return UI::GUIControl::GK_NUMPAD0;
	case VK_NUMPAD1:
		return UI::GUIControl::GK_NUMPAD1;
	case VK_NUMPAD2:
		return UI::GUIControl::GK_NUMPAD2;
	case VK_NUMPAD3:
		return UI::GUIControl::GK_NUMPAD3;
	case VK_NUMPAD4:
		return UI::GUIControl::GK_NUMPAD4;
	case VK_NUMPAD5:
		return UI::GUIControl::GK_NUMPAD5;
	case VK_NUMPAD6:
		return UI::GUIControl::GK_NUMPAD6;
	case VK_NUMPAD7:
		return UI::GUIControl::GK_NUMPAD7;
	case VK_NUMPAD8:
		return UI::GUIControl::GK_NUMPAD8;
	case VK_NUMPAD9:
		return UI::GUIControl::GK_NUMPAD9;
	case VK_MULTIPLY:
		return UI::GUIControl::GK_MULTIPLY;
	case VK_ADD:
		return UI::GUIControl::GK_ADD;
	case VK_SEPARATOR:
		return UI::GUIControl::GK_SEPARATOR;
	case VK_SUBTRACT:
		return UI::GUIControl::GK_SUBTRACT;
	case VK_DECIMAL:
		return UI::GUIControl::GK_DECIMAL;
	case VK_DIVIDE:
		return UI::GUIControl::GK_DIVIDE;
	case VK_F1:
		return UI::GUIControl::GK_F1;
	case VK_F2:
		return UI::GUIControl::GK_F2;
	case VK_F3:
		return UI::GUIControl::GK_F3;
	case VK_F4:
		return UI::GUIControl::GK_F4;
	case VK_F5:
		return UI::GUIControl::GK_F5;
	case VK_F6:
		return UI::GUIControl::GK_F6;
	case VK_F7:
		return UI::GUIControl::GK_F7;
	case VK_F8:
		return UI::GUIControl::GK_F8;
	case VK_F9:
		return UI::GUIControl::GK_F9;
	case VK_F10:
		return UI::GUIControl::GK_F10;
	case VK_F11:
		return UI::GUIControl::GK_F11;
	case VK_F12:
		return UI::GUIControl::GK_F12;
	case VK_F13:
		return UI::GUIControl::GK_F13;
	case VK_F14:
		return UI::GUIControl::GK_F14;
	case VK_F15:
		return UI::GUIControl::GK_F15;
	case VK_F16:
		return UI::GUIControl::GK_F16;
	case VK_F17:
		return UI::GUIControl::GK_F17;
	case VK_F18:
		return UI::GUIControl::GK_F18;
	case VK_F19:
		return UI::GUIControl::GK_F19;
	case VK_F20:
		return UI::GUIControl::GK_F20;
	case VK_F21:
		return UI::GUIControl::GK_F21;
	case VK_F22:
		return UI::GUIControl::GK_F22;
	case VK_F23:
		return UI::GUIControl::GK_F23;
	case VK_F24:
		return UI::GUIControl::GK_F24;
	case VK_NUMLOCK:
		return UI::GUIControl::GK_NUMLOCK;
	case VK_SCROLL:
		return UI::GUIControl::GK_SCROLLLOCK;
#ifndef _WIN32_WCE
	case VK_OEM_1:
		return UI::GUIControl::GK_OEM_1;
	case VK_OEM_PLUS:
		return UI::GUIControl::GK_OEM_PLUS;
	case VK_OEM_COMMA:
		return UI::GUIControl::GK_OEM_COMMA;
	case VK_OEM_MINUS:
		return UI::GUIControl::GK_OEM_MINUS;
	case VK_OEM_PERIOD:
		return UI::GUIControl::GK_OEM_PERIOD;
	case VK_OEM_2:
		return UI::GUIControl::GK_OEM_2;
	case VK_OEM_3:
		return UI::GUIControl::GK_OEM_3;
	case VK_OEM_4:
		return UI::GUIControl::GK_OEM_4;
	case VK_OEM_5:
		return UI::GUIControl::GK_OEM_5;
	case VK_OEM_6:
		return UI::GUIControl::GK_OEM_6;
	case VK_OEM_7:
		return UI::GUIControl::GK_OEM_7;
#endif
	default:
		return UI::GUIControl::GK_NONE;
	}
}

UI::GUIControl::DragErrorType UI::GUIControl::HandleDropEvents(UI::GUIDropHandler *hdlr)
{
	if (this->dropHdlr)
	{
		UI::GUIWindowDragDrop *dragDrop = (UI::GUIWindowDragDrop *)this->dropHdlr;
		dragDrop->SetHandler(hdlr);
		return UI::GUIControl::DET_NOERROR;
	}
	else
	{
		UI::GUIWindowDragDrop *dragDrop;
		NEW_CLASS(dragDrop, UI::GUIWindowDragDrop((HWND)this->GetHandle(), hdlr));

		UI::GUIControl::DragErrorType errType;
#if !defined(_WIN32_WCE)
		OleInitialize(0);
		HRESULT hRes = RegisterDragDrop((HWND)this->GetHandle(), dragDrop);
		if (hRes == S_OK)
		{
			errType = UI::GUIControl::DET_NOERROR;
			this->dropHdlr = dragDrop;
		}
		else
		{
			DEL_CLASS(dragDrop);
			if (hRes == DRAGDROP_E_INVALIDHWND)
			{
				errType = UI::GUIControl::DET_INVALIDCONTROL;
			}
			else if (hRes == DRAGDROP_E_ALREADYREGISTERED)
			{
				errType = UI::GUIControl::DET_ALREADYREGISTER;
			}
			else if (hRes == E_OUTOFMEMORY)
			{
				errType = UI::GUIControl::DET_OUTOFMEMORY;
			}
			else
			{
				errType = UI::GUIControl::DET_UNKNOWN;
			}
			OleUninitialize();
		}
#else
		errType = UI::GUIControl::DET_NOERROR;
#endif
		return errType;
	}
}
