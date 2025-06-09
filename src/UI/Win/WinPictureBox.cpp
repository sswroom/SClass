#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/Interlocked.h"
#include "Media/GDIEngine.h"
#include "Media/Resizer/LanczosResizerRGB_C8.h"
#include "UI/GUIClientControl.h"
#include "UI/Win/WinCore.h"
#include "UI/Win/WinPictureBox.h"
#include <windows.h>

#define CLASSNAME L"PictureBox"
Int32 UI::Win::WinPictureBox::useCnt = 0;

#ifndef GWL_USERDATA
#define GWL_USERDATA GWLP_USERDATA
#endif

OSInt __stdcall UI::Win::WinPictureBox::PBWndProc(void *hWnd, UInt32 msg, UInt32 wParam, OSInt lParam)
{
	UI::Win::WinPictureBox *me = (UI::Win::WinPictureBox*)(OSInt)GetWindowLongPtr((HWND)hWnd, GWL_USERDATA);
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

void UI::Win::WinPictureBox::OnPaint()
{
	HGDIOBJ lastObj;
	PAINTSTRUCT ps;
	RECT rc;
	GetClientRect((HWND)this->hwnd.OrNull(), &rc);
	BeginPaint((HWND)this->hwnd.OrNull(), &ps);
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

	NN<Media::DrawImage> prevImageD;
	if (this->prevImageD.SetTo(prevImageD))
	{
		OSInt x = (rc.right - rc.left - (OSInt)prevImageD->GetWidth()) >> 1;
		OSInt y = (rc.bottom - rc.top - (OSInt)prevImageD->GetHeight()) >> 1;

		if (prevImageD->GetAlphaType() == Media::AT_ALPHA)
		{
			BLENDFUNCTION bf;
			bf.SourceConstantAlpha = 255;
			bf.AlphaFormat = AC_SRC_ALPHA;
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			AlphaBlend(ps.hdc, (int)x, (int)y, (int)prevImageD->GetWidth(), (int)prevImageD->GetHeight(), (HDC)NN<Media::GDIImage>::ConvertFrom(prevImageD)->GetHDC(), 0, 0, (int)prevImageD->GetWidth(), (int)prevImageD->GetHeight(), bf);
		}
		else
		{
			BitBlt(ps.hdc, (int)x, (int)y, (int)prevImageD->GetWidth(), (int)prevImageD->GetHeight(), (HDC)NN<Media::GDIImage>::ConvertFrom(prevImageD)->GetHDC(), 0, 0, SRCCOPY);
		}
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
	EndPaint((HWND)this->hwnd.OrNull(), &ps);
}

void UI::Win::WinPictureBox::Init(Optional<InstanceHandle> hInst)
{
	WNDCLASSW wc;
    wc.style = 0; 
	wc.lpfnWndProc = (WNDPROC)UI::Win::WinPictureBox::PBWndProc; 
    wc.cbClsExtra = 0; 
    wc.cbWndExtra = 0; 
    wc.hInstance = (HINSTANCE)hInst.OrNull(); 
    wc.hIcon = 0; 
    wc.hCursor = LoadCursor((HINSTANCE) NULL, IDC_ARROW); 
    wc.hbrBackground = 0; 
    wc.lpszMenuName = 0; 
    wc.lpszClassName = CLASSNAME; 

    if (!RegisterClassW(&wc)) 
        return; 
}

void UI::Win::WinPictureBox::Deinit(Optional<InstanceHandle> hInst)
{
	UnregisterClassW(CLASSNAME, (HINSTANCE)hInst.OrNull());
}

void UI::Win::WinPictureBox::UpdatePreview()
{
	NN<Media::DrawImage> img;
	NN<Media::StaticImage> simg;
	if (this->prevImageD.SetTo(img))
	{
		this->eng->DeleteImage(img);
		this->prevImageD = 0;
	}

	if (this->currImage.SetTo(simg))
	{
		if (this->allowResize)
		{
			NN<Media::StaticImage> tmpImage;
			if (resizer->ProcessToNew(simg).SetTo(tmpImage))
			{
				this->prevImageD = this->eng->ConvImage(tmpImage, 0);
				tmpImage.Delete();
			}
			else
			{
				this->prevImageD = this->eng->ConvImage(simg, 0);
			}
		}
		else
		{
			this->prevImageD = this->eng->ConvImage(simg, 0);
		}
	}
	this->Redraw();
}

UI::Win::WinPictureBox::WinPictureBox(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, Bool hasBorder, Bool allowResize) : UI::GUIPictureBox(ui, parent, eng, hasBorder, allowResize)
{
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

UI::Win::WinPictureBox::~WinPictureBox()
{
	NN<Media::DrawImage> img;
	if (this->prevImageD.SetTo(img))
	{
		this->eng->DeleteImage(img);
		this->prevImageD = 0;
	}
	if (Sync::Interlocked::DecrementI32(useCnt) == 0)
	{
		Deinit(((UI::Win::WinCore*)this->ui.Ptr())->GetHInst());
	}
}

OSInt UI::Win::WinPictureBox::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}
