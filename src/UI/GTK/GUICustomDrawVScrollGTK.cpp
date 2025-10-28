#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Library.h"
#include "Math/Math_C.h"
#include "Math/Geometry/Ellipse.h"
#include "Math/Geometry/PieArea.h"
#include "Math/Geometry/Polyline.h"
#include "Media/GTKDrawEngine.h"
#include "Sync/Interlocked.h"
#include "UI/GUIClientControl.h"
#include "UI/GUICustomDrawVScroll.h"
#include <gtk/gtk.h>

#define SCROLLWIDTH 8

struct UI::GUICustomDrawVScroll::ClassData
{
	UOSInt currPos;
	UOSInt min;
	UOSInt max;
	UOSInt pageSize;
	Bool scrollDown;
	OSInt scrollDownY;
	UOSInt scrollDownPos;
	OSInt scrollSize;
};

Int32 UI::GUICustomDrawVScroll::useCnt = 0;

gboolean GUICustomDrawVScroll_OnDraw(GtkWidget *widget, cairo_t *cr, gpointer data)
{
	UI::GUICustomDrawVScroll *me = (UI::GUICustomDrawVScroll*)data;
	NN<UI::GUICustomDrawVScroll::ClassData> clsData = me->clsData;
	clsData->scrollSize = Double2OSInt(8 * me->GetHDPI() / me->GetDDPI());
	OSInt width = gtk_widget_get_allocated_width(widget);
	OSInt height = gtk_widget_get_allocated_height(widget);
	if ((clsData->max - clsData->min) > clsData->pageSize)
	{
		NN<Media::DrawImage> dimg = ((Media::GTKDrawEngine*)me->deng.Ptr())->CreateImageScn(cr, Math::Coord2D<OSInt>(0, 0), Math::Coord2D<OSInt>(width - clsData->scrollSize, height), me->colorSess);
		dimg->SetHDPI(me->GetHDPI() / me->GetDDPI() * 96.0);
		dimg->SetVDPI(me->GetHDPI() / me->GetDDPI() * 96.0);
		me->OnDraw(dimg);
		me->deng->DeleteImage(dimg);
		
		dimg = ((Media::GTKDrawEngine*)me->deng.Ptr())->CreateImageScn(cr, Math::Coord2D<OSInt>(width - clsData->scrollSize, 0), Math::Coord2D<OSInt>(clsData->scrollSize, height), me->colorSess);
		dimg->SetHDPI(me->GetHDPI() / me->GetDDPI() * 96.0);
		dimg->SetVDPI(me->GetHDPI() / me->GetDDPI() * 96.0);
		NN<Media::DrawBrush> b = dimg->NewBrushARGB(0xff000000);
		dimg->DrawRect(Math::Coord2DDbl(0, 0), Math::Size2DDbl(OSInt2Double(clsData->scrollSize), OSInt2Double(height)), 0, b);
		dimg->DelBrush(b);
		b = dimg->NewBrushARGB(0xffcccccc);
		UOSInt range = clsData->max - clsData->min;
		dimg->DrawRect(Math::Coord2DDbl(0, UOSInt2Double((UOSInt)height * (clsData->currPos - clsData->min) / range)), Math::Size2DDbl(OSInt2Double(clsData->scrollSize), UOSInt2Double((UOSInt)height * clsData->pageSize / range)), 0, b);
		dimg->DelBrush(b);
		me->deng->DeleteImage(dimg);
	}
	else
	{
		NN<Media::DrawImage> dimg = ((Media::GTKDrawEngine*)me->deng.Ptr())->CreateImageScn(cr, Math::Coord2D<OSInt>(0, 0), Math::Coord2D<OSInt>(width, height), me->colorSess);
		dimg->SetHDPI(me->GetHDPI() / me->GetDDPI() * 96.0);
		dimg->SetVDPI(me->GetHDPI() / me->GetDDPI() * 96.0);
		me->OnDraw(dimg);
		me->deng->DeleteImage(dimg);
	}
	return true;
}

gboolean GUICustomDrawVScroll_OnMouseDown(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	UI::GUICustomDrawVScroll *me = (UI::GUICustomDrawVScroll*)data;
	NN<UI::GUICustomDrawVScroll::ClassData> clsData = me->clsData;
	GdkEventButton *evt = (GdkEventButton*)event;
	me->Focus();
	if (evt->type == GDK_BUTTON_PRESS)
	{
		OSInt width = gtk_widget_get_allocated_width(widget);
		OSInt height = gtk_widget_get_allocated_height(widget);
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
		if ((clsData->max - clsData->min) > clsData->pageSize && evt->x >= OSInt2Double(width - SCROLLWIDTH))
		{
			if (btn == UI::GUIControl::MBTN_LEFT)
			{
				UOSInt range = clsData->max - clsData->min;
				UOSInt scrollY1 = (UOSInt)height * (clsData->currPos - clsData->min) / range;
				UOSInt scrollY2 = (UOSInt)height * clsData->pageSize / range + scrollY1;
				if (evt->y >= UOSInt2Double(scrollY1) && evt->y < UOSInt2Double(scrollY2))
				{
					clsData->scrollDownY = Double2Int32(evt->y);
					clsData->scrollDownPos = clsData->currPos;
					clsData->scrollDown = true;
				}
				else
				{
					clsData->currPos = (UInt32)Double2Int32(evt->y * UOSInt2Double(clsData->max - clsData->min - clsData->pageSize) / OSInt2Double(height));
					me->Redraw();
				}
			}
		}
		else
		{
			UI::GUICustomDrawVScroll::KeyButton keys = UI::GUICustomDrawVScroll::KBTN_NONE;
			if (evt->state & GDK_SHIFT_MASK)
			{
				keys = (UI::GUICustomDrawVScroll::KeyButton)(keys | UI::GUICustomDrawVScroll::KBTN_SHIFT);
			}
			if (evt->state & GDK_CONTROL_MASK)
			{
				keys = (UI::GUICustomDrawVScroll::KeyButton)(keys | UI::GUICustomDrawVScroll::KBTN_CONTROL);
			}
			me->OnMouseDown((OSInt)clsData->currPos, Math::Coord2D<OSInt>(Double2OSInt(evt->x), Double2OSInt(evt->y)), btn, keys);
		}
	}
	return false;
}

gboolean GUICustomDrawVScroll_OnMouseUp(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	UI::GUICustomDrawVScroll *me = (UI::GUICustomDrawVScroll*)data;
	NN<UI::GUICustomDrawVScroll::ClassData> clsData = me->clsData;
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
		if (clsData->scrollDown && btn == UI::GUIControl::MBTN_LEFT)
		{
			clsData->scrollDown = false;
		}
//		me->OnMouseDown(Double2Int32(evt->x), Double2Int32(evt->y), btn);
	}
	return false;
}

gboolean GUICustomDrawVScroll_OnMouseMove(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	UI::GUICustomDrawVScroll *me = (UI::GUICustomDrawVScroll*)data;
	NN<UI::GUICustomDrawVScroll::ClassData> clsData = me->clsData;
	OSInt height = gtk_widget_get_allocated_height(widget);
	GdkEventMotion *evt = (GdkEventMotion*)event;
	if (clsData->scrollDown)
	{
		UOSInt range = clsData->max - clsData->min;
		UOSInt scrollPos = clsData->scrollDownPos + (UOSInt)((Double2Int32(evt->y) - clsData->scrollDownY) * (OSInt)range / height);
		if ((OSInt)scrollPos < (OSInt)clsData->min)
		{
			clsData->currPos = clsData->min;
		}
		else if (scrollPos > clsData->max - clsData->pageSize)
		{
			clsData->currPos = clsData->max - clsData->pageSize;
		}
		else
		{
			clsData->currPos = scrollPos;
		}
		me->Redraw();
	}
	else
	{
//	me->OnMouseMove(Double2Int32(evt->x), Double2Int32(evt->y));
	}
	return false;
}

gboolean GUICustomDrawVScroll_OnMouseWheel(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	UI::GUICustomDrawVScroll *me = (UI::GUICustomDrawVScroll*)data;
	NN<UI::GUICustomDrawVScroll::ClassData> clsData = me->clsData;
	UOSInt scrollSize = clsData->pageSize >> 2;
	if (scrollSize < 1)
	{
		scrollSize = 1;
	}
	GdkEventScroll *evt = (GdkEventScroll*)event;
	if (evt->direction == GDK_SCROLL_UP || (evt->direction == GDK_SCROLL_SMOOTH && evt->delta_y < 0))
	{
		if (clsData->currPos < clsData->min + scrollSize)
			clsData->currPos = clsData->min;
		else
			clsData->currPos -= scrollSize;
		me->Redraw();
		return true;
	}
	else if (evt->direction == GDK_SCROLL_DOWN || (evt->direction == GDK_SCROLL_SMOOTH && evt->delta_y > 0))
	{
		clsData->currPos += scrollSize;
		if (clsData->currPos > clsData->max - clsData->pageSize)
			clsData->currPos = clsData->max - clsData->pageSize;
		me->Redraw();
		return true;
	}
//	me->OnMouseWheel(Double2Int32(evt->x), Double2Int32(evt->y), Double2Int32(evt->delta_y));
	return false;
}


gboolean GUICustomDrawVScroll_OnKeyDown(GtkWidget* self, GdkEventKey *event, gpointer user_data)
{
	UI::GUICustomDrawVScroll *me = (UI::GUICustomDrawVScroll*)user_data;
	me->OnKeyDown(event->keyval);
	return true;
}

void UI::GUICustomDrawVScroll::ClearBackground(NN<Media::DrawImage> img)
{
	GtkStyleContext *context;
	context = gtk_widget_get_style_context((GtkWidget*)this->hwnd.OrNull());
	gtk_render_background(context, (cairo_t*)((Media::GTKDrawImage*)img.Ptr())->GetCairo(), 0, 0, UOSInt2Double(img->GetWidth()), UOSInt2Double(img->GetHeight()));
}

UI::GUICustomDrawVScroll::GUICustomDrawVScroll(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> deng, Optional<Media::ColorSess> colorSess) : UI::GUIControl(ui, parent)
{
	this->deng = deng;
	this->colorSess = colorSess;

	NN<ClassData> data = MemAllocNN(ClassData);
	this->clsData = data;
	data->min = 0;
	data->max = 100;
	data->pageSize = 10;
	data->currPos = 0;
	data->scrollDown = false;
	data->scrollDownPos = 0;
	data->scrollDownY = 0;
	data->scrollSize = 8;

	this->hwnd = (ControlHandle*)gtk_drawing_area_new();
	g_signal_connect(G_OBJECT(this->hwnd.OrNull()), "draw", G_CALLBACK(GUICustomDrawVScroll_OnDraw), this);
	g_signal_connect(G_OBJECT(this->hwnd.OrNull()), "button-press-event", G_CALLBACK(GUICustomDrawVScroll_OnMouseDown), this);
	g_signal_connect(G_OBJECT(this->hwnd.OrNull()), "button-release-event", G_CALLBACK(GUICustomDrawVScroll_OnMouseUp), this);
	g_signal_connect(G_OBJECT(this->hwnd.OrNull()), "motion-notify-event", G_CALLBACK(GUICustomDrawVScroll_OnMouseMove), this);
	g_signal_connect(G_OBJECT(this->hwnd.OrNull()), "scroll-event", G_CALLBACK(GUICustomDrawVScroll_OnMouseWheel), this);
	g_signal_connect(G_OBJECT(this->hwnd.OrNull()), "key-press-event", G_CALLBACK(GUICustomDrawVScroll_OnKeyDown), this);
	gtk_widget_set_events((GtkWidget*)this->hwnd.OrNull(), GDK_ALL_EVENTS_MASK);
	gtk_widget_set_can_focus((GtkWidget*)this->hwnd.OrNull(), true);
	parent->AddChild(*this);
	this->Show();
}

UI::GUICustomDrawVScroll::~GUICustomDrawVScroll()
{
	MemFreeNN(this->clsData);
}

Text::CStringNN UI::GUICustomDrawVScroll::GetObjectClass() const
{
	return CSTR("CustomDrawVScroll");
}

OSInt UI::GUICustomDrawVScroll::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GUICustomDrawVScroll::OnSizeChanged(Bool updateScn)
{
	UOSInt i = this->resizeHandlers.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<UI::UIEvent> cb = this->resizeHandlers.GetItem(i);
		cb.func(cb.userObj);
	}
}

void UI::GUICustomDrawVScroll::OnMouseDown(OSInt scrollY, Math::Coord2D<OSInt> pos, UI::GUIClientControl::MouseButton btn, UI::GUICustomDrawVScroll::KeyButton keys)
{
}

void UI::GUICustomDrawVScroll::OnKeyDown(UInt32 keyCode)
{
}

void UI::GUICustomDrawVScroll::HandleSelChg(UI::UIEvent hdlr, AnyType userObj)
{
	this->selChgHdlrs.Add({hdlr, userObj});
}

void UI::GUICustomDrawVScroll::HandleDblClk(UI::UIEvent hdlr, AnyType userObj)
{
	this->dblClkHdlrs.Add({hdlr, userObj});
}

void UI::GUICustomDrawVScroll::EventSelChg()
{
	UOSInt i;
	i = this->selChgHdlrs.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<UI::UIEvent> cb = this->selChgHdlrs.GetItem(i);
		cb.func(cb.userObj);
	}
}

void UI::GUICustomDrawVScroll::EventDblClk()
{
	UOSInt i;
	i = this->dblClkHdlrs.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<UI::UIEvent> cb = this->dblClkHdlrs.GetItem(i);
		cb.func(cb.userObj);
	}
}

void UI::GUICustomDrawVScroll::SetVScrollBar(UOSInt min, UOSInt max, UOSInt pageSize)
{
	NN<ClassData> data = this->clsData;
	data->min = min;
	data->max = max;
	data->pageSize = pageSize;
	if (data->currPos > data->max - data->pageSize + 1)
	{
		data->currPos = data->max - data->pageSize + 1;
	}
	if (data->currPos < data->min)
	{
		data->currPos = data->min;
	}
	this->Redraw();
}

UOSInt UI::GUICustomDrawVScroll::GetVScrollPos()
{
	NN<ClassData> data = this->clsData;
	return data->currPos;
}

Bool UI::GUICustomDrawVScroll::MakeVisible(UOSInt firstIndex, UOSInt lastIndex)
{
	NN<ClassData> data = this->clsData;
	if (lastIndex >= data->max)
	{
		lastIndex = data->max - 1;
	}
	if (firstIndex > lastIndex)
	{
		return false;
	}

	if (firstIndex < data->min)
		return false;

	if (data->currPos > firstIndex)
	{
		data->currPos = firstIndex;
		this->Redraw();
		return true;
	}
	else if ((data->currPos + data->pageSize) > lastIndex)
	{
		return false;
	}
	else
	{
		data->currPos = lastIndex - data->pageSize + 1;
		this->Redraw();
		return true;
	}
}
