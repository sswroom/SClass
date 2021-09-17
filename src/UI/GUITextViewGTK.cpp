#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Library.h"
#include "Math/Ellipse.h"
#include "Math/Math.h"
#include "Math/PieArea.h"
#include "Math/Polyline.h"
#include "Media/GTKDrawEngine.h"
#include "Sync/Interlocked.h"
#include "UI/GUIClientControl.h"
#include "UI/GUITextView.h"
#include "UI/MessageDialog.h"
#include <gtk/gtk.h>
#define SCROLLWIDTH 8

struct UI::GUITextView::ClassData
{
	UOSInt scrHPos;
	UOSInt scrHMin;
	UOSInt scrHMax;
	UOSInt scrHPage;
	Bool scrHDown;
	OSInt scrHDownX;
	UOSInt scrHDownPos;

	UOSInt scrVPos;
	UOSInt scrVMin;
	UOSInt scrVMax;
	UOSInt scrVPage;
	Bool scrVDown;
	OSInt scrVDownY;
	UOSInt scrVDownPos;

	guint timerId;
	Bool shiftDown;
};

OSInt UI::GUITextView::useCnt = 0;

gboolean GUITextView_OnDraw(GtkWidget *widget, cairo_t *cr, gpointer data)
{
	UI::GUITextView *me = (UI::GUITextView*)data;
	me->OnDraw(cr);
	return true;
}

gboolean GUITextView_OnMouseDown(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	UI::GUITextView *me = (UI::GUITextView*)data;
	GdkEventButton *evt = (GdkEventButton*)event;
	me->SetShiftState((evt->state & GDK_SHIFT_MASK) != 0);
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
		me->OnMouseDown(Math::Double2OSInt(evt->x), Math::Double2OSInt(evt->y), btn);
		me->Focus();
	}
	return false;
}

gboolean GUITextView_OnMouseUp(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	UI::GUITextView *me = (UI::GUITextView*)data;
	GdkEventButton *evt = (GdkEventButton*)event;
	me->SetShiftState((evt->state & GDK_SHIFT_MASK) != 0);
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
		me->OnMouseUp(Math::Double2OSInt(evt->x), Math::Double2OSInt(evt->y), btn);
	}
	return false;
}

gboolean GUITextView_OnMouseMove(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	UI::GUITextView *me = (UI::GUITextView*)data;
	GdkEventMotion *evt = (GdkEventMotion*)event;
	me->SetShiftState((evt->state & GDK_SHIFT_MASK) != 0);
	me->OnMouseMove(Math::Double2OSInt(evt->x), Math::Double2OSInt(evt->y));
	return false;
}

gboolean GUITextView_OnMouseWheel(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	UI::GUITextView *me = (UI::GUITextView*)data;
	GdkEventScroll *evt = (GdkEventScroll*)event;
	me->SetShiftState((evt->state & GDK_SHIFT_MASK) != 0);
	if (evt->direction == GDK_SCROLL_UP || (evt->direction == GDK_SCROLL_SMOOTH && evt->delta_y < 0))
	{
		me->OnMouseWheel(false);
		return true;
	}
	else if (evt->direction == GDK_SCROLL_DOWN || (evt->direction == GDK_SCROLL_SMOOTH && evt->delta_y > 0))
	{
		me->OnMouseWheel(true);
		return true;
	}
	return false;
}

Int32 GUITextView_OnTick(void *userObj)
{
	UI::GUITextView *me = (UI::GUITextView*)userObj;
	me->EventTimerTick();
	return 1;
}

gboolean GUITextView_OnKeyDown(GtkWidget* self, GdkEventKey *event, gpointer user_data)
{
	UI::GUITextView *me = (UI::GUITextView*)user_data;
	me->SetShiftState((event->state & GDK_SHIFT_MASK) != 0);
	switch (event->keyval)
	{
	case GDK_KEY_Home:
		if (event->state & GDK_CONTROL_MASK)
		{
			me->EventHome();
		}
		else
		{
			me->EventLineBegin();
		}
		break;
	case GDK_KEY_End:
		if (event->state & GDK_CONTROL_MASK)
		{
			me->EventEnd();
		}
		else
		{
			me->EventLineEnd();
		}
		break;
	case GDK_KEY_Left:
		me->EventLeft();
		break;
	case GDK_KEY_Right:
		me->EventRight();
		break;
	case GDK_KEY_Up:
		me->EventLineUp();
		break;
	case GDK_KEY_Down:
		me->EventLineDown();
		break;
	case GDK_KEY_Page_Up:
		me->EventPageUp();
		break;
	case GDK_KEY_Page_Down:
		me->EventPageDown();
		break;
	case GDK_KEY_c:
	case GDK_KEY_C:
		if (event->state & GDK_CONTROL_MASK)
		{
			me->EventCopy();
		}
		break;
	default:
		return false;
	}
	return true;
}

void __stdcall UI::GUITextView::OnResize(void *userObj)
{
	UI::GUITextView *me = (UI::GUITextView*)userObj;
	UOSInt scnW;
	UOSInt scnH;
	if (me->drawBuff)
	{
		me->deng->DeleteImage(me->drawBuff);
	}
	me->GetSizeP(&scnW, &scnH);
	if ((me->clsData->scrVMax - me->clsData->scrVMin) > me->clsData->scrVPage)
	{
		scnH -= SCROLLWIDTH;
	}
	if ((me->clsData->scrHMax - me->clsData->scrHMin) > me->clsData->scrHPage)
	{
		scnW -= SCROLLWIDTH;
	}
	me->drawBuff = me->deng->CreateImage32(scnW, scnH, Media::AT_NO_ALPHA);
	me->drawBuff->SetHDPI(me->GetHDPI());
	me->drawBuff->SetVDPI(me->GetHDPI());
	me->UpdateScrollBar();
	me->Redraw();
}

void UI::GUITextView::UpdateScrollBar()
{
	if (this->drawFont == 0)
	{
		return;
	}

	Double sz[2];
	if (this->drawBuff == 0)
	{
		sz[1] = 12;
	}
	else
	{
		Media::DrawFont *fnt = this->CreateDrawFont(this->drawBuff);
		if (fnt == 0)
		{
			sz[1] = 12;
		}
		else
		{
			this->drawBuff->GetTextSize(fnt, (const UTF8Char*)"Test", sz);
			this->drawBuff->DelFont(fnt);
		}
	}
	UOSInt width;
	UOSInt height;
	this->GetSizeP(&width, &height);
	if ((clsData->scrVMax - clsData->scrVMin) > clsData->scrVPage)
	{
		height -= SCROLLWIDTH;
	}
	if ((clsData->scrHMax - clsData->scrHMin) > clsData->scrHPage)
	{
		width -= SCROLLWIDTH;
	}

	this->pageLineCnt = (UInt32)(Math::UOSInt2Double(height) / sz[1]);
	this->pageLineHeight = sz[1];
	this->clsData->scrVPage = this->pageLineCnt;
	this->clsData->scrHPage = width;
	this->Redraw();
}

Bool UI::GUITextView::IsShiftPressed()
{
	return this->clsData->shiftDown;
}

void UI::GUITextView::SetScrollHPos(UOSInt pos, Bool redraw)
{
	if (this->clsData->scrHPos == pos)
	{
		return;
	}
	this->clsData->scrHPos = pos;
	if (this->clsData->scrHPos > this->clsData->scrHMax - this->clsData->scrHPage + 1)
	{
		this->clsData->scrHPos = this->clsData->scrHMax - this->clsData->scrHPage + 1;
	}
	if (this->clsData->scrHPos < this->clsData->scrHMin)
	{
		this->clsData->scrHPos = this->clsData->scrHMin;
	}
	if (redraw)
	{
		this->Redraw();
	}
}

void UI::GUITextView::SetScrollVPos(UOSInt pos, Bool redraw)
{
	if (this->clsData->scrVPos == pos)
	{
		return;
	}
	this->clsData->scrVPos = pos;
	if (this->clsData->scrVPos > this->clsData->scrVMax - this->clsData->scrVPage + 1)
	{
		this->clsData->scrVPos = this->clsData->scrVMax - this->clsData->scrVPage + 1;
	}
	if (this->clsData->scrVPos < this->clsData->scrVMin)
	{
		this->clsData->scrVPos = this->clsData->scrVMin;
	}
	if (redraw)
	{
		this->Redraw();
	}
}

void UI::GUITextView::SetScrollHRange(UOSInt min, UOSInt max)
{
	if (this->clsData->scrHMin == min && this->clsData->scrHMax == max)
	{
		return;
	}
	this->clsData->scrHMin = min;
	this->clsData->scrHMax = max;
	if (this->clsData->scrHPos > this->clsData->scrHMax - this->clsData->scrHPage + 1)
	{
		this->clsData->scrHPos = this->clsData->scrHMax - this->clsData->scrHPage + 1;
	}
	if (this->clsData->scrHPos < this->clsData->scrHMin)
	{
		this->clsData->scrHPos = this->clsData->scrHMin;
	}
	this->Redraw();
}

void UI::GUITextView::SetScrollVRange(UOSInt min, UOSInt max)
{
	if (this->clsData->scrVMin == min && this->clsData->scrVMax == max)
	{
		return;
	}
	this->clsData->scrVMin = min;
	this->clsData->scrVMax = max;
	if (this->clsData->scrVPos > this->clsData->scrVMax - this->clsData->scrVPage + 1)
	{
		this->clsData->scrVPos = this->clsData->scrVMax - this->clsData->scrVPage + 1;
	}
	if (this->clsData->scrVPos < this->clsData->scrVMin)
	{
		this->clsData->scrVPos = this->clsData->scrVMin;
	}
	this->Redraw();
}

UInt32 UI::GUITextView::GetCharCntAtWidth(WChar *str, UOSInt strLen, UOSInt pxWidth)
{
	Double pxLeft = Math::UOSInt2Double(pxWidth);
	UTF8Char sbuff[7];
	UTF32Char u32c;
	const UTF8Char *currPtr;
	const UTF8Char *nextPtr;

	GdkWindow* window = gtk_widget_get_window((GtkWidget*)this->hwnd);  
	cairo_region_t *region = cairo_region_create();
	GdkDrawingContext *drawing = gdk_window_begin_draw_frame(window, region);
	cairo_t *cr = gdk_drawing_context_get_cairo_context(drawing);
	WChar c;
	Media::DrawFont *fnt = this->CreateDrawFont(this->drawBuff);
	c = str[strLen];
	str[strLen] = 0;
	const UTF8Char *csptr = Text::StrToUTF8New(str);
	str[strLen] = c;
	((Media::GTKDrawFont*)fnt)->Init(cr);
	cairo_text_extents_t extents;

	currPtr = csptr;
	while (true)
	{
		nextPtr = Text::StrReadChar(currPtr, &u32c);
		if (u32c == 0)
		{
			break;
		}
		Text::StrWriteChar(sbuff, u32c)[0] = 0;
		
		cairo_text_extents(cr, (const Char*)sbuff, &extents);
		if (extents.width > pxLeft)
		{
			break;
		}
		pxLeft -= extents.width;
		currPtr = nextPtr;
		if (pxLeft <= 0)
		{
			break;
		}
	}
	UOSInt ret = Text::StrUTF8_WCharCntC(csptr, (UOSInt)(currPtr - csptr));
	Text::StrDelNew(csptr);
	this->drawBuff->DelFont(fnt);
	gdk_window_end_draw_frame(window, drawing);
	cairo_region_destroy(region);
	return (UInt32)ret;
}

void UI::GUITextView::GetDrawSize(WChar *str, UOSInt strLen, UOSInt *width, UOSInt *height)
{
	if (this->drawBuff)
	{
		WChar c;
		Double sz[2];
		Media::DrawFont *fnt = this->CreateDrawFont(this->drawBuff);
		c = str[strLen];
		str[strLen] = 0;
		const UTF8Char *csptr = Text::StrToUTF8New(str);
		str[strLen] = c;
		this->drawBuff->GetTextSize(fnt, csptr, sz);
		Text::StrDelNew(csptr);
		*width = (UOSInt)Math::Double2OSInt(sz[0]);
		*height = (UOSInt)Math::Double2OSInt(sz[1]);
		this->drawBuff->DelFont(fnt);
	}
	else
	{
		GdkWindow* window = gtk_widget_get_window((GtkWidget*)this->hwnd);  
		cairo_region_t *region = cairo_region_create();
		GdkDrawingContext *drawing = gdk_window_begin_draw_frame(window, region);
		cairo_t *cr = gdk_drawing_context_get_cairo_context(drawing);
		WChar c;
		Media::DrawFont *fnt = this->CreateDrawFont(this->drawBuff);
		c = str[strLen];
		str[strLen] = 0;
		const UTF8Char *csptr = Text::StrToUTF8New(str);
		str[strLen] = c;
		((Media::GTKDrawFont*)fnt)->Init(cr);
		cairo_text_extents_t extents;
		cairo_text_extents(cr, (const Char*)csptr, &extents);
		Text::StrDelNew(csptr);
		*width = (UOSInt)Math::Double2OSInt(extents.width) + 2;
		*height = (UOSInt)Math::Double2OSInt(((Media::GTKDrawFont*)fnt)->GetHeight()) + 2;
		this->drawBuff->DelFont(fnt);
		gdk_window_end_draw_frame(window, drawing);
		cairo_region_destroy(region);
	}
}

void UI::GUITextView::SetCaretPos(OSInt scnX, OSInt scnY)
{

}

UI::GUITextView::GUITextView(UI::GUICore *ui, UI::GUIClientControl *parent, Media::DrawEngine *deng) : UI::GUIControl(ui, parent)
{
	this->deng = deng;
	this->drawBuff = 0;
	this->pageLineCnt = 0;
	this->pageLineHeight = 12;
	this->bgColor = 0xffffffff;
	this->scrColor = 0xffcccccc;
	this->txtColor = 0xff000000;
	this->lineNumColor = 0xff0000ff;
	this->selColor = 0xffccccff;


	this->clsData = MemAlloc(ClassData, 1);
	this->clsData->scrVMin = 0;
	this->clsData->scrVMax = 100;
	this->clsData->scrVPage = 10;
	this->clsData->scrVPos = 0;
	this->clsData->scrVDown = false;
	this->clsData->scrVDownPos = 0;
	this->clsData->scrVDownY = 0;
	this->clsData->scrHMin = 0;
	this->clsData->scrHMax = 100;
	this->clsData->scrHPage = 10;
	this->clsData->scrHPos = 0;
	this->clsData->scrHDown = false;
	this->clsData->scrHDownPos = 0;
	this->clsData->scrHDownX = 0;
	this->clsData->shiftDown = false;

	this->hwnd = (ControlHandle*)gtk_drawing_area_new();
	g_signal_connect(G_OBJECT(this->hwnd), "draw", G_CALLBACK(GUITextView_OnDraw), this);
	g_signal_connect(G_OBJECT(this->hwnd), "button-press-event", G_CALLBACK(GUITextView_OnMouseDown), this);
	g_signal_connect(G_OBJECT(this->hwnd), "button-release-event", G_CALLBACK(GUITextView_OnMouseUp), this);
	g_signal_connect(G_OBJECT(this->hwnd), "motion-notify-event", G_CALLBACK(GUITextView_OnMouseMove), this);
	g_signal_connect(G_OBJECT(this->hwnd), "scroll-event", G_CALLBACK(GUITextView_OnMouseWheel), this);
	g_signal_connect(G_OBJECT(this->hwnd), "key-press-event", G_CALLBACK(GUITextView_OnKeyDown), this);

	gtk_widget_set_events((GtkWidget*)this->hwnd, GDK_ALL_EVENTS_MASK);
	gtk_widget_set_can_focus((GtkWidget*)this->hwnd, true);
//	gtk_grab_add((GtkWidget*)this->hwnd);
	parent->AddChild(this);
	this->HandleSizeChanged(OnResize, this);
	this->Show();
	this->clsData->timerId = g_timeout_add(1000, GUITextView_OnTick, this);
}

UI::GUITextView::~GUITextView()
{
	g_source_remove(this->clsData->timerId);
	if (this->drawBuff)
	{
		this->deng->DeleteImage(this->drawBuff);
	}
	MemFree(this->clsData);
}

const UTF8Char *UI::GUITextView::GetObjectClass()
{
	return (const UTF8Char*)"TextView";
}

OSInt UI::GUITextView::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GUITextView::UpdateFont()
{
	this->drawFont = this->GetFont();
	UpdateScrollBar();
}

OSInt UI::GUITextView::GetScrollHPos()
{
	return (OSInt)this->clsData->scrHPos;
}

OSInt UI::GUITextView::GetScrollVPos()
{
	return (OSInt)this->clsData->scrVPos;
}

void UI::GUITextView::OnMouseDown(OSInt scnX, OSInt scnY, MouseButton btn)
{
	UOSInt width;
	UOSInt height;
	this->GetSizeP(&width, &height);
	if ((clsData->scrVMax - clsData->scrVMin) > clsData->scrVPage && scnX >= (OSInt)width - SCROLLWIDTH)
	{
		if (btn == UI::GUIControl::MBTN_LEFT)
		{
			UOSInt range = clsData->scrVMax - clsData->scrVMin;
			UOSInt scrollY1 = height * (clsData->scrVPos - clsData->scrVMin) / range;
			UOSInt scrollY2 = height * clsData->scrVPage / range + scrollY1;
			if (scnY >= (OSInt)scrollY1 && scnY < (OSInt)scrollY2)
			{
				clsData->scrVDownY = scnY;
				clsData->scrVDownPos = clsData->scrVPos;
				clsData->scrVDown = true;
			}
			else
			{
				clsData->scrVPos = (UInt32)(scnY * (OSInt)(clsData->scrVMax - clsData->scrVMin - clsData->scrVPage) / (OSInt)height);
				this->Redraw();
			}
		}
	}
	else if ((clsData->scrHMax - clsData->scrHMin) > clsData->scrHPage && scnY >= (OSInt)height - SCROLLWIDTH)
	{
		if (btn == UI::GUIControl::MBTN_LEFT)
		{
			UOSInt range = clsData->scrHMax - clsData->scrHMin;
			UOSInt scrollX1 = width * (clsData->scrHPos - clsData->scrHMin) / range;
			UOSInt scrollX2 = width * clsData->scrHPage / range + scrollX1;
			if (scnX >= (OSInt)scrollX1 && scnX < (OSInt)scrollX2)
			{
				clsData->scrHDownX = scnX;
				clsData->scrHDownPos = clsData->scrHPos;
				clsData->scrHDown = true;
			}
			else
			{
				clsData->scrHPos = (UInt32)(scnX * (OSInt)(clsData->scrHMax - clsData->scrHMin - clsData->scrHPage) / (OSInt)width);
				this->Redraw();
			}
		}
	}
	else
	{
		this->EventMouseDown(scnX, scnY, btn);
	}

}

void UI::GUITextView::OnMouseUp(OSInt scnX, OSInt scnY, MouseButton btn)
{
	if (this->clsData->scrVDown && btn == UI::GUIControl::MBTN_LEFT)
	{
		this->clsData->scrVDown = false;
	}
	else if (this->clsData->scrHDown && btn == UI::GUIControl::MBTN_LEFT)
	{
		this->clsData->scrHDown = false;
	}
	else
	{
		this->EventMouseUp(scnX, scnY, btn);
	}
}

void UI::GUITextView::OnMouseMove(OSInt scnX, OSInt scnY)
{
	UOSInt width;
	UOSInt height;
	this->GetSizeP(&width, &height);
	if (this->clsData->scrVDown)
	{
		UOSInt range = this->clsData->scrVMax - this->clsData->scrVMin;
		UOSInt scrollPos = clsData->scrVDownPos + (UOSInt)(scnY - clsData->scrVDownY) * range / height;
		if (scrollPos < clsData->scrVMin)
		{
			clsData->scrVPos = clsData->scrVMin;
		}
		else if (scrollPos > clsData->scrVMax - clsData->scrVPage)
		{
			clsData->scrVPos = clsData->scrVMax - clsData->scrVPage;
		}
		else
		{
			clsData->scrVPos = scrollPos;
		}
		this->Redraw();
	}
	else if (this->clsData->scrHDown)
	{
		UOSInt range = clsData->scrHMax - clsData->scrHMin;
		UOSInt scrollPos = clsData->scrHDownPos + (UOSInt)(scnX - clsData->scrHDownX) * range / width;
		if (scrollPos < clsData->scrHMin)
		{
			clsData->scrHPos = clsData->scrHMin;
		}
		else if (scrollPos > clsData->scrHMax - clsData->scrHPage)
		{
			clsData->scrHPos = clsData->scrHMax - clsData->scrHPage;
		}
		else
		{
			clsData->scrHPos = scrollPos;
		}
		this->Redraw();
	}
	else
	{
		this->EventMouseMove(scnX, scnY);
	}
}

void UI::GUITextView::OnMouseWheel(Bool isDown)
{
	if (false)
	{
		UOSInt scrollSize = clsData->scrHPage >> 2;
		if (scrollSize < 1)
		{
			scrollSize = 1;
		}
		if (!isDown)
		{
			clsData->scrHPos -= scrollSize;
			if (clsData->scrHPos < clsData->scrHMin)
				clsData->scrHPos = clsData->scrHMin;
			this->Redraw();
		}
		else
		{
			clsData->scrHPos += scrollSize;
			if (clsData->scrHPos > clsData->scrHMax - clsData->scrHPage)
				clsData->scrHPos = clsData->scrHMax - clsData->scrHPage;
			this->Redraw();
		}
	}
	else
	{
		UOSInt scrollSize = clsData->scrVPage >> 2;
		if (scrollSize < 1)
		{
			scrollSize = 1;
		}
		if (!isDown)
		{
			if (clsData->scrVPos <= scrollSize)
			{
				clsData->scrVPos = clsData->scrVMin;
			}
			else
			{
				clsData->scrVPos -= scrollSize;
				if (clsData->scrVPos < clsData->scrVMin)
					clsData->scrVPos = clsData->scrVMin;
			}
			this->Redraw();
		}
		else
		{
			clsData->scrVPos += scrollSize;
			if (clsData->scrVPos > clsData->scrVMax - clsData->scrVPage)
				clsData->scrVPos = clsData->scrVMax - clsData->scrVPage;
			this->Redraw();
		}
	}
}

void UI::GUITextView::OnDraw(void *cr)
{
	UOSInt width;
	UOSInt height;
	this->GetSizeP(&width, &height);
	UOSInt drawWidth = width;
	UOSInt drawHeight = height;
	Bool hasHScr = false;;
	Bool hasVScr = false;

	if ((clsData->scrVMax - clsData->scrVMin) > clsData->scrVPage)
	{
		hasVScr = true;
		drawHeight -= SCROLLWIDTH;
	}
	if ((clsData->scrHMax - clsData->scrHMin) > clsData->scrHPage)
	{
		hasHScr = true;
		drawHeight -= SCROLLWIDTH;
	}
	Media::DrawImage *dimg = ((Media::GTKDrawEngine*)this->deng)->CreateImageScn(cr, 0, 0, (OSInt)drawWidth, (OSInt)drawHeight);
	dimg->SetHDPI(this->GetHDPI() / this->GetDDPI() * 96.0);
	dimg->SetVDPI(this->GetHDPI() / this->GetDDPI() * 96.0);
	this->DrawImage(dimg);
	this->deng->DeleteImage(dimg);

	if (hasVScr)
	{
		dimg = ((Media::GTKDrawEngine*)this->deng)->CreateImageScn(cr, (OSInt)width - SCROLLWIDTH, 0, SCROLLWIDTH, (OSInt)drawHeight);
		dimg->SetHDPI(this->GetHDPI() / this->GetDDPI() * 96.0);
		dimg->SetVDPI(this->GetHDPI() / this->GetDDPI() * 96.0);
		Media::DrawBrush *b = dimg->NewBrushARGB(this->bgColor);
		dimg->DrawRect(0, 0, SCROLLWIDTH, Math::UOSInt2Double(drawHeight), 0, b);
		dimg->DelBrush(b);
		b = dimg->NewBrushARGB(this->scrColor);
		UOSInt range = clsData->scrVMax - clsData->scrVMin;
		dimg->DrawRect(0, Math::UOSInt2Double(drawHeight * (clsData->scrVPos - clsData->scrVMin) / range), SCROLLWIDTH, Math::UOSInt2Double(drawHeight * clsData->scrVPage / range), 0, b);
		this->deng->DeleteImage(dimg);
	}
	if (hasHScr)
	{
		dimg = ((Media::GTKDrawEngine*)this->deng)->CreateImageScn(cr, 0, (OSInt)height - SCROLLWIDTH, (OSInt)drawWidth, SCROLLWIDTH);
		dimg->SetHDPI(this->GetHDPI() / this->GetDDPI() * 96.0);
		dimg->SetVDPI(this->GetHDPI() / this->GetDDPI() * 96.0);
		Media::DrawBrush *b = dimg->NewBrushARGB(this->bgColor);
		dimg->DrawRect(0, 0, Math::UOSInt2Double(drawWidth), SCROLLWIDTH, 0, b);
		dimg->DelBrush(b);
		b = dimg->NewBrushARGB(this->scrColor);
		UOSInt range = clsData->scrHMax - clsData->scrHMin;
		dimg->DrawRect(Math::UOSInt2Double(drawWidth * (clsData->scrHPos - clsData->scrHMin) / range), 0, Math::UOSInt2Double(drawHeight * clsData->scrHPage / range), SCROLLWIDTH, 0, b);
		this->deng->DeleteImage(dimg);
	}
}

void UI::GUITextView::SetShiftState(Bool isDown)
{
	this->clsData->shiftDown = isDown;
}
