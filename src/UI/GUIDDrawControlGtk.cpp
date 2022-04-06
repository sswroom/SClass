#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "Manage/HiResClock.h"
#include "Math/Math.h"
#include "Media/FBMonitorSurfaceMgr.h"
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
	UOSInt drawPause;
	GdkPixbuf *pixBuf;
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
	GdkPixbuf *pixBuf = (GdkPixbuf*)me->GetPixBuf();
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
		me->OnMouseDown(Double2Int32(evt->x), Double2Int32(evt->y), btn);
	}
	else if (evt->type == GDK_DOUBLE_BUTTON_PRESS)
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
		me->OnMouseDblClick(Double2Int32(evt->x), Double2Int32(evt->y), btn);
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
		me->OnMouseUp(Double2Int32(evt->x), Double2Int32(evt->y), btn);
	}
	return false;
}

gboolean GUIDDrawControl_OnMouseMove(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	UI::GUIDDrawControl *me = (UI::GUIDDrawControl*)data;
	GdkEventMotion *evt = (GdkEventMotion*)event;
	me->OnMouseMove(Double2Int32(evt->x), Double2Int32(evt->y));
	return false;
}

gboolean GUIDDrawControl_OnMouseWheel(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	UI::GUIDDrawControl *me = (UI::GUIDDrawControl*)data;
	GdkEventScroll *evt = (GdkEventScroll*)event;
	Int32 scrollY = 0;
	if (evt->direction == GDK_SCROLL_SMOOTH)
	{
		scrollY = Double2Int32(-evt->delta_y);
	}
	else if (evt->direction == GDK_SCROLL_UP)
	{
		scrollY = -1;
	}
	else if (evt->direction == GDK_SCROLL_DOWN)
	{
		scrollY = 1;
	}
	me->OnMouseWheel(Double2Int32(evt->x), Double2Int32(evt->y), scrollY);
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
			sb.AppendC(UTF8STRC("Surface size changed to "));
			sb.AppendUOSInt(data->me->surfaceW);
			sb.AppendC(UTF8STRC(" x "));
			sb.AppendUOSInt(data->me->surfaceH);
			sb.AppendC(UTF8STRC(", hMon="));
			sb.AppendOSInt((OSInt)data->me->GetHMonitor());
			data->me->debugWriter->WriteLineC(sb.ToString(), sb.GetLength());
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
/*	this->ReleaseSurface();
	this->ReleaseSubSurface();

	if (this->debugWriter)
	{
		this->debugWriter->WriteLineC(UTF8STRC("Create Surface");
	}

	if (this->currScnMode == SM_FS)
	{
		this->surfaceMon = this->GetHMonitor();
		Bool succ = this->surfaceMgr->CreatePrimarySurfaceWithBuffer(this->surfaceMon, &this->primarySurface, &this->buffSurface);
		if (succ)
		{
			this->bitDepth = this->primarySurface->info.storeBPP;
			this->scnW = this->primarySurface->info.dispWidth;
			this->scnH = this->primarySurface->info.dispHeight;
		}
		return succ;
	}
	else
	{
		ControlHandle *hWnd;
		if (this->currScnMode == SM_VFS)
		{
			this->surfaceMon = this->GetHMonitor();
			hWnd = 0;
		}
		else if (this->currScnMode == SM_WINDOWED_DIR)
		{
			this->surfaceMon = this->GetHMonitor();
			hWnd = this->GetHandle();
		}
		else
		{
			this->surfaceMon = 0;
			hWnd = this->GetHandle();
		}
		this->primarySurface = this->surfaceMgr->CreatePrimarySurface(this->surfaceMon, hWnd);
		if (this->primarySurface)
		{
			if (this->debugWriter)
			{
				Text::StringBuilderUTF8 sb;
				sb.AppendC(UTF8STRC("Primary surface desc: Size = "));
				sb.AppendUOSInt(this->primarySurface->info.dispWidth);
				sb.AppendC(UTF8STRC(" x "));
				sb.AppendUOSInt(this->primarySurface->info.dispHeight);
				sb.AppendC(UTF8STRC(", bpl = "));
				sb.AppendUOSInt(this->primarySurface->GetDataBpl());
				sb.AppendC(UTF8STRC(", hMon = "));
				sb.AppendOSInt((OSInt)this->surfaceMon);
				sb.AppendC(UTF8STRC(", hWnd = "));
				sb.AppendOSInt((OSInt)hWnd);
				this->debugWriter->WriteLineC(sb.ToString(), sb.GetLength());
			}
			this->bitDepth = this->primarySurface->info.storeBPP;
			this->scnW = this->primarySurface->info.dispWidth;
			this->scnH = this->primarySurface->info.dispHeight;

			CreateSubSurface();
			return true;
		}
		else
		{
			return false;
		}
	}*/
	return false;
}

void UI::GUIDDrawControl::ReleaseSurface()
{
//	SDEL_CLASS(this->primarySurface);
}

void UI::GUIDDrawControl::CreateSubSurface()
{
	if (this->primarySurface)
	{
		return;
	}
	if (this->surfaceW <= 0 || this->surfaceH <= 0)
	{
		return;
	}
	this->primarySurface = this->surfaceMgr->CreateSurface(this->surfaceW, this->surfaceH, 32);
//	this->primarySurface = this->surfaceMgr->CreatePrimarySurface(this->GetHMonitor(), 0);
	this->buffSurface = this->surfaceMgr->CreateSurface(this->surfaceW, this->surfaceH, 32);
	GdkPixbuf *buf = gdk_pixbuf_new_from_data((const guchar*)this->primarySurface->GetHandle(), GDK_COLORSPACE_RGB, true, 8, (int)(OSInt)this->surfaceW, (int)(OSInt)this->surfaceH, (int)(OSInt)this->surfaceW * 4, 0, 0);
//	GdkPixbuf *buf = gdk_pixbuf_new_from_data((const guchar*)this->buffSurface->GetHandle(), GDK_COLORSPACE_RGB, true, 8, (int)(OSInt)this->surfaceW, (int)(OSInt)this->surfaceH, (int)(OSInt)this->surfaceW * 4, 0, 0);
	if (buf == 0)
	{
		DEL_CLASS(this->primarySurface);
		DEL_CLASS(this->buffSurface);
		this->primarySurface = 0;
		this->buffSurface = 0;
	}
	else
	{
		this->primarySurface->info.atype = Media::AT_ALPHA;
		OSInt lineAdd;
		ImageUtil_ColorFill32((UInt8*)this->primarySurface->LockSurface(&lineAdd), this->surfaceW * this->surfaceH, 0xff000000);
		this->clsData->pixBuf = buf;
		this->clsData->pSurfaceUpdated = true;
	}
}

void UI::GUIDDrawControl::ReleaseSubSurface()
{
	if (this->clsData->imgCtrl)
	{
		gtk_image_clear((GtkImage*)this->clsData->imgCtrl);
	}
	if (this->primarySurface)
	{
		g_object_unref(this->clsData->pixBuf);
		this->clsData->pixBuf = 0;
		this->clsData->pSurfaceUpdated = true;
		DEL_CLASS(this->buffSurface);
		DEL_CLASS(this->primarySurface);
		this->buffSurface = 0;
		this->primarySurface = 0;
	}
}

UInt8 *UI::GUIDDrawControl::LockSurfaceBegin(UOSInt targetWidth, UOSInt targetHeight, UOSInt *bpl)
{
	this->surfaceMut->Lock();
	if (this->buffSurface == 0)
	{
		this->surfaceMut->Unlock();
		return 0;
	}
	if (targetWidth == this->surfaceW && targetHeight == this->surfaceH)
	{
		UInt8 *dptr = this->buffSurface->LockSurface((OSInt*)bpl);
		if (dptr)
		{
			return dptr;
		}
	}
	this->surfaceMut->Unlock();
	return 0;
}

void UI::GUIDDrawControl::LockSurfaceEnd()
{
	this->buffSurface->UnlockSurface();
	this->surfaceMut->Unlock();
}

Media::PixelFormat UI::GUIDDrawControl::GetPixelFormat()
{
	return Media::PF_B8G8R8A8;
}

void UI::GUIDDrawControl::BeginUpdateSize()
{
	this->switching = true;
}

void UI::GUIDDrawControl::EndUpdateSize()
{
	this->switching = false;
}

UI::GUIDDrawControl::GUIDDrawControl(GUICore *ui, UI::GUIClientControl *parent, Bool directMode, Media::ColorManagerSess *colorSess) : UI::GUIControl(ui, parent)
{
	this->clsData = MemAlloc(ClassData, 1);
	this->clsData->pSurfaceUpdated = true;
	this->clsData->drawPause = 0;
	
	NEW_CLASS(this->surfaceMgr, Media::FBMonitorSurfaceMgr(ui->GetMonitorMgr(), colorSess));
	this->inited = false;
	this->primarySurface = 0;
	this->buffSurface = 0;
	this->surfaceW = 0;
	this->surfaceH = 0;
	this->imgCopy = 0;
	this->joystickId = 0;
	this->jsLastButtons = 0;
	NEW_CLASS(this->drawEvt, Sync::Event(false));
	NEW_CLASS(this->surfaceMut, Sync::Mutex());
	this->rootForm = parent->GetRootForm();
	this->fullScnMode = SM_WINDOWED;
	this->directMode = directMode;
	this->switching = false;
	this->debugFS = 0;
	this->debugWriter = 0;
	this->bitDepth = 32;
	this->HandleSizeChanged(OnResized, this);
	this->currScnMode = SM_WINDOWED;
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
	this->clsData->imgCtrl = 0;
	this->ReleaseSubSurface();

	DEL_CLASS(this->surfaceMut);
	DEL_CLASS(this->drawEvt);
	SDEL_CLASS(this->imgCopy);
	if (this->debugWriter)
	{
		this->debugWriter->WriteLineC(UTF8STRC("Release DDraw"));
		DEL_CLASS(this->debugWriter);
		DEL_CLASS(this->debugFS);
		this->debugFS = 0;
		this->debugWriter = 0;
	}
	DEL_CLASS(this->surfaceMgr);
	MemFree(this->clsData);
}

void UI::GUIDDrawControl::SetUserFSMode(ScreenMode fullScnMode)
{
	this->fullScnMode = fullScnMode;
}

void UI::GUIDDrawControl::DrawToScreen()
{
	if (this->primarySurface && this->buffSurface)
	{
		if (this->clsData->drawPause)
		{
			this->clsData->drawPause--;
		}
		else
		{
			this->primarySurface->DrawFromSurface(this->buffSurface, true);
//			this->clsData->drawPause = 0;
			this->clsData->drawPause = 10;
			if (this->clsData->pSurfaceUpdated)
			{
				this->clsData->pSurfaceUpdated = false;
				g_idle_add(GUIDDrawControl_ToGenDrawSignal, this->clsData->imgCtrl);
			}
			else
			{
				g_idle_add(GUIDDrawControl_ToGenDrawSignal, this->clsData->imgCtrl);
			}
			while (gtk_events_pending())
			{
				gtk_main_iteration();
			}
		}
//		printf("Draw to screen 2\r\n");
	}
}

void UI::GUIDDrawControl::DrawFromBuff(UInt8 *buff, OSInt lineAdd, OSInt tlx, OSInt tly, UOSInt drawW, UOSInt drawH, Bool clearScn)
{
	Sync::MutexUsage mutUsage(this->surfaceMut);
	if (this->primarySurface)
	{
		if (this->clsData->drawPause)
		{
			this->clsData->drawPause--;
		}
		else
		{
			this->primarySurface->DrawFromMem(buff, lineAdd, tlx, tly, drawW, drawH, clearScn, true);
//			this->clsData->drawPause = 0;
//			Data::DateTime dt;
//			dt.SetCurrTimeUTC();
//			Int64 t = dt.ToTicks();
//			printf("Draw from buff 1 %lld\r\n", t);
			this->clsData->drawPause = 10;
			if (this->clsData->pSurfaceUpdated)
			{
				this->clsData->pSurfaceUpdated = false;
				g_idle_add(GUIDDrawControl_ToGenDrawSignal, this->clsData->imgCtrl);
			}
			else
			{
				g_idle_add(GUIDDrawControl_ToGenDrawSignal, this->clsData->imgCtrl);
			}
//			printf("Draw from buff 2 %lld\r\n", t);
		}
	}
	mutUsage.EndUse();
}

void UI::GUIDDrawControl::SwitchFullScreen(Bool fullScn, Bool vfs)
{
	if (fullScn)
	{
		this->GetRootForm()->ToFullScn();
		this->currScnMode = SM_VFS;
	}
	else
	{
		this->GetRootForm()->FromFullScn();
		this->currScnMode = SM_WINDOWED;
	}
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

Bool UI::GUIDDrawControl::IsSurfaceReady()
{
	return this->buffSurface != 0;
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

void *UI::GUIDDrawControl::GetPixBuf()
{
	return this->clsData->pixBuf;
}

void UI::GUIDDrawControl::UseDrawSurface(Sync::MutexUsage *mutUsage)
{
	mutUsage->ReplaceMutex(this->surfaceMut);
}

void UI::GUIDDrawControl::UnuseDrawSurface(Sync::MutexUsage *mutUsage)
{
	this->clsData->drawPause = 0;
	mutUsage->EndUse();
	this->drawEvt->Set();
}
