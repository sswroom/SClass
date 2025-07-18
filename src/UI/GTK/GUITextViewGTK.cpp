#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Library.h"
#include "Math/Math.h"
#include "Media/GTKDrawEngine.h"
#include "Sync/Interlocked.h"
#include "Text/MyStringW.h"
#include "UI/GUIClientControl.h"
#include "UI/GUITextView.h"
#include <gtk/gtk.h>

struct UI::GUITextView::ClassData
{
	UOSInt scrSize;

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
		me->OnMouseDown(Double2OSInt(evt->x), Double2OSInt(evt->y), btn);
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
		me->OnMouseUp(Double2OSInt(evt->x), Double2OSInt(evt->y), btn);
	}
	return false;
}

gboolean GUITextView_OnMouseMove(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	UI::GUITextView *me = (UI::GUITextView*)data;
	GdkEventMotion *evt = (GdkEventMotion*)event;
	me->SetShiftState((evt->state & GDK_SHIFT_MASK) != 0);
	me->OnMouseMove(Double2OSInt(evt->x), Double2OSInt(evt->y));
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

void __stdcall UI::GUITextView::OnResize(AnyType userObj)
{
	NN<UI::GUITextView> me = userObj.GetNN<UI::GUITextView>();
	Math::Size2D<UOSInt> scnSize;
	NN<Media::DrawImage> img;
	if (me->drawBuff.SetTo(img))
	{
		me->deng->DeleteImage(img);
	}
	scnSize = me->GetSizeP();
	if ((me->clsData->scrVMax - me->clsData->scrVMin) > me->clsData->scrVPage)
	{
		scnSize.y -= me->clsData->scrSize;
	}
	if ((me->clsData->scrHMax - me->clsData->scrHMin) > me->clsData->scrHPage)
	{
		scnSize.x -= me->clsData->scrSize;
	}
	me->drawBuff = me->deng->CreateImage32(scnSize, Media::AT_ALPHA_ALL_FF);
	if (me->drawBuff.SetTo(img))
	{
		img->SetHDPI(me->GetHDPI());
		img->SetVDPI(me->GetHDPI());
		me->UpdateScrollBar();
		me->Redraw();
	}
}

void UI::GUITextView::UpdateScrollBar()
{
	if (this->drawFont == 0)
	{
		return;
	}

	Math::Size2DDbl sz;
	NN<Media::DrawImage> drawBuff;
	if (!this->drawBuff.SetTo(drawBuff))
	{
		sz.y = 12;
	}
	else
	{
		NN<Media::DrawFont> fnt;
		if (!this->CreateDrawFont(drawBuff).SetTo(fnt))
		{
			sz.y = 12;
		}
		else
		{
			sz = drawBuff->GetTextSize(fnt, CSTR("Test"));
			drawBuff->DelFont(fnt);
		}
	}
	Math::Size2D<UOSInt> usz = this->GetSizeP();
	if ((clsData->scrVMax - clsData->scrVMin) > clsData->scrVPage)
	{
		usz.y -= clsData->scrSize;
	}
	if ((clsData->scrHMax - clsData->scrHMin) > clsData->scrHPage)
	{
		usz.x -= clsData->scrSize;
	}

	this->pageLineCnt = (UInt32)(UOSInt2Double(usz.y) / sz.y);
	this->pageLineHeight = sz.y;
	this->clsData->scrVPage = this->pageLineCnt;
	this->clsData->scrHPage = usz.x;
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

UInt32 UI::GUITextView::GetCharCntAtWidth(UnsafeArray<const WChar> str, UOSInt strLen, UOSInt pxWidth)
{
	WChar wbuff[256];
	NN<Media::DrawImage> drawBuff;
	if (this->drawBuff.SetTo(drawBuff))
	{
		Double pxLeft = UOSInt2Double(pxWidth);
		UTF8Char sbuff[7];
		UTF32Char u32c;
		UnsafeArray<const UTF8Char> currPtr;
		UnsafeArray<const UTF8Char> nextPtr;

		GdkWindow* window = gtk_widget_get_window((GtkWidget*)this->hwnd.OrNull());  
		cairo_region_t *region = cairo_region_create();
		GdkDrawingContext *drawing = gdk_window_begin_draw_frame(window, region);
		cairo_t *cr = gdk_drawing_context_get_cairo_context(drawing);
		UOSInt ret;
		NN<Media::DrawFont> fnt;
		if (this->CreateDrawFont(drawBuff).SetTo(fnt))
		{
			UnsafeArray<const UTF8Char> csptr;
			if (strLen < 256)
			{
				MemCopyNO(wbuff, str.Ptr(), strLen * sizeof(WChar));
				wbuff[strLen] = 0;
				csptr = Text::StrToUTF8New(wbuff);
			}
			else
			{
				WChar *wbuffTmp = MemAlloc(WChar, strLen + 1);
				MemCopyNO(wbuffTmp, str.Ptr(), strLen * sizeof(WChar));
				wbuffTmp[strLen] = 0;
				csptr = Text::StrToUTF8New(wbuffTmp);
				MemFree(wbuffTmp);
			}
			NN<Media::GTKDrawFont>::ConvertFrom(fnt)->Init(cr);
			cairo_text_extents_t extents;

			currPtr = csptr;
			while (true)
			{
				nextPtr = Text::StrReadChar(currPtr, u32c);
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
			ret = Text::StrUTF8_WCharCntC(csptr, (UOSInt)(currPtr - csptr));
			Text::StrDelNew(csptr);
			drawBuff->DelFont(fnt);
		}
		else
		{
			ret = 0;
		}
		gdk_window_end_draw_frame(window, drawing);
		cairo_region_destroy(region);
		return (UInt32)ret;
	}
	else
	{
		return 0;
	}
}

void UI::GUITextView::GetDrawSize(UnsafeArray<const WChar> str, UOSInt strLen, OutParam<UOSInt> width, OutParam<UOSInt> height)
{
	NN<Media::DrawImage> drawBuff;
	if (this->drawBuff.SetTo(drawBuff))
	{
		WChar wbuff[256];
		Math::Size2DDbl sz;
		NN<Media::DrawFont> fnt;
		if (this->CreateDrawFont(drawBuff).SetTo(fnt))
		{
			NN<Text::String> s;
			if (strLen < 256)
			{
				MemCopyNO(wbuff, str.Ptr(), strLen * sizeof(WChar));
				wbuff[strLen] = 0;
				s = Text::String::NewNotNull(wbuff);
			}
			else
			{
				WChar *wbuffTmp = MemAlloc(WChar, strLen + 1);
				MemCopyNO(wbuffTmp, str.Ptr(), strLen * sizeof(WChar));
				wbuffTmp[strLen] = 0;
				s = Text::String::NewNotNull(wbuffTmp);
				MemFree(wbuffTmp);
			}
			sz = drawBuff->GetTextSize(fnt, s->ToCString());
			s->Release();
			width.Set((UOSInt)Double2OSInt(sz.x));
			height.Set((UOSInt)Double2OSInt(sz.y));
			drawBuff->DelFont(fnt);
		}
		else
		{
			width.Set(0);
			height.Set(0);
		}
	}
	else
	{
		width.Set(0);
		height.Set(0);
	}
/*	else
	{
		GdkWindow* window = gtk_widget_get_window((GtkWidget*)this->hwnd);  
		cairo_region_t *region = cairo_region_create();
		GdkDrawingContext *drawing = gdk_window_begin_draw_frame(window, region);
		cairo_t *cr = gdk_drawing_context_get_cairo_context(drawing);
		WChar c;
		Media::DrawFont *fnt = this->CreateDrawFont(this->drawBuff);
		c = str[strLen];
		str[strLen] = 0;
		UnsafeArray<const UTF8Char> csptr = Text::StrToUTF8New(str);
		str[strLen] = c;
		((Media::GTKDrawFont*)fnt)->Init(cr);
		cairo_text_extents_t extents;
		cairo_text_extents(cr, (const Char*)csptr, &extents);
		Text::StrDelNew(csptr);
		*width = (UOSInt)Double2OSInt(extents.width) + 2;
		*height = (UOSInt)Double2OSInt(((Media::GTKDrawFont*)fnt)->GetHeight()) + 2;
		this->drawBuff->DelFont(fnt);
		gdk_window_end_draw_frame(window, drawing);
		cairo_region_destroy(region);
	}*/
}

void UI::GUITextView::SetCaretPos(OSInt scnX, OSInt scnY)
{

}

UI::GUITextView::GUITextView(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> deng, Optional<Media::ColorSess> colorSess) : UI::GUIControl(ui, parent)
{
	this->deng = deng;
	this->colorSess = colorSess;
	this->drawBuff = 0;
	this->pageLineCnt = 0;
	this->pageLineHeight = 12;


	this->clsData = MemAlloc(ClassData, 1);
	this->clsData->scrSize = 8;
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
	this->bgColor = this->GetColorBg();
	this->scrColor = 0xffcccccc;
	this->txtColor = this->GetColorText();
	this->lineNumColor = this->GetColorTextAlt();
	this->selColor = this->GetColorHightlight();
	this->selTextColor = this->GetColorHightlightText();

	g_signal_connect(G_OBJECT(this->hwnd.OrNull()), "draw", G_CALLBACK(GUITextView_OnDraw), this);
	g_signal_connect(G_OBJECT(this->hwnd.OrNull()), "button-press-event", G_CALLBACK(GUITextView_OnMouseDown), this);
	g_signal_connect(G_OBJECT(this->hwnd.OrNull()), "button-release-event", G_CALLBACK(GUITextView_OnMouseUp), this);
	g_signal_connect(G_OBJECT(this->hwnd.OrNull()), "motion-notify-event", G_CALLBACK(GUITextView_OnMouseMove), this);
	g_signal_connect(G_OBJECT(this->hwnd.OrNull()), "scroll-event", G_CALLBACK(GUITextView_OnMouseWheel), this);
	g_signal_connect(G_OBJECT(this->hwnd.OrNull()), "key-press-event", G_CALLBACK(GUITextView_OnKeyDown), this);

	gtk_widget_set_events((GtkWidget*)this->hwnd.OrNull(), GDK_ALL_EVENTS_MASK);
	gtk_widget_set_can_focus((GtkWidget*)this->hwnd.OrNull(), true);
//	gtk_grab_add((GtkWidget*)this->hwnd);
	parent->AddChild(*this);
	this->HandleSizeChanged(OnResize, this);
	this->Show();
	this->clsData->timerId = g_timeout_add(1000, GUITextView_OnTick, this);
}

UI::GUITextView::~GUITextView()
{
	g_source_remove(this->clsData->timerId);
	NN<Media::DrawImage> img;
	if (this->drawBuff.SetTo(img))
	{
		this->deng->DeleteImage(img);
	}
	MemFree(this->clsData);
}

Text::CStringNN UI::GUITextView::GetObjectClass() const
{
	return CSTR("TextView");
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
	Math::Size2D<UOSInt> sz = this->GetSizeP();
	if ((clsData->scrVMax - clsData->scrVMin) > clsData->scrVPage && scnX >= (OSInt)(sz.x - clsData->scrSize))
	{
		if (btn == UI::GUIControl::MBTN_LEFT)
		{
			UOSInt btnSize = sz.y * clsData->scrVPage / (clsData->scrVMax - clsData->scrVMin);
			if (btnSize  < clsData->scrSize)
			{
				btnSize = clsData->scrSize;
			}
			UOSInt range = clsData->scrVMax - clsData->scrVMin - clsData->scrVPage;
			UOSInt scrollY1 = (sz.y - btnSize) * (clsData->scrVPos - clsData->scrVMin) / range;
			UOSInt scrollY2 = scrollY1 + btnSize;
			if (scnY >= (OSInt)scrollY1 && scnY < (OSInt)scrollY2)
			{
				clsData->scrVDownY = scnY;
				clsData->scrVDownPos = clsData->scrVPos;
				clsData->scrVDown = true;
			}
			else if (scnY < (OSInt)(btnSize >> 1))
			{
				clsData->scrVPos = clsData->scrVMin;;
				this->Redraw();
			}
			else if (scnY >= (OSInt)(sz.y - (btnSize >> 1)))
			{
				clsData->scrVPos = clsData->scrVMax - clsData->scrVPage + 1;
				this->Redraw();
			}
			else  
			{
				clsData->scrVPos = (UOSInt)((scnY - (OSInt)(btnSize >> 1)) * (OSInt)(clsData->scrVMax - clsData->scrVMin - clsData->scrVPage + 1) / (OSInt)(sz.y - btnSize));
				this->Redraw();
			}
		}
	}
	else if ((clsData->scrHMax - clsData->scrHMin) > clsData->scrHPage && scnY >= (OSInt)(sz.y - clsData->scrSize))
	{
		if (btn == UI::GUIControl::MBTN_LEFT)
		{
			UOSInt btnSize = sz.x * clsData->scrHPage / (clsData->scrHMax - clsData->scrHMin);
			if (btnSize  < clsData->scrSize)
			{
				btnSize = clsData->scrSize;
			}
			UOSInt range = clsData->scrHMax - clsData->scrHMin - clsData->scrHPage;
			UOSInt scrollX1 = (sz.x - btnSize) * (clsData->scrHPos - clsData->scrHMin) / range;
			UOSInt scrollX2 = scrollX1 + btnSize;
			if (scnX >= (OSInt)scrollX1 && scnX < (OSInt)scrollX2)
			{
				clsData->scrHDownX = scnX;
				clsData->scrHDownPos = clsData->scrHPos;
				clsData->scrHDown = true;
			}
			else if (scnX < (OSInt)(btnSize >> 1))
			{
				clsData->scrHPos = clsData->scrHMin;;
				this->Redraw();
			}
			else if (scnX >= (OSInt)(sz.x - (btnSize >> 1)))
			{
				clsData->scrHPos = clsData->scrHMax - clsData->scrHPage + 1;
				this->Redraw();
			}
			else
			{
				clsData->scrHPos = (UOSInt)((scnX - (OSInt)(btnSize >> 1)) * (OSInt)(clsData->scrHMax - clsData->scrHMin - clsData->scrHPage + 1) / (OSInt)(sz.x - btnSize));
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
	Math::Size2D<UOSInt> sz = this->GetSizeP();
	if (this->clsData->scrVDown)
	{
		UOSInt btnSize = sz.y * clsData->scrVPage / (clsData->scrVMax - clsData->scrVMin);
		if (btnSize  < clsData->scrSize)
		{
			btnSize = clsData->scrSize;
		}
		UOSInt range = this->clsData->scrVMax - this->clsData->scrVMin - clsData->scrVPage;
		OSInt scrollPos = (OSInt)clsData->scrVDownPos + (scnY - clsData->scrVDownY) * (OSInt)range / (OSInt)(sz.y - btnSize);
		if (scrollPos < (OSInt)clsData->scrVMin)
		{
			clsData->scrVPos = clsData->scrVMin;
		}
		else if ((UOSInt)scrollPos > clsData->scrVMax - clsData->scrVPage + 1)
		{
			clsData->scrVPos = clsData->scrVMax - clsData->scrVPage + 1;
		}
		else
		{
			clsData->scrVPos = (UOSInt)scrollPos;
		}
		this->Redraw();
	}
	else if (this->clsData->scrHDown)
	{
		UOSInt btnSize = sz.x * clsData->scrHPage / (clsData->scrHMax - clsData->scrHMin);
		if (btnSize  < clsData->scrSize)
		{
			btnSize = clsData->scrSize;
		}
		UOSInt range = clsData->scrHMax - clsData->scrHMin - clsData->scrHPage;
		OSInt scrollPos = (OSInt)clsData->scrHDownPos + (scnX - clsData->scrHDownX) * (OSInt)range / (OSInt)(sz.x - btnSize);
		if (scrollPos < (OSInt)clsData->scrHMin)
		{
			clsData->scrHPos = clsData->scrHMin;
		}
		else if ((UOSInt)scrollPos > clsData->scrHMax - clsData->scrHPage + 1)
		{
			clsData->scrHPos = clsData->scrHMax - clsData->scrHPage + 1;
		}
		else
		{
			clsData->scrHPos = (UOSInt)scrollPos;
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
			if (clsData->scrHPos > clsData->scrHMax - clsData->scrHPage + 1)
				clsData->scrHPos = clsData->scrHMax - clsData->scrHPage + 1;
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
			if (clsData->scrVPos > clsData->scrVMax - clsData->scrVPage + 1)
				clsData->scrVPos = clsData->scrVMax - clsData->scrVPage + 1;
			this->Redraw();
		}
	}
}

void UI::GUITextView::OnDraw(void *cr)
{
	Math::Size2D<UOSInt> sz = this->GetSizeP();
	UOSInt drawWidth = sz.x;
	UOSInt drawHeight = sz.y;
	Bool hasHScr = false;
	Bool hasVScr = false;

	if ((clsData->scrVMax - clsData->scrVMin) > clsData->scrVPage)
	{
		hasVScr = true;
		drawWidth -= clsData->scrSize;
	}
	if ((clsData->scrHMax - clsData->scrHMin) > clsData->scrHPage)
	{
		hasHScr = true;
		drawHeight -= clsData->scrSize;
	}
	NN<Media::DrawImage> dimg = ((Media::GTKDrawEngine*)this->deng.Ptr())->CreateImageScn(cr, Math::Coord2D<OSInt>(0, 0), Math::Coord2D<OSInt>((OSInt)drawWidth, (OSInt)drawHeight), this->colorSess);
	this->clsData->scrSize = (UOSInt)Double2OSInt(8.0 * this->GetHDPI() / this->GetDDPI());
	dimg->SetHDPI(this->GetHDPI() / this->GetDDPI() * 96.0);
	dimg->SetVDPI(this->GetHDPI() / this->GetDDPI() * 96.0);
	this->DrawImage(dimg);
	this->deng->DeleteImage(dimg);

	if (hasVScr)
	{
		dimg = ((Media::GTKDrawEngine*)this->deng.Ptr())->CreateImageScn(cr, Math::Coord2D<OSInt>((OSInt)(sz.x - clsData->scrSize), 0), Math::Coord2D<OSInt>((OSInt)clsData->scrSize, (OSInt)drawHeight), this->colorSess);
		dimg->SetHDPI(this->GetHDPI() / this->GetDDPI() * 96.0);
		dimg->SetVDPI(this->GetHDPI() / this->GetDDPI() * 96.0);
		NN<Media::DrawBrush> b = dimg->NewBrushARGB(this->bgColor);
		dimg->DrawRect(Math::Coord2DDbl(0, 0), Math::Size2DDbl(UOSInt2Double(clsData->scrSize), UOSInt2Double(drawHeight)), 0, b);
		dimg->DelBrush(b);
		b = dimg->NewBrushARGB(this->scrColor);
		UOSInt btnSize = drawHeight * clsData->scrVPage / (clsData->scrVMax - clsData->scrVMin);
		if (btnSize  < clsData->scrSize)
		{
			btnSize = clsData->scrSize;
		}
		UOSInt range = clsData->scrVMax - clsData->scrVMin - clsData->scrVPage;
		dimg->DrawRect(Math::Coord2DDbl(0, UOSInt2Double((drawHeight - btnSize) * (clsData->scrVPos - clsData->scrVMin) / range)), Math::Size2DDbl(UOSInt2Double(clsData->scrSize), UOSInt2Double(btnSize)), 0, b);
		dimg->DelBrush(b);
		this->deng->DeleteImage(dimg);
	}
	if (hasHScr)
	{
		dimg = ((Media::GTKDrawEngine*)this->deng.Ptr())->CreateImageScn(cr, Math::Coord2D<OSInt>(0, (OSInt)(sz.y - clsData->scrSize)), Math::Coord2D<OSInt>((OSInt)drawWidth, (OSInt)clsData->scrSize), this->colorSess);
		dimg->SetHDPI(this->GetHDPI() / this->GetDDPI() * 96.0);
		dimg->SetVDPI(this->GetHDPI() / this->GetDDPI() * 96.0);
		NN<Media::DrawBrush> b = dimg->NewBrushARGB(this->bgColor);
		dimg->DrawRect(Math::Coord2DDbl(0, 0), Math::Size2DDbl(UOSInt2Double(drawWidth), UOSInt2Double(clsData->scrSize)), 0, b);
		dimg->DelBrush(b);
		b = dimg->NewBrushARGB(this->scrColor);
		UOSInt btnSize = drawWidth * clsData->scrHPage / (clsData->scrHMax - clsData->scrHMin);
		if (btnSize  < clsData->scrSize)
		{
			btnSize = clsData->scrSize;
		}
		UOSInt range = clsData->scrHMax - clsData->scrHMin - clsData->scrHPage;
		if (range <= 0)
		{
			range = 1;
		}
		dimg->DrawRect(Math::Coord2DDbl(UOSInt2Double((drawWidth - btnSize) * (clsData->scrHPos - clsData->scrHMin) / range), 0), Math::Size2DDbl(UOSInt2Double(btnSize), UOSInt2Double(clsData->scrSize)), 0, b);
		dimg->DelBrush(b);
		this->deng->DeleteImage(dimg);
	}
}

void UI::GUITextView::SetShiftState(Bool isDown)
{
	this->clsData->shiftDown = isDown;
}
