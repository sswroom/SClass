#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/GDIEngine.h"
#include "Sync/Interlocked.h"
#include "UI/GUIClientControl.h"
#include "UI/GUICoreWin.h"
#include "UI/GUIPictureBoxSimple.h"
#include <windows.h>

#define CLASSNAME L"PictureBoxSimle"
Int32 UI::GUIPictureBoxSimple::useCnt = 0;

#ifndef GWL_USERDATA
#define GWL_USERDATA GWLP_USERDATA
#endif

OSInt __stdcall UI::GUIPictureBoxSimple::PBWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam)
{
	UI::GUIPictureBoxSimple *me = (UI::GUIPictureBoxSimple*)UI::GUICoreWin::MSGetWindowObj((ControlHandle*)hWnd, GWL_USERDATA);
	UOSInt i;
	switch (msg)
	{
	case WM_SIZE:
		break;
	case WM_ERASEBKGND:
		return 0;
	case WM_PAINT:
		me->OnPaint();
		return 0;
	case WM_LBUTTONDOWN:
		i = me->mouseDownHdlrs.GetCount();
		while (i-- > 0)
		{
			me->mouseDownHdlrs.GetItem(i)(me->mouseDownObjs.GetItem(i), Math::Coord2D<OSInt>((Int16)LOWORD(lParam), (Int16)HIWORD(lParam)), MBTN_LEFT);
		}
		return 0;
	case WM_RBUTTONDOWN:
		i = me->mouseDownHdlrs.GetCount();
		while (i-- > 0)
		{
			me->mouseDownHdlrs.GetItem(i)(me->mouseDownObjs.GetItem(i), Math::Coord2D<OSInt>((Int16)LOWORD(lParam), (Int16)HIWORD(lParam)), MBTN_RIGHT);
		}
		return 0;
	case WM_MBUTTONDOWN:
		i = me->mouseDownHdlrs.GetCount();
		while (i-- > 0)
		{
			me->mouseDownHdlrs.GetItem(i)(me->mouseDownObjs.GetItem(i), Math::Coord2D<OSInt>((Int16)LOWORD(lParam), (Int16)HIWORD(lParam)), MBTN_MIDDLE);
		}
		return 0;
	case WM_LBUTTONUP:
		i = me->mouseUpHdlrs.GetCount();
		while (i-- > 0)
		{
			me->mouseUpHdlrs.GetItem(i)(me->mouseUpObjs.GetItem(i), Math::Coord2D<OSInt>((Int16)LOWORD(lParam), (Int16)HIWORD(lParam)), MBTN_LEFT);
		}
		return 0;
	case WM_RBUTTONUP:
		i = me->mouseUpHdlrs.GetCount();
		while (i-- > 0)
		{
			me->mouseUpHdlrs.GetItem(i)(me->mouseUpObjs.GetItem(i), Math::Coord2D<OSInt>((Int16)LOWORD(lParam), (Int16)HIWORD(lParam)), MBTN_RIGHT);
		}
		return 0;
	case WM_MBUTTONUP:
		i = me->mouseUpHdlrs.GetCount();
		while (i-- > 0)
		{
			me->mouseUpHdlrs.GetItem(i)(me->mouseUpObjs.GetItem(i), Math::Coord2D<OSInt>((Int16)LOWORD(lParam), (Int16)HIWORD(lParam)), MBTN_MIDDLE);
		}
		return 0;
	case WM_MOUSEMOVE:
		i = me->mouseMoveHdlrs.GetCount();
		while (i-- > 0)
		{
			me->mouseMoveHdlrs.GetItem(i)(me->mouseMoveObjs.GetItem(i), Math::Coord2D<OSInt>((Int16)LOWORD(lParam), (Int16)HIWORD(lParam)), MBTN_MIDDLE);
		}
		return 0;
	}
	return DefWindowProc((HWND)hWnd, msg, wParam, lParam);
}

void UI::GUIPictureBoxSimple::OnPaint()
{
	HGDIOBJ lastObj;
	PAINTSTRUCT ps;
	RECT rc;
	GetClientRect((HWND)this->hwnd, &rc);
	BeginPaint((HWND)this->hwnd, &ps);
	if (!this->noBGColor)
	{
		if (this->hbrBackground)
		{
			FillRect(ps.hdc, &rc, (HBRUSH)this->hbrBackground);
		}
		else
		{
			FillRect(ps.hdc, &rc, (HBRUSH)(COLOR_WINDOW));
		}
	}

	if (this->prevImageD)
	{
		OSInt x = (rc.right - rc.left - (OSInt)this->prevImageD->GetWidth()) >> 1;
		OSInt y = (rc.bottom - rc.top - (OSInt)this->prevImageD->GetHeight()) >> 1;

		if (this->noBGColor || this->prevImageD->GetAlphaType() == Media::AT_NO_ALPHA)
		{
			BitBlt(ps.hdc, (int)x, (int)y, (int)this->prevImageD->GetWidth(), (int)this->prevImageD->GetHeight(), (HDC)((Media::GDIImage*)this->prevImageD)->GetHDC(), 0, 0, SRCCOPY);
		}
		else
		{
			BLENDFUNCTION bf;
			bf.SourceConstantAlpha = 255;
			bf.AlphaFormat = AC_SRC_ALPHA;
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			AlphaBlend(ps.hdc, (int)x, (int)y, (int)this->prevImageD->GetWidth(), (int)this->prevImageD->GetHeight(), (HDC)((Media::GDIImage*)this->prevImageD)->GetHDC(), 0, 0, (int)this->prevImageD->GetWidth(), (int)this->prevImageD->GetHeight(), bf);
		}
	}
	else if (this->noBGColor)
	{
		FillRect(ps.hdc, &rc, (HBRUSH)(COLOR_WINDOW));
	}

	if (this->hasBorder)
	{
		POINT pts[5];
		HPEN p = CreatePen(0, 0, 0);
		lastObj = SelectObject(ps.hdc, p);
		pts[0].x = rc.left;
		pts[0].y = rc.top;
		pts[1].x = rc.right - 1;
		pts[1].y = rc.top;
		pts[2].x = rc.right - 1;
		pts[2].y = rc.bottom - 1;
		pts[3].x = rc.left;
		pts[3].y = rc.bottom - 1;
		pts[4].x = rc.left;
		pts[4].y = rc.top;
		Polyline(ps.hdc, pts, 5);
		SelectObject(ps.hdc, lastObj);
		DeleteObject(p);
	}
	EndPaint((HWND)this->hwnd, &ps);
}

void UI::GUIPictureBoxSimple::Init(void *hInst)
{
	WNDCLASSW wc;
    wc.style = 0; 
	wc.lpfnWndProc = (WNDPROC)UI::GUIPictureBoxSimple::PBWndProc; 
    wc.cbClsExtra = 0; 
    wc.cbWndExtra = 0; 
    wc.hInstance = (HINSTANCE)hInst; 
    wc.hIcon = 0; 
    wc.hCursor = LoadCursor((HINSTANCE) NULL, IDC_ARROW); 
    wc.hbrBackground = 0; 
    wc.lpszMenuName = 0; 
    wc.lpszClassName = CLASSNAME; 

    if (!RegisterClassW(&wc)) 
        return; 
}

void UI::GUIPictureBoxSimple::Deinit(void *hInst)
{
	UnregisterClassW(CLASSNAME, (HINSTANCE)hInst);
}

void UI::GUIPictureBoxSimple::UpdatePreview()
{
	if (this->prevImageD)
	{
		this->eng->DeleteImage(this->prevImageD);
		this->prevImageD = 0;
	}

	if (this->currImage)
	{
		this->prevImageD = this->eng->ConvImage(this->currImage);
	}
	this->Redraw();
}

UI::GUIPictureBoxSimple::GUIPictureBoxSimple(NotNullPtr<UI::GUICore> ui, UI::GUIClientControl *parent, Media::DrawEngine *eng, Bool hasBorder) : UI::GUIControl(ui, parent)
{
	this->hasBorder = hasBorder;
	this->eng = eng;
	this->currImage = 0;
	this->prevImageD = 0;
	this->noBGColor = false;

	if (Sync::Interlocked::Increment(&useCnt) == 1)
	{
		Init(((UI::GUICoreWin*)this->ui.Ptr())->GetHInst());
	}

	UInt32 style = WS_CLIPSIBLINGS | WS_CHILD;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	this->InitControl(((UI::GUICoreWin*)this->ui.Ptr())->GetHInst(), parent, CLASSNAME, (const UTF8Char*)"", style, 0, 0, 0, 200, 200);
}

UI::GUIPictureBoxSimple::~GUIPictureBoxSimple()
{
	if (this->prevImageD)
	{
		this->eng->DeleteImage(this->prevImageD);
		this->prevImageD = 0;
	}
	if (Sync::Interlocked::Decrement(&useCnt) == 0)
	{
		Deinit(((UI::GUICoreWin*)this->ui.Ptr())->GetHInst());
	}
}

Text::CString UI::GUIPictureBoxSimple::GetObjectClass()
{
	return CSTR("PictureBoxSimple");
}

OSInt UI::GUIPictureBoxSimple::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GUIPictureBoxSimple::HandleMouseDown(MouseEventHandler hdlr, void *userObj)
{
	this->mouseDownHdlrs.Add(hdlr);
	this->mouseDownObjs.Add(userObj);
}

void UI::GUIPictureBoxSimple::HandleMouseMove(MouseEventHandler hdlr, void *userObj)
{
	this->mouseMoveHdlrs.Add(hdlr);
	this->mouseMoveObjs.Add(userObj);
}

void UI::GUIPictureBoxSimple::HandleMouseUp(MouseEventHandler hdlr, void *userObj)
{
	this->mouseUpHdlrs.Add(hdlr);
	this->mouseUpObjs.Add(userObj);
}

void UI::GUIPictureBoxSimple::SetImage(Media::StaticImage *currImage)
{
	this->currImage = currImage;
	this->UpdatePreview();
}

void UI::GUIPictureBoxSimple::SetImageDImg(Media::DrawImage *img)
{
	this->currImage = 0;
	if (this->prevImageD)
	{
		this->eng->DeleteImage(this->prevImageD);
		this->prevImageD = 0;
	}
	if (img)
	{
		this->prevImageD = ((Media::GDIEngine*)this->eng)->CloneImage((Media::GDIImage*)img);
	}
	this->Redraw();
}

void UI::GUIPictureBoxSimple::SetNoBGColor(Bool noBGColor)
{
	this->noBGColor = noBGColor;
}
