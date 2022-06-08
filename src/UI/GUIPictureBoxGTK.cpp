#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/ImageUtil.h"
#include "Media/Resizer/LanczosResizer8_C8.h"
#include "Sync/Interlocked.h"
#include "UI/GUIClientControl.h"
#include "UI/GUIPictureBox.h"
#include <gtk/gtk.h>

typedef struct
{
	GdkPixbuf *pixbuf;
	GtkWidget *gtkImage;
	GtkWidget *eventBox;
	Media::StaticImage *tmpImage;
} PictureBoxData;

gboolean GUIPictureBox_ButtonPress(GtkWidget *widget, GdkEvent  *event, gpointer user_data)
{
	UI::GUIPictureBox *me = (UI::GUIPictureBox*)user_data;
	GdkEventButton *evt = (GdkEventButton*)event;
	if (evt->type == GDK_BUTTON_PRESS)
	{
		UI::GUIControl::MouseButton btn;
		switch (evt->button)
		{
			default:
			case 1:
				btn = UI::GUIControl::MBTN_LEFT;
				break;
			case 2:
				btn = UI::GUIControl::MBTN_MIDDLE;
				break;
			case 3:
				btn = UI::GUIControl::MBTN_RIGHT;
				break;
			case 4:
				btn = UI::GUIControl::MBTN_X1;
				break;
			case 5:
				btn = UI::GUIControl::MBTN_X2;
				break;
		}
		me->EventButtonDown(Double2Int32(evt->x), Double2Int32(evt->y), btn);
	}
	return false;
}

gboolean GUIPictureBox_ButtonRelease(GtkWidget *widget, GdkEvent  *event, gpointer user_data)
{
	UI::GUIPictureBox *me = (UI::GUIPictureBox*)user_data;
	GdkEventButton *evt = (GdkEventButton*)event;
	if (evt->type == GDK_BUTTON_RELEASE)
	{
		UI::GUIControl::MouseButton btn;
		switch (evt->button)
		{
			default:
			case 1:
				btn = UI::GUIControl::MBTN_LEFT;
				break;
			case 2:
				btn = UI::GUIControl::MBTN_MIDDLE;
				break;
			case 3:
				btn = UI::GUIControl::MBTN_RIGHT;
				break;
			case 4:
				btn = UI::GUIControl::MBTN_X1;
				break;
			case 5:
				btn = UI::GUIControl::MBTN_X2;
				break;
		}
		me->EventButtonUp(Double2Int32(evt->x), Double2Int32(evt->y), btn);
	}
	return false;
}

gboolean GUIPictureBox_OnMouseMove(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	UI::GUIPictureBox *me = (UI::GUIPictureBox*)data;
	GdkEventMotion *evt = (GdkEventMotion*)event;
	me->EventMouseMove(Double2Int32(evt->x), Double2Int32(evt->y));
	return false;
}
/*OSInt __stdcall UI::GUIPictureBox::PBWndProc(void *hWnd, UInt32 msg, UInt32 wParam, OSInt lParam)
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

void UI::GUIPictureBox::OnPaint()
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

void UI::GUIPictureBox::UpdatePreview()
{
	PictureBoxData *data = (PictureBoxData*)this->clsData;
	gtk_image_set_from_pixbuf((GtkImage*)data->gtkImage, 0);
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
		if (this->allowResize)
		{
			Media::StaticImage *tmpImage = resizer->ProcessToNew(this->currImage);
			if (tmpImage)
			{
				GdkPixbuf *buf = gdk_pixbuf_new_from_data(tmpImage->data, GDK_COLORSPACE_RGB, tmpImage->info.storeBPP == 32, 8, (int)(OSInt)tmpImage->info.dispWidth, (int)(OSInt)tmpImage->info.dispHeight, (int)(OSInt)tmpImage->info.storeWidth << 2, 0, 0);
				guchar *pixels = gdk_pixbuf_get_pixels(buf);
				ImageUtil_SwapRGB(pixels, (UInt32)gdk_pixbuf_get_rowstride(buf) / 4 * this->currImage->info.dispHeight, 32);
				if (this->currImage->info.atype != Media::AT_ALPHA)
				{
					ImageUtil_ImageFillAlpha32(pixels, this->currImage->info.dispWidth, this->currImage->info.dispHeight, (UInt32)gdk_pixbuf_get_rowstride(buf), 255);
				}
				data->pixbuf = buf;
				data->tmpImage = tmpImage;
				gtk_image_set_from_pixbuf((GtkImage*)data->gtkImage, buf);
			}
		}
		else
		{
			GdkPixbuf *buf = gdk_pixbuf_new_from_data(this->currImage->data, GDK_COLORSPACE_RGB, this->currImage->info.storeBPP == 32, 8, (int)(OSInt)this->currImage->info.dispWidth, (int)(OSInt)this->currImage->info.dispHeight, (int)(OSInt)this->currImage->info.storeWidth << 2, 0, 0);
			guchar *pixels = gdk_pixbuf_get_pixels(buf);
			ImageUtil_SwapRGB(pixels, (UInt32)gdk_pixbuf_get_rowstride(buf) / 4 * this->currImage->info.dispHeight, 32);
			if (this->currImage->info.atype != Media::AT_ALPHA)
			{
				ImageUtil_ImageFillAlpha32(pixels, this->currImage->info.dispWidth, this->currImage->info.dispHeight, (UInt32)gdk_pixbuf_get_rowstride(buf), 255);
			}
			data->pixbuf = buf;
			gtk_image_set_from_pixbuf((GtkImage*)data->gtkImage, buf);
		}
	}
	this->Redraw();
}

UI::GUIPictureBox::GUIPictureBox(UI::GUICore *ui, UI::GUIClientControl *parent, Media::DrawEngine *eng, Bool hasBorder, Bool allowResize) : UI::GUIControl(ui, parent)
{
	this->hasBorder = hasBorder;
	this->allowResize = allowResize;
	this->eng = eng;
	this->currImage = 0;
	this->prevImageD = 0;
	this->noBGColor = false;
	PictureBoxData *data;
	data = MemAlloc(PictureBoxData, 1);
	data->pixbuf = 0;
	data->tmpImage = 0;
	this->clsData = data;

	Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
	NEW_CLASS(this->resizer, Media::Resizer::LanczosResizer8_C8(4, 3, &color, &color, 0, Media::AT_NO_ALPHA));
	this->resizer->SetResizeAspectRatio(Media::IImgResizer::RAR_SQUAREPIXEL);
	this->resizer->SetTargetWidth(200);
	this->resizer->SetTargetHeight(200);
	data->gtkImage = gtk_image_new_from_pixbuf(0);
	data->eventBox = gtk_event_box_new ();
	gtk_container_add(GTK_CONTAINER(data->eventBox), data->gtkImage);

	g_signal_connect(G_OBJECT(data->eventBox), "button-press-event", G_CALLBACK(GUIPictureBox_ButtonPress), this);
	g_signal_connect(G_OBJECT(data->eventBox), "button-release-event", G_CALLBACK(GUIPictureBox_ButtonRelease), this);
	g_signal_connect(G_OBJECT(data->eventBox), "motion-notify-event", G_CALLBACK(GUIPictureBox_OnMouseMove), this);

	this->hwnd = (ControlHandle*)data->eventBox;
	parent->AddChild(this);
	gtk_widget_show((GtkWidget*)data->gtkImage);
	this->Show();
}

UI::GUIPictureBox::~GUIPictureBox()
{
	DEL_CLASS(this->resizer);
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

Text::CString UI::GUIPictureBox::GetObjectClass()
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
	Math::Size2D<UOSInt> sz;
	sz = GetSizeP();
	this->resizer->SetTargetWidth(sz.width);
	this->resizer->SetTargetHeight(sz.height);
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
