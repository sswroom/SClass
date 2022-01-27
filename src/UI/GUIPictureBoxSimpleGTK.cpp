#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/Interlocked.h"
#include "Media/GTKDrawEngine.h"
#include "UI/GUIClientControl.h"
#include "UI/GUIPictureBoxSimple.h"
#include <gtk/gtk.h>

typedef struct
{
	GdkPixbuf *pixbuf;
	Media::StaticImage *tmpImage;
} PictureBoxData;

/*OSInt __stdcall UI::GUIPictureBoxSimple::PBWndProc(void *hWnd, UInt32 msg, UInt32 wParam, OSInt lParam)
{
	UI::GUIPictureBox *me = (UI::GUIPictureBox*)(OSInt)GetWindowLongPtr((HWND)hWnd, GWL_USERDATA);
	OSInt i;
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
		i = me->mouseDownHdlrs->GetCount();
		while (i-- > 0)
		{
			me->mouseDownHdlrs->GetItem(i)(me->mouseDownObjs->GetItem(i), (Int16)LOWORD(lParam), (Int16)HIWORD(lParam), MBTN_LEFT);
		}
		return 0;
	case WM_RBUTTONDOWN:
		i = me->mouseDownHdlrs->GetCount();
		while (i-- > 0)
		{
			me->mouseDownHdlrs->GetItem(i)(me->mouseDownObjs->GetItem(i), (Int16)LOWORD(lParam), (Int16)HIWORD(lParam), MBTN_RIGHT);
		}
		return 0;
	case WM_MBUTTONDOWN:
		i = me->mouseDownHdlrs->GetCount();
		while (i-- > 0)
		{
			me->mouseDownHdlrs->GetItem(i)(me->mouseDownObjs->GetItem(i), (Int16)LOWORD(lParam), (Int16)HIWORD(lParam), MBTN_MIDDLE);
		}
		return 0;
	case WM_LBUTTONUP:
		i = me->mouseUpHdlrs->GetCount();
		while (i-- > 0)
		{
			me->mouseUpHdlrs->GetItem(i)(me->mouseUpObjs->GetItem(i), (Int16)LOWORD(lParam), (Int16)HIWORD(lParam), MBTN_LEFT);
		}
		return 0;
	case WM_RBUTTONUP:
		i = me->mouseUpHdlrs->GetCount();
		while (i-- > 0)
		{
			me->mouseUpHdlrs->GetItem(i)(me->mouseUpObjs->GetItem(i), (Int16)LOWORD(lParam), (Int16)HIWORD(lParam), MBTN_RIGHT);
		}
		return 0;
	case WM_MBUTTONUP:
		i = me->mouseUpHdlrs->GetCount();
		while (i-- > 0)
		{
			me->mouseUpHdlrs->GetItem(i)(me->mouseUpObjs->GetItem(i), (Int16)LOWORD(lParam), (Int16)HIWORD(lParam), MBTN_MIDDLE);
		}
		return 0;
	case WM_MOUSEMOVE:
		i = me->mouseMoveHdlrs->GetCount();
		while (i-- > 0)
		{
			me->mouseMoveHdlrs->GetItem(i)(me->mouseMoveObjs->GetItem(i), (Int16)LOWORD(lParam), (Int16)HIWORD(lParam), MBTN_MIDDLE);
		}
		return 0;
	}
	return DefWindowProc((HWND)hWnd, msg, wParam, lParam);
}*/

void UI::GUIPictureBoxSimple::OnPaint()
{
/*	HGDIOBJ lastObj;
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
		OSInt x = (rc.right - rc.left - this->prevImageD->GetWidth()) >> 1;
		OSInt y = (rc.bottom - rc.top - this->prevImageD->GetHeight()) >> 1;

		if (this->prevImageD->GetAlphaType() == Media::AT_ALPHA)
		{
			BLENDFUNCTION bf;
			bf.SourceConstantAlpha = 255;
			bf.AlphaFormat = AC_SRC_ALPHA;
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			AlphaBlend(ps.hdc, x, y, this->prevImageD->GetWidth(), this->prevImageD->GetHeight(), (HDC)((Media::GDIImage*)this->prevImageD)->GetHDC(), 0, 0, this->prevImageD->GetWidth(), this->prevImageD->GetHeight(), bf);
		}
		else
		{
			BitBlt(ps.hdc, x, y, this->prevImageD->GetWidth(), this->prevImageD->GetHeight(), (HDC)((Media::GDIImage*)this->prevImageD)->GetHDC(), 0, 0, SRCCOPY);
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
	EndPaint((HWND)this->hwnd, &ps);*/
}

void UI::GUIPictureBoxSimple::UpdatePreview()
{
	PictureBoxData *data = (PictureBoxData*)this->clsData;
	gtk_image_clear((GtkImage*)this->hwnd);
	if (data->pixbuf)
	{
		g_object_unref(data->pixbuf);
		data->pixbuf = 0;
	}
	if (data->tmpImage)
	{
		DEL_CLASS(data->tmpImage);
		data->tmpImage = 0;
	}
	if (this->currImage)
	{
		GdkPixbuf *buf;
		if (this->currImage->info->atype == Media::AT_ALPHA)
		{
			buf = gdk_pixbuf_new_from_data(this->currImage->data, GDK_COLORSPACE_RGB, true, 8, (int)(UInt32)this->currImage->info->dispWidth, (int)(UInt32)this->currImage->info->dispHeight, (int)(UInt32)(this->currImage->info->storeWidth << 2), 0, 0);
		}
		else if (this->currImage->info->atype == Media::AT_PREMUL_ALPHA)
		{
			buf = gdk_pixbuf_new_from_data(this->currImage->data, GDK_COLORSPACE_RGB, true, 8, (int)(UInt32)this->currImage->info->dispWidth, (int)(UInt32)this->currImage->info->dispHeight, (int)(UInt32)(this->currImage->info->storeWidth << 2), 0, 0);
		}
		else
		{
			buf = gdk_pixbuf_new_from_data(this->currImage->data, GDK_COLORSPACE_RGB, true, 8, (int)(UInt32)this->currImage->info->dispWidth, (int)(UInt32)this->currImage->info->dispHeight, (int)(UInt32)(this->currImage->info->storeWidth << 2), 0, 0);
		}
		data->pixbuf = buf;
		gtk_image_set_from_pixbuf((GtkImage*)this->hwnd, buf);
	}
	else if (this->prevImageD)
	{
		gtk_image_set_from_surface((GtkImage*)this->hwnd, (cairo_surface_t*)((Media::GTKDrawImage*)this->prevImageD)->GetSurface());
	}
	this->Redraw();
}

UI::GUIPictureBoxSimple::GUIPictureBoxSimple(UI::GUICore *ui, UI::GUIClientControl *parent, Media::DrawEngine *eng, Bool hasBorder) : UI::GUIControl(ui, parent)
{
	this->hasBorder = hasBorder;
	this->eng = eng;
	this->currImage = 0;
	this->prevImageD = 0;
	this->noBGColor = false;

	PictureBoxData *data;
	data = MemAlloc(PictureBoxData, 1);
	data->pixbuf = 0;
	data->tmpImage = 0;
	this->clsData = data;

	NEW_CLASS(this->mouseDownHdlrs, Data::ArrayList<MouseEventHandler>());
	NEW_CLASS(this->mouseDownObjs, Data::ArrayList<void *>());
	NEW_CLASS(this->mouseMoveHdlrs, Data::ArrayList<MouseEventHandler>());
	NEW_CLASS(this->mouseMoveObjs, Data::ArrayList<void *>());
	NEW_CLASS(this->mouseUpHdlrs, Data::ArrayList<MouseEventHandler>());
	NEW_CLASS(this->mouseUpObjs, Data::ArrayList<void *>());
	this->hwnd = (ControlHandle*)gtk_image_new();
	parent->AddChild(this);
	this->Show();
}

UI::GUIPictureBoxSimple::~GUIPictureBoxSimple()
{
	DEL_CLASS(this->mouseDownHdlrs);
	DEL_CLASS(this->mouseDownObjs);
	DEL_CLASS(this->mouseMoveHdlrs);
	DEL_CLASS(this->mouseMoveObjs);
	DEL_CLASS(this->mouseUpHdlrs);
	DEL_CLASS(this->mouseUpObjs);
	if (this->prevImageD)
	{
		this->eng->DeleteImage(this->prevImageD);
		this->prevImageD = 0;
	}
	PictureBoxData *data = (PictureBoxData*)this->clsData;
	if (data->pixbuf)
	{
		g_object_unref(data->pixbuf);
		data->pixbuf = 0;
	}
	if (data->tmpImage)
	{
		DEL_CLASS(data->tmpImage);
		data->tmpImage = 0;
	}
	MemFree(data);
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
	this->mouseDownHdlrs->Add(hdlr);
	this->mouseDownObjs->Add(userObj);
}

void UI::GUIPictureBoxSimple::HandleMouseMove(MouseEventHandler hdlr, void *userObj)
{
	this->mouseMoveHdlrs->Add(hdlr);
	this->mouseMoveObjs->Add(userObj);
}

void UI::GUIPictureBoxSimple::HandleMouseUp(MouseEventHandler hdlr, void *userObj)
{
	this->mouseUpHdlrs->Add(hdlr);
	this->mouseUpObjs->Add(userObj);
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
		this->prevImageD = this->eng->CloneImage(img);
		if (this->prevImageD == 0)
		{
		}
		else
		{
			gtk_image_set_from_surface((GtkImage*)this->hwnd, (cairo_surface_t*)((Media::GTKDrawImage*)this->prevImageD)->GetSurface());
		}
	}
	this->Redraw();
}

void UI::GUIPictureBoxSimple::SetNoBGColor(Bool noBGColor)
{
	this->noBGColor = noBGColor;
}
