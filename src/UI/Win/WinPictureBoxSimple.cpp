#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/GDIEngine.h"
#include "Sync/Interlocked.h"
#include "UI/GUIClientControl.h"
#include "UI/Win/WinCore.h"
#include "UI/Win/WinPictureBoxSimple.h"
#include <windows.h>

#define CLASSNAME L"PictureBoxSimle"
Int32 UI::Win::WinPictureBoxSimple::useCnt = 0;

#ifndef GWL_USERDATA
#define GWL_USERDATA GWLP_USERDATA
#endif

OSInt __stdcall UI::Win::WinPictureBoxSimple::PBWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam)
{
	UI::Win::WinPictureBoxSimple *me = (UI::Win::WinPictureBoxSimple*)UI::Win::WinCore::MSGetWindowObj((ControlHandle*)hWnd, GWL_USERDATA);
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
		me->EventButtonDown(Math::Coord2D<OSInt>((Int16)LOWORD(lParam), (Int16)HIWORD(lParam)), MBTN_LEFT);
		return 0;
	case WM_RBUTTONDOWN:
		me->EventButtonDown(Math::Coord2D<OSInt>((Int16)LOWORD(lParam), (Int16)HIWORD(lParam)), MBTN_RIGHT);
		return 0;
	case WM_MBUTTONDOWN:
		me->EventButtonDown(Math::Coord2D<OSInt>((Int16)LOWORD(lParam), (Int16)HIWORD(lParam)), MBTN_MIDDLE);
		return 0;
	case WM_LBUTTONUP:
		me->EventButtonUp(Math::Coord2D<OSInt>((Int16)LOWORD(lParam), (Int16)HIWORD(lParam)), MBTN_LEFT);
		return 0;
	case WM_RBUTTONUP:
		me->EventButtonUp(Math::Coord2D<OSInt>((Int16)LOWORD(lParam), (Int16)HIWORD(lParam)), MBTN_RIGHT);
		return 0;
	case WM_MBUTTONUP:
		me->EventButtonUp(Math::Coord2D<OSInt>((Int16)LOWORD(lParam), (Int16)HIWORD(lParam)), MBTN_MIDDLE);
		return 0;
	case WM_MOUSEMOVE:
		me->EventMouseMove(Math::Coord2D<OSInt>((Int16)LOWORD(lParam), (Int16)HIWORD(lParam)));
		return 0;
	}
	return DefWindowProc((HWND)hWnd, msg, wParam, lParam);
}

void UI::Win::WinPictureBoxSimple::OnPaint()
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

void UI::Win::WinPictureBoxSimple::Init(void *hInst)
{
	WNDCLASSW wc;
    wc.style = 0; 
	wc.lpfnWndProc = (WNDPROC)UI::Win::WinPictureBoxSimple::PBWndProc; 
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

void UI::Win::WinPictureBoxSimple::Deinit(void *hInst)
{
	UnregisterClassW(CLASSNAME, (HINSTANCE)hInst);
}

void UI::Win::WinPictureBoxSimple::UpdatePreview()
{
	NotNullPtr<Media::DrawImage> dimg;
	if (dimg.Set(this->prevImageD))
	{
		this->eng->DeleteImage(dimg);
		this->prevImageD = 0;
	}

	NotNullPtr<Media::StaticImage> simg;
	if (this->currImage.SetTo(simg))
	{
		this->prevImageD = this->eng->ConvImage(simg);
	}
	this->Redraw();
}

UI::Win::WinPictureBoxSimple::WinPictureBoxSimple(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, NotNullPtr<Media::DrawEngine> eng, Bool hasBorder) : UI::GUIPictureBoxSimple(ui, parent, eng, hasBorder)
{
	this->currImage = 0;
	this->prevImageD = 0;

	if (Sync::Interlocked::IncrementI32(useCnt) == 1)
	{
		Init(((UI::Win::WinCore*)this->ui.Ptr())->GetHInst());
	}

	UInt32 style = WS_CLIPSIBLINGS | WS_CHILD;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	this->InitControl(((UI::Win::WinCore*)this->ui.Ptr())->GetHInst(), parent, CLASSNAME, (const UTF8Char*)"", style, 0, 0, 0, 200, 200);
}

UI::Win::WinPictureBoxSimple::~WinPictureBoxSimple()
{
	NotNullPtr<Media::DrawImage> dimg;
	if (dimg.Set(this->prevImageD))
	{
		this->eng->DeleteImage(dimg);
		this->prevImageD = 0;
	}
	if (Sync::Interlocked::DecrementI32(useCnt) == 0)
	{
		Deinit(((UI::Win::WinCore*)this->ui.Ptr())->GetHInst());
	}
}

OSInt UI::Win::WinPictureBoxSimple::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::Win::WinPictureBoxSimple::SetImage(Optional<Media::StaticImage> currImage)
{
	this->currImage = currImage;
	this->UpdatePreview();
}

void UI::Win::WinPictureBoxSimple::SetImageDImg(Media::DrawImage *img)
{
	this->currImage = 0;
	NotNullPtr<Media::DrawImage> dimg;
	NotNullPtr<Media::GDIImage> gimg;
	if (dimg.Set(this->prevImageD))
	{
		this->eng->DeleteImage(dimg);
		this->prevImageD = 0;
	}
	if (gimg.Set((Media::GDIImage*)img))
	{
		this->prevImageD = ((Media::GDIEngine*)this->eng.Ptr())->CloneImage(gimg);
	}
	this->Redraw();
}
