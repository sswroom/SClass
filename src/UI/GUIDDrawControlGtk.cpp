#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "Manage/HiResClock.h"
#include "Math/Math.h"
#include "Media/ImageUtil.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"
#include "UI/GUIClientControl.h"
#include "UI/GUIDDrawControl.h"
#include "UI/GUIForm.h"
#include <gtk/gtk.h>

struct UI::GUIDDrawControl::ClassData
{
	Bool pSurfaceUpdated;
	GtkWidget *imgCtrl;
	UInt8 *bufferSurfaceData;
};

gboolean GUIDDrawControl_ToGenDrawSignal(gpointer user_data)
{
	gtk_widget_queue_draw((GtkWidget*)user_data);
	return false;
}

gboolean GUIDDrawControl_OnDDDraw(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
	UI::GUIDDrawControl *me = (UI::GUIDDrawControl*)user_data;
	Sync::MutexUsage mutUsage;
	me->UseDrawSurface(&mutUsage);
	GdkPixbuf *pixBuf = (GdkPixbuf*)me->GetSurface();
	if (pixBuf)
	{
		gdk_cairo_set_source_pixbuf(cr, pixBuf, 0, 0);
		cairo_paint(cr);
//		printf("Draw begin\r\n");
/*		cairo_surface_t *s = gdk_cairo_surface_create_from_pixbuf(pixBuf, 0, gtk_widget_get_window(widget));
		cairo_save(cr);
		cairo_translate(cr, 0, 0);
		cairo_scale(cr, 1, 1);
		cairo_set_source_surface(cr, s, 0, 0);
		cairo_paint(cr);
		cairo_restore(cr);
		cairo_surface_destroy(s);*/
//		printf("Draw end\r\n");
	}
	else
	{
		printf("DDraw: Error in drawing\r\n");
	}
	me->UnuseDrawSurface(&mutUsage);
	return true;
}

gboolean GUIDDrawControl_OnMouseDown(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	UI::GUIDDrawControl *me = (UI::GUIDDrawControl*)data;
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
		me->OnMouseDown(Math::Double2Int32(evt->x), Math::Double2Int32(evt->y), btn);
	}
	return false;
}

gboolean GUIDDrawControl_OnMouseUp(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	UI::GUIDDrawControl *me = (UI::GUIDDrawControl*)data;
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
		me->OnMouseUp(Math::Double2Int32(evt->x), Math::Double2Int32(evt->y), btn);
	}
	return false;
}

gboolean GUIDDrawControl_OnMouseMove(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	UI::GUIDDrawControl *me = (UI::GUIDDrawControl*)data;
	GdkEventMotion *evt = (GdkEventMotion*)event;
	me->OnMouseMove(Math::Double2Int32(evt->x), Math::Double2Int32(evt->y));
	return false;
}

gboolean GUIDDrawControl_OnMouseWheel(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	UI::GUIDDrawControl *me = (UI::GUIDDrawControl*)data;
	GdkEventScroll *evt = (GdkEventScroll*)event;
	Int32 scrollY = 0;
	if (evt->direction == GDK_SCROLL_SMOOTH)
	{
		scrollY = Math::Double2Int32(-evt->delta_y);
	}
	else if (evt->direction == GDK_SCROLL_UP)
	{
		scrollY = -1;
	}
	else if (evt->direction == GDK_SCROLL_DOWN)
	{
		scrollY = 1;
	}
	me->OnMouseWheel(Math::Double2Int32(evt->x), Math::Double2Int32(evt->y), scrollY);
	return true;
}

typedef struct
{
	UI::GUIDDrawControl *me;
	GdkRectangle *allocation;
} ResizedData;

void GUIDDrawControl_OnResize(GtkWidget *widget, GdkRectangle *allocation, gpointer data)
{
	UI::GUIDDrawControl *me = (UI::GUIDDrawControl*)data;
	ResizedData rdata;
	rdata.me = me;
	rdata.allocation = allocation;
	UI::GUIDDrawControl::OnResized(&rdata);
}
/*gboolean GUIDDrawControl_OnKeyDown(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	UI::GUIDDrawControl *me = (UI::GUIDDrawControl*)user_data;
	GdkEventKey *evt = (GdkEventKey*)event;
	if (me->OnKeyDown(me->OSKey2GUIKey(evt->keyval)))
	{
		return true;
	}
	else
	{
		return false;
	}
}*/

void UI::GUIDDrawControl::Init(void *hInst)
{
}

void UI::GUIDDrawControl::Deinit(void *hInst)
{
}

void __stdcall UI::GUIDDrawControl::OnResized(void *userObj)
{
	ResizedData *data = (ResizedData*)userObj;
	if (data->me->switching)
		return;
	if (data->me->currScnMode == SM_FS)
	{
	}
	else
	{
		Sync::MutexUsage mutUsage(data->me->surfaceMut);
		data->me->surfaceW = (UOSInt)data->allocation->width;
		data->me->surfaceH = (UOSInt)data->allocation->height;
		data->me->ReleaseSubSurface();
		data->me->CreateSubSurface();
		mutUsage.EndUse();

		if (data->me->debugWriter)
		{
			Text::StringBuilderUTF8 sb;
			sb.Append((const UTF8Char*)"Surface size changed to ");
			sb.AppendUOSInt(data->me->surfaceW);
			sb.Append((const UTF8Char*)" x ");
			sb.AppendUOSInt(data->me->surfaceH);
			sb.Append((const UTF8Char*)", hMon=");
			sb.AppendOSInt((OSInt)data->me->GetHMonitor());
			data->me->debugWriter->WriteLine(sb.ToString());
		}
		if (data->me->inited)
		{
			data->me->OnSurfaceCreated();
		}
	}
}

void UI::GUIDDrawControl::GetDrawingRect(void *rc)
{
}

void UI::GUIDDrawControl::OnPaint()
{
	if (this->currScnMode != SM_FS && this->currScnMode != SM_VFS)
	{
		Sync::MutexUsage mutUsage(this->surfaceMut);
		DrawToScreen();
		mutUsage.EndUse();
	}
	else
	{
	}
}

Bool UI::GUIDDrawControl::CreateSurface()
{
	return false;
}

void UI::GUIDDrawControl::ReleaseSurface()
{
}

void UI::GUIDDrawControl::CreateSubSurface()
{
	if (this->pSurface)
	{
		return;
	}
	if (this->surfaceW <= 0 || this->surfaceH <= 0)
	{
		return;
	}
	this->surfaceBuff = MemAllocA(UInt8, this->surfaceW * this->surfaceH * 4);
	this->clsData->bufferSurfaceData = MemAllocA(UInt8, this->surfaceW * this->surfaceH * 4);
	GdkPixbuf *buf = gdk_pixbuf_new_from_data(this->clsData->bufferSurfaceData, GDK_COLORSPACE_RGB, true, 8, (int)(OSInt)this->surfaceW, (int)(OSInt)this->surfaceH, (int)(OSInt)this->surfaceW * 4, 0, 0);
	if (buf == 0)
	{
		MemFreeA(this->surfaceBuff);
		MemFreeA(this->clsData->bufferSurfaceData);
	}
	else
	{
		ImageUtil_ColorFill32(this->clsData->bufferSurfaceData, this->surfaceW * this->surfaceH, 0xff000000);
		g_object_ref(buf);
		this->pSurface = buf;
		this->clsData->pSurfaceUpdated = true;
	}
}

void UI::GUIDDrawControl::ReleaseSubSurface()
{
	gtk_image_clear((GtkImage*)this->surfaceMon);
	if (this->pSurface)
	{
		g_object_unref((GdkPixbuf*)this->pSurface);
		this->pSurface = 0;
		this->clsData->pSurfaceUpdated = true;
		MemFreeA(this->surfaceBuff);
		MemFreeA(this->clsData->bufferSurfaceData);
		this->surfaceBuff = 0;
		this->clsData->bufferSurfaceData = 0;
	}
}

Bool UI::GUIDDrawControl::CreateClipper(void *lpDD)
{
	return false;
}

UInt8 *UI::GUIDDrawControl::LockSurfaceBegin(UOSInt targetWidth, UOSInt targetHeight, UOSInt *bpl)
{
	if (this->surfaceW == targetWidth && this->surfaceH == targetHeight)
	{
		*bpl = this->surfaceW * 4;
		return (UInt8*)this->surfaceBuff;
	}
	return 0;
}

void UI::GUIDDrawControl::LockSurfaceEnd()
{
}

UInt8 *UI::GUIDDrawControl::LockSurfaceDirect(UOSInt *bpl)
{
	return 0;
}

void UI::GUIDDrawControl::LockSurfaceUnlock()
{
}

Media::PixelFormat UI::GUIDDrawControl::GetPixelFormat()
{
	return Media::PF_B8G8R8A8;
}

UI::GUIDDrawControl::GUIDDrawControl(GUICore *ui, UI::GUIClientControl *parent, Bool directMode, Media::ColorManagerSess *colorSess) : UI::GUIControl(ui, parent)
{
	this->clsData = MemAlloc(ClassData, 1);
	this->clsData->pSurfaceUpdated = true;
	this->clsData->bufferSurfaceData = 0;
	
	this->inited = false;
	this->clipper = 0;
	this->pSurface = 0;
	this->surfaceBuff = 0;
	this->imgCopy = 0;
	this->surfaceNoRelease = false;
	this->joystickId = 0;
	this->jsLastButtons = 0;
	this->focusing = false;
	NEW_CLASS(this->drawEvt, Sync::Event(false, (const UTF8Char*)"UI.GUIDDrawControl.drawEvt"));
	NEW_CLASS(this->surfaceMut, Sync::Mutex());
	this->rootForm = parent->GetRootForm();
	this->fullScnMode = SM_WINDOWED;
	this->directMode = directMode;
	this->switching = false;
	this->debugFS = 0;
	this->debugWriter = 0;
	this->bitDepth = 32;
	this->HandleSizeChanged(OnResized, this);
	this->currScnMode = SM_VFS;
	this->clipper = 0;
	this->clsData->imgCtrl = gtk_image_new();
	this->hwnd = (ControlHandle*)gtk_event_box_new();
	gtk_container_add((GtkContainer*)this->hwnd, this->clsData->imgCtrl);
	g_signal_connect(G_OBJECT(this->hwnd), "button-press-event", G_CALLBACK(GUIDDrawControl_OnMouseDown), this);
	g_signal_connect(G_OBJECT(this->hwnd), "button-release-event", G_CALLBACK(GUIDDrawControl_OnMouseUp), this);
	g_signal_connect(G_OBJECT(this->hwnd), "motion-notify-event", G_CALLBACK(GUIDDrawControl_OnMouseMove), this);
	g_signal_connect(G_OBJECT(this->hwnd), "scroll-event", G_CALLBACK(GUIDDrawControl_OnMouseWheel), this);
//	g_signal_connect(G_OBJECT(this->hwnd), "key-press-event", G_CALLBACK(GUIDDrawControl_OnKeyDown), this);
	g_signal_connect(G_OBJECT(this->hwnd), "size-allocate", G_CALLBACK(GUIDDrawControl_OnResize), this);
	gtk_widget_set_events((GtkWidget*)this->hwnd, GDK_ALL_EVENTS_MASK);
	gtk_widget_set_can_focus((GtkWidget*)this->hwnd, true);
	parent->AddChild(this);
	g_signal_connect(G_OBJECT(this->clsData->imgCtrl), "draw", G_CALLBACK(GUIDDrawControl_OnDDDraw), this);
	gtk_widget_show(this->clsData->imgCtrl);
	this->Show();
	this->inited = true;
}

UI::GUIDDrawControl::~GUIDDrawControl()
{
	this->ReleaseSurface();
	this->ReleaseSubSurface();

	DEL_CLASS(this->surfaceMut);
	DEL_CLASS(this->drawEvt);
	SDEL_CLASS(this->imgCopy);
	if (this->debugWriter)
	{
		this->debugWriter->WriteLine((const UTF8Char*)"Release DDraw");
		DEL_CLASS(this->debugWriter);
		DEL_CLASS(this->debugFS);
		this->debugFS = 0;
		this->debugWriter = 0;
	}
	MemFree(this->clsData);
}

void UI::GUIDDrawControl::SetUserFSMode(ScreenMode fullScnMode)
{
	this->fullScnMode = fullScnMode;
}

void UI::GUIDDrawControl::DrawToScreen()
{
	if (this->surfaceBuff && this->clsData->bufferSurfaceData)
	{
//		printf("Draw to screen 1\r\n");
		ImageUtil_ConvR8G8B8N8_ARGB32((const UInt8*)this->surfaceBuff, this->clsData->bufferSurfaceData, this->surfaceW, this->surfaceH, (OSInt)this->surfaceW * 4, (OSInt)this->surfaceW * 4);
		if (this->focusing)
		{

		}
		else
		{
			this->focusing = true;
			if (this->clsData->pSurfaceUpdated)
			{
				this->clsData->pSurfaceUpdated = false;
				g_idle_add(GUIDDrawControl_ToGenDrawSignal, this->surfaceMon);
			}
			else
			{
				g_idle_add(GUIDDrawControl_ToGenDrawSignal, this->surfaceMon);
			}
			while (gtk_events_pending())
			{
				gtk_main_iteration();
			}
		}
//		printf("Draw to screen 2\r\n");
	}
}

void UI::GUIDDrawControl::DrawFromBuff(UInt8 *buff, OSInt bpl, OSInt tlx, OSInt tly, UOSInt drawW, UOSInt drawH, Bool clearScn)
{
	Sync::MutexUsage mutUsage(this->surfaceMut);
	if (this->clsData->bufferSurfaceData)
	{
		if (this->focusing)
		{

		}
		else
		{
			if (buff[0] == 0 && buff[1] == 0 && buff[2] == 0)
			{
				clearScn = true;
			}
			if (tlx < 0)
			{
				drawW += (UOSInt)tlx;
				tlx = 0;
			}
			if (tly < 0)
			{
				drawH += (UOSInt)tly;
				tly = 0;
			}
			if (tlx + (OSInt)drawW > (OSInt)this->surfaceW)
			{
				drawW = this->surfaceW - (UOSInt)tlx;
			}
			if (tly + (OSInt)drawH > (OSInt)this->surfaceH)
			{
				drawH = this->surfaceH - (UOSInt)tly;
			}
			if ((OSInt)drawW > 0 && (OSInt)drawH > 0)
			{
				if (tly > 0)
				{
					ImageUtil_ColorFill32(this->clsData->bufferSurfaceData, this->surfaceW * (UOSInt)tly, 0xffcccccc);
				}
				if (tlx > 0)
				{
					ImageUtil_ImageColorFill32(this->clsData->bufferSurfaceData, (UOSInt)tlx, this->surfaceH, this->surfaceW * 4, 0xffcccccc);
				}
				if (tlx + (OSInt)drawW < (OSInt)this->surfaceW)
				{
					ImageUtil_ImageColorFill32(this->clsData->bufferSurfaceData + (tlx + (OSInt)drawW) * 4, (UOSInt)((OSInt)this->surfaceW - tlx - (OSInt)drawW), this->surfaceH, this->surfaceW * 4, 0xffcccccc);
				}
				ImageUtil_ConvR8G8B8N8_ARGB32(buff, tly * (OSInt)this->surfaceW * 4 + tlx * 4 + this->clsData->bufferSurfaceData, (UOSInt)drawW, (UOSInt)drawH, bpl, (OSInt)this->surfaceW * 4);
				if (tly + (OSInt)drawH < (OSInt)this->surfaceH)
				{
					ImageUtil_ColorFill32(this->clsData->bufferSurfaceData + (OSInt)this->surfaceW * 4 * (tly + (OSInt)drawH), this->surfaceW * (UOSInt)((OSInt)this->surfaceH - tly - (OSInt)drawH), 0xffcccccc);
				}
			}
			else
			{
				ImageUtil_ColorFill32(this->clsData->bufferSurfaceData, this->surfaceW * this->surfaceH, 0xffcccccc);
			}
//			Data::DateTime dt;
//			dt.SetCurrTimeUTC();
//			Int64 t = dt.ToTicks();
//			printf("Draw from buff 1 %lld\r\n", t);
			this->focusing = true;
			if (this->clsData->pSurfaceUpdated)
			{
				this->clsData->pSurfaceUpdated = false;
				g_idle_add(GUIDDrawControl_ToGenDrawSignal, this->surfaceMon);
			}
			else
			{
				g_idle_add(GUIDDrawControl_ToGenDrawSignal, this->surfaceMon);
			}
//			printf("Draw from buff 2 %lld\r\n", t);
		}
	}
	mutUsage.EndUse();
}

void UI::GUIDDrawControl::SwitchFullScreen(Bool fullScn, Bool vfs)
{
}

Bool UI::GUIDDrawControl::IsFullScreen()
{
	return this->currScnMode == SM_FS || this->currScnMode == SM_VFS;
}

void UI::GUIDDrawControl::ChangeMonitor(MonitorHandle *hMon)
{
	this->OnMonitorChanged();
}

UInt32 UI::GUIDDrawControl::GetRefreshRate()
{
	return 0;
}

void UI::GUIDDrawControl::OnMouseWheel(OSInt x, OSInt y, Int32 amount)
{
}

void UI::GUIDDrawControl::OnMouseMove(OSInt x, OSInt y)
{
}

void UI::GUIDDrawControl::OnMouseDown(OSInt x, OSInt y, MouseButton button)
{
}

void UI::GUIDDrawControl::OnMouseUp(OSInt x, OSInt y, MouseButton button)
{
}

void UI::GUIDDrawControl::OnMouseDblClick(OSInt x, OSInt y, MouseButton button)
{
	if (button == MBTN_LEFT)
	{
		if (this->fullScnMode == SM_WINDOWED || this->fullScnMode == SM_WINDOWED_DIR)
		{
		}
		else
		{
			if (this->currScnMode == SM_VFS || this->currScnMode == SM_FS)
			{
				this->SwitchFullScreen(false, false);
				this->OnSizeChanged(true);
			}
			else
			{
				this->SwitchFullScreen(true, this->fullScnMode == SM_VFS);
				this->OnSizeChanged(true);
			}
		}
	}
}

void UI::GUIDDrawControl::OnGZoomBegin(OSInt x, OSInt y, UInt64 dist)
{
}

void UI::GUIDDrawControl::OnGZoomStep(OSInt x, OSInt y, UInt64 dist)
{
}

void UI::GUIDDrawControl::OnGZoomEnd(OSInt x, OSInt y, UInt64 dist)
{
}

void UI::GUIDDrawControl::OnJSButtonDown(OSInt buttonId)
{
}

void UI::GUIDDrawControl::OnJSButtonUp(OSInt buttonId)
{
}

void UI::GUIDDrawControl::OnJSAxis(OSInt axis1, OSInt axis2, OSInt axis3, OSInt axis4)
{
}

void *UI::GUIDDrawControl::GetSurface()
{
	return this->pSurface;
}

void UI::GUIDDrawControl::UseDrawSurface(Sync::MutexUsage *mutUsage)
{
	mutUsage->ReplaceMutex(this->surfaceMut);
}

void UI::GUIDDrawControl::UnuseDrawSurface(Sync::MutexUsage *mutUsage)
{
	this->focusing = false;
	mutUsage->EndUse();
	this->drawEvt->Set();
}
