#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/Interlocked.h"
#include "Media/GDIEngine.h"
#include "Media/Resizer/LanczosResizer8_C8.h"
#include "UI/GUIClientControl.h"
#include "UI/GUIPictureBox.h"
#include "UI/Win/WinCore.h"
#include <windows.h>

#define CLASSNAME L"PictureBox"
Int32 UI::GUIPictureBox::useCnt = 0;

#ifndef GWL_USERDATA
#define GWL_USERDATA GWLP_USERDATA
#endif

OSInt __stdcall UI::GUIPictureBox::PBWndProc(void *hWnd, UInt32 msg, UInt32 wParam, OSInt lParam)
{
	UI::GUIPictureBox *me = (UI::GUIPictureBox*)(OSInt)GetWindowLongPtr((HWND)hWnd, GWL_USERDATA);
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
		me->EventButtonDown((Int16)LOWORD(lParam), (Int16)HIWORD(lParam), MBTN_LEFT);
		return 0;
	case WM_RBUTTONDOWN:
		me->EventButtonDown((Int16)LOWORD(lParam), (Int16)HIWORD(lParam), MBTN_RIGHT);
		return 0;
	case WM_MBUTTONDOWN:
		me->EventButtonDown((Int16)LOWORD(lParam), (Int16)HIWORD(lParam), MBTN_MIDDLE);
		return 0;
	case WM_LBUTTONUP:
		me->EventButtonUp((Int16)LOWORD(lParam), (Int16)HIWORD(lParam), MBTN_LEFT);
		return 0;
	case WM_RBUTTONUP:
		me->EventButtonUp((Int16)LOWORD(lParam), (Int16)HIWORD(lParam), MBTN_RIGHT);
		return 0;
	case WM_MBUTTONUP:
		me->EventButtonUp((Int16)LOWORD(lParam), (Int16)HIWORD(lParam), MBTN_MIDDLE);
		return 0;
	case WM_MOUSEMOVE:
		me->EventMouseMove((Int16)LOWORD(lParam), (Int16)HIWORD(lParam));
		return 0;
	}
	return DefWindowProc((HWND)hWnd, msg, wParam, lParam);
}

void UI::GUIPictureBox::OnPaint()
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

		if (this->prevImageD->GetAlphaType() == Media::AT_ALPHA)
		{
			BLENDFUNCTION bf;
			bf.SourceConstantAlpha = 255;
			bf.AlphaFormat = AC_SRC_ALPHA;
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			AlphaBlend(ps.hdc, (int)x, (int)y, (int)this->prevImageD->GetWidth(), (int)this->prevImageD->GetHeight(), (HDC)((Media::GDIImage*)this->prevImageD)->GetHDC(), 0, 0, (int)this->prevImageD->GetWidth(), (int)this->prevImageD->GetHeight(), bf);
		}
		else
		{
			BitBlt(ps.hdc, (int)x, (int)y, (int)this->prevImageD->GetWidth(), (int)this->prevImageD->GetHeight(), (HDC)((Media::GDIImage*)this->prevImageD)->GetHDC(), 0, 0, SRCCOPY);
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
	EndPaint((HWND)this->hwnd, &ps);
}

void UI::GUIPictureBox::Init(void *hInst)
{
	WNDCLASSW wc;
    wc.style = 0; 
	wc.lpfnWndProc = (WNDPROC)UI::GUIPictureBox::PBWndProc; 
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

void UI::GUIPictureBox::Deinit(void *hInst)
{
	UnregisterClassW(CLASSNAME, (HINSTANCE)hInst);
}

void UI::GUIPictureBox::UpdatePreview()
{
	NotNullPtr<Media::DrawImage> img;
	NotNullPtr<Media::StaticImage> simg;
	if (img.Set(this->prevImageD))
	{
		this->eng->DeleteImage(img);
		this->prevImageD = 0;
	}

	if (simg.Set(this->currImage))
	{
		if (this->allowResize)
		{
			NotNullPtr<Media::StaticImage> tmpImage;
			if (tmpImage.Set(resizer->ProcessToNew(simg)))
			{
				this->prevImageD = this->eng->ConvImage(tmpImage);
				tmpImage.Delete();
			}
			else
			{
				this->prevImageD = this->eng->ConvImage(simg);
			}
		}
		else
		{
			this->prevImageD = this->eng->ConvImage(simg);
		}
	}
	this->Redraw();
}

UI::GUIPictureBox::GUIPictureBox(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, NotNullPtr<Media::DrawEngine> eng, Bool hasBorder, Bool allowResize) : UI::GUIControl(ui, parent)
{
	this->hasBorder = hasBorder;
	this->allowResize = allowResize;
	this->eng = eng;
	this->currImage = 0;
	this->prevImageD = 0;
	this->noBGColor = false;

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
	Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
	NEW_CLASS(this->resizer, Media::Resizer::LanczosResizer8_C8(4, 3, color, color, 0, Media::AT_NO_ALPHA));
	this->resizer->SetResizeAspectRatio(Media::IImgResizer::RAR_SQUAREPIXEL);
	this->resizer->SetTargetSize(Math::Size2D<UOSInt>(200, 200));
}

UI::GUIPictureBox::~GUIPictureBox()
{
	NotNullPtr<Media::DrawImage> img;
	DEL_CLASS(this->resizer);
	if (img.Set(this->prevImageD))
	{
		this->eng->DeleteImage(img);
		this->prevImageD = 0;
	}
	if (Sync::Interlocked::DecrementI32(useCnt) == 0)
	{
		Deinit(((UI::Win::WinCore*)this->ui.Ptr())->GetHInst());
	}
}

Text::CStringNN UI::GUIPictureBox::GetObjectClass() const
{
	return CSTR("PictureBox");
}

OSInt UI::GUIPictureBox::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GUIPictureBox::OnSizeChanged(Bool updateScn)
{
	UI::GUIControl::OnSizeChanged(updateScn);
	Math::Size2D<UOSInt> sz = GetSizeP();
	this->resizer->SetTargetSize(sz);
	if (this->allowResize)
	{
		this->UpdatePreview();
	}
}

void UI::GUIPictureBox::HandleMouseDown(MouseEventHandler hdlr, void *userObj)
{
	this->mouseDownHdlrs.Add(hdlr);
	this->mouseDownObjs.Add(userObj);
}

void UI::GUIPictureBox::HandleMouseMove(MouseEventHandler hdlr, void *userObj)
{
	this->mouseMoveHdlrs.Add(hdlr);
	this->mouseMoveObjs.Add(userObj);
}

void UI::GUIPictureBox::HandleMouseUp(MouseEventHandler hdlr, void *userObj)
{
	this->mouseUpHdlrs.Add(hdlr);
	this->mouseUpObjs.Add(userObj);
}

void UI::GUIPictureBox::SetImage(Media::StaticImage *currImage)
{
	this->currImage = currImage;
	this->UpdatePreview();
}

void UI::GUIPictureBox::SetNoBGColor(Bool noBGColor)
{
	this->noBGColor = noBGColor;
}

void UI::GUIPictureBox::EventButtonDown(OSInt x, OSInt y, UI::GUIControl::MouseButton btn)
{
	UOSInt i;
	i = this->mouseDownHdlrs.GetCount();
	while (i-- > 0)
	{
		this->mouseDownHdlrs.GetItem(i)(this->mouseDownObjs.GetItem(i), Math::Coord2D<OSInt>(x, y), btn);
	}
}

void UI::GUIPictureBox::EventButtonUp(OSInt x, OSInt y, UI::GUIControl::MouseButton btn)
{
	UOSInt i;
	i = this->mouseUpHdlrs.GetCount();
	while (i-- > 0)
	{
		this->mouseUpHdlrs.GetItem(i)(this->mouseUpObjs.GetItem(i), Math::Coord2D<OSInt>(x, y), btn);
	}
}

void UI::GUIPictureBox::EventMouseMove(OSInt x, OSInt y)
{
	UOSInt i = this->mouseMoveHdlrs.GetCount();
	while (i-- > 0)
	{
		this->mouseMoveHdlrs.GetItem(i)(this->mouseMoveObjs.GetItem(i), Math::Coord2D<OSInt>(x, y), MBTN_MIDDLE);
	}
}