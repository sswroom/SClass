#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Media/GTKDrawEngine.h"
#include "Text/CSSBuilder.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "UI/GUIClientControl.h"
#include "UI/GUIControl.h"
#include "UI/GTK/GTKDragDrop.h"
#include <gtk/gtk.h>
#include <stdio.h>

#define GDK_VERSION_AFTER(major, minor) (GDK_MAJOR_VERSION > major || (GDK_MAJOR_VERSION == major && GDK_MINOR_VERSION >= minor))
/*
your_widget.get_style_context().add_class('red-background')
*/

gboolean GUIControl_ToGenDrawSignal(gpointer user_data)
{
	gtk_widget_queue_draw((GtkWidget*)user_data);
	return false;
}


gboolean GUIControl_SetNoResize(gpointer user_data)
{
	gtk_window_set_resizable((GtkWindow*)user_data, false);
	return false;
}

void GUIControl_SizeChanged(GtkWidget *wnd, GdkEvent *event, gpointer data)
{
	UI::GUIControl *me = (UI::GUIControl*)data;
	me->OnSizeChanged(false);
}

UI::GUIControl::GUIControl(NN<UI::GUICore> ui, Optional<GUIClientControl> parent)
{
	this->ui = ui;
	this->parent = parent;
	this->selfResize = false;
	this->dockType = UI::GUIControl::DOCK_NONE;
	NN<GUIClientControl> nnparent;
	if (parent.SetTo(nnparent))
	{
		this->hdpi = nnparent->GetHDPI();
		this->ddpi = nnparent->GetDDPI();
	}
	else
	{
		this->hdpi = gdk_screen_get_resolution(gdk_screen_get_default());
		this->ddpi = 96.0;
	}
	this->hFont = 0;
	this->fontName = 0;
	this->fontHeightPt = 0;
	this->fontIsBold = false;
	this->lxPos = 0;
	this->lyPos = 0;
	this->lxPos2 = 320;
	this->lyPos2 = 240;
	this->inited = false;
	this->dropHdlr = 0;
}

UI::GUIControl::~GUIControl()
{
	if (this->dropHdlr)
	{
		UI::GTK::GTKDragDrop *dragDrop = (UI::GTK::GTKDragDrop*)this->dropHdlr;
		DEL_CLASS(dragDrop);
	}
	OPTSTR_DEL(this->fontName);
	if (this->hFont)
	{
		pango_font_description_free((PangoFontDescription *)this->hFont);
		this->hFont = 0;
	}
}

void *UI::GUIControl::GetFont()
{
	NN<GUIClientControl> nnparent;
	if (this->hFont)
	{
		return this->hFont;
	}
	else if (this->parent.SetTo(nnparent))
	{
		return nnparent->GetFont();
	}
	else
	{
		return 0;
	}
}

void UI::GUIControl::Show()
{
	this->UpdateFont();
	gtk_widget_show((GtkWidget*)this->hwnd.OrNull());
	if (!this->inited)
	{
		g_signal_connect((GtkWidget*)this->hwnd.OrNull(), "configure-event", G_CALLBACK(GUIControl_SizeChanged), this);
		this->inited = true;
	}
}

void UI::GUIControl::Close()
{
//	gtk_widget_hide((GtkWidget*)this->hwnd);
	gtk_widget_destroy((GtkWidget*)this->hwnd.OrNull());
}

void UI::GUIControl::SetText(Text::CStringNN text)
{
	/////////////////////////////////
}

UnsafeArrayOpt<UTF8Char> UI::GUIControl::GetText(UnsafeArray<UTF8Char> buff)
{
	////////////////////////////////
	return 0;
}

Bool UI::GUIControl::GetText(NN<Text::StringBuilderUTF8> sb)
{
	///////////////////////////////
	return false;
}

void UI::GUIControl::SetSize(Double width, Double height)
{
	this->SetArea(this->lxPos, this->lyPos, this->lxPos + width, this->lyPos + height, true);
}

void UI::GUIControl::SetSizeP(Math::Size2D<UOSInt> size)
{
	this->SetArea(this->lxPos, this->lyPos, this->lxPos + UOSInt2Double(size.x) * this->ddpi / this->hdpi, this->lyPos + UOSInt2Double(size.y) * this->ddpi / this->hdpi, true);
}

Math::Size2DDbl UI::GUIControl::GetSize()
{
	return Math::Size2DDbl(this->lxPos2 - this->lxPos, this->lyPos2 - this->lyPos);
//	printf("Control.GetSize %lf, %lf\r\n", *width, *height);
}

Math::Size2D<UOSInt> UI::GUIControl::GetSizeP()
{
	return Math::Size2D<UOSInt>((UOSInt)Double2OSInt((this->lxPos2 - this->lxPos) * this->hdpi / this->ddpi),
		(UOSInt)Double2OSInt((this->lyPos2 - this->lyPos) * this->hdpi / this->ddpi));
//	printf("Control.GetSizeP %ld, %ld\r\n", (Int32)*width, (Int32)*height);
}

void UI::GUIControl::SetPosition(Double x, Double y)
{
	SetArea(x, y, x + this->lxPos2 - this->lxPos, y + this->lyPos2 - this->lyPos, true);
}

Math::Coord2D<OSInt> UI::GUIControl::GetPositionP()
{
	return Math::Coord2D<OSInt>(Double2Int32(this->lxPos * this->hdpi / this->ddpi), Double2Int32(this->lyPos * this->hdpi / this->ddpi));
}

Math::Coord2D<OSInt> UI::GUIControl::GetScreenPosP()
{
	return Math::Coord2D<OSInt>(0, 0);
}

void UI::GUIControl::SetArea(Double left, Double top, Double right, Double bottom, Bool updateScn)
{
	if (left == this->lxPos && top == this->lyPos && right == this->lxPos2 && bottom == this->lyPos2)
		return;
	Math::Coord2DDbl ofst = Math::Coord2DDbl(0, 0);
	NN<GUIClientControl> nnparent;
	if (this->parent.SetTo(nnparent))
	{
		ofst = nnparent->GetClientOfst();
	}
	this->lxPos = left;
	this->lyPos = top;
	this->selfResize = true;

	if (this->parent.SetTo(nnparent))
	{
		void *container = nnparent->GetContainer();
		gtk_fixed_move((GtkFixed*)container, (GtkWidget*)this->hwnd.OrNull(), Double2Int32((left + ofst.x) * this->hdpi / this->ddpi), Double2Int32((top + ofst.y) * this->hdpi / this->ddpi));
	}
	if ((right - left) < 0)
	{
		right = left;
	}
	if ((bottom - top) < 0)
	{
		bottom = top;
	}
	gtk_widget_set_size_request((GtkWidget*)this->hwnd.OrNull(), Double2Int32((right - left) * this->hdpi / this->ddpi), Double2Int32((bottom - top) * this->hdpi / this->ddpi));

	gint outW;
	gint outH;
	gtk_widget_get_size_request((GtkWidget*)this->hwnd.OrNull(), &outW, &outH);
//	printf("%s set size to %lf, %lf, Size = %d, %d\r\n", this->GetObjectClass(), right - left, bottom - top, outW, outH);
//	if (Text::StrEquals(this->GetObjectClass(), (const UTF8Char*)"Panel"))
//	{
//		printf("Panel (Parent = %s) set size to %lf, %lf, Size = %d, %d\r\n", this->parent->GetObjectClass(), right - left, bottom - top, outW, outH);
//	}
	if (outW == -1)
	{
		this->lxPos2 = right;
	}
	else
	{
		this->lxPos2 = left + outW * this->ddpi / this->hdpi;
	}
	if (outH == -1)
	{
		this->lyPos2 = bottom;
	}
	else
	{
		this->lyPos2 = top + outH * this->ddpi / this->hdpi;
	}
	this->selfResize = false;
	this->OnSizeChanged(updateScn);
}

void UI::GUIControl::SetAreaP(OSInt left, OSInt top, OSInt right, OSInt bottom, Bool updateScn)
{
	if (OSInt2Double(left) == this->lxPos && OSInt2Double(top) == this->lyPos && OSInt2Double(right) == this->lxPos2 && OSInt2Double(bottom) == this->lyPos2)
		return;
	Math::Coord2DDbl ofst = Math::Coord2DDbl(0, 0);
	NN<GUIClientControl> nnparent;
	if (this->parent.SetTo(nnparent))
	{
		ofst = nnparent->GetClientOfst();
	}
	this->lxPos = OSInt2Double(left) * this->ddpi / this->hdpi;
	this->lyPos = OSInt2Double(top) * this->ddpi / this->hdpi;
	this->selfResize = true;

	if (this->parent.SetTo(nnparent))
	{
		void *container = nnparent->GetContainer();
		gtk_fixed_move((GtkFixed*)container, (GtkWidget*)this->hwnd.OrNull(), Double2Int32(OSInt2Double(left) + ofst.x * this->hdpi / this->ddpi), Double2Int32(OSInt2Double(top) + ofst.y * this->hdpi / this->ddpi));
	}
	if ((right - left) < 0)
	{
		right = left;
	}
	if ((bottom - top) < 0)
	{
		bottom = top;
	}
	gtk_widget_set_size_request((GtkWidget*)this->hwnd.OrNull(), (gint)(right - left), (gint)(bottom - top));

	gint outW;
	gint outH;
	gtk_widget_get_size_request((GtkWidget*)this->hwnd.OrNull(), &outW, &outH);
	if (outW == -1)
	{
		this->lxPos2 = OSInt2Double(right) * this->ddpi / this->hdpi;
	}
	else
	{
		this->lxPos2 = OSInt2Double(left + outW) * this->ddpi / this->hdpi;
	}
	if (outH == -1)
	{
		this->lyPos2 = OSInt2Double(bottom) * this->ddpi / this->hdpi;
	}
	else
	{
		this->lyPos2 = OSInt2Double(top + outH) * this->ddpi / this->hdpi;
	}
	this->selfResize = false;
	this->OnSizeChanged(updateScn);
}

void UI::GUIControl::SetRect(Double left, Double top, Double width, Double height, Bool updateScn)
{
	this->SetArea(left, top, left + width, top + height, updateScn);
}

void UI::GUIControl::SetFont(UnsafeArrayOpt<const UTF8Char> name, UOSInt nameLen, Double ptSize, Bool isBold)
{
	OPTSTR_DEL(this->fontName);
	UnsafeArray<const UTF8Char> nnname;
	if (name.SetTo(nnname))
	{
		this->fontName = Text::String::New(nnname, nameLen);
	}
	this->fontHeightPt = ptSize;
	this->fontIsBold = isBold;
	this->InitFont();
}

void UI::GUIControl::InitFont()
{
	PangoFontDescription *font = pango_font_description_new();
	NN<Text::String> nns;
	if (this->fontName.SetTo(nns))
	{
		pango_font_description_set_family(font, (const Char*)nns->v.Ptr());
	}
	pango_font_description_set_absolute_size(font, this->fontHeightPt * this->hdpi / this->ddpi * PANGO_SCALE / 0.75);
	if (this->fontIsBold)
		pango_font_description_set_weight(font, PANGO_WEIGHT_BOLD);
	if (this->hFont)
	{
		pango_font_description_free((PangoFontDescription *)this->hFont);
	}
	this->hFont = font;
#if GDK_VERSION_AFTER(3, 16)
	Text::CSSBuilder builder(Text::CSSBuilder::PM_SPACE);
	builder.NewStyle(CSTR("label"), nullptr);
	if (this->fontName.SetTo(nns)) builder.AddFontFamily(nns->v);
	if (this->fontHeightPt != 0) builder.AddFontSize(this->fontHeightPt * this->hdpi / this->ddpi, Math::Unit::Distance::DU_PIXEL);
	if (this->fontIsBold) builder.AddFontWeight(Text::CSSBuilder::FONT_WEIGHT_BOLD);
	GtkWidget *widget = (GtkWidget*)this->GetDisplayHandle().OrNull();
	GtkStyleContext *style = gtk_widget_get_style_context(widget);
	GtkCssProvider *styleProvider = gtk_css_provider_new();
	gtk_css_provider_load_from_data(styleProvider, (const gchar*)builder.ToString().Ptr(), -1, 0);
	gtk_style_context_add_provider(style, (GtkStyleProvider*)styleProvider, GTK_STYLE_PROVIDER_PRIORITY_USER);
	gtk_widget_reset_style(widget);
#else
	gtk_widget_override_font((GtkWidget*)this->GetDisplayHandle().OrNull(), font);
#endif
}

void UI::GUIControl::SetDockType(UI::GUIControl::DockType dockType)
{
	if (this->dockType != dockType)
	{
		this->dockType = dockType;
		NN<GUIClientControl> nnparent;
		if (this->parent.SetTo(nnparent))
		{
			nnparent->UpdateChildrenSize(true);
		}
	}
}

UI::GUIControl::DockType UI::GUIControl::GetDockType()
{
	return this->dockType;
}

void UI::GUIControl::SetVisible(Bool isVisible)
{
	if (isVisible)
		gtk_widget_show((GtkWidget*)this->hwnd.OrNull());
	else
		gtk_widget_hide((GtkWidget*)this->hwnd.OrNull());
}

Bool UI::GUIControl::GetVisible()
{
	//////////////////////////////////////////
	return true;//gtk_widget_is_visible((GtkWidget*)this->window);
}

void UI::GUIControl::SetEnabled(Bool isEnable)
{
	gtk_widget_set_sensitive((GtkWidget*)this->hwnd.OrNull(), isEnable);
}

void UI::GUIControl::SetBGColor(UInt32 bgColor)
{
#if GDK_VERSION_AFTER(3, 16)
	Text::CSSBuilder builder(Text::CSSBuilder::PM_SPACE);
	builder.NewStyle(nullptr, nullptr);
	builder.AddBGColorRGBA(bgColor);

	GtkWidget *widget = (GtkWidget*)this->hwnd.OrNull();
	GtkStyleContext *style = gtk_widget_get_style_context(widget);
	GtkCssProvider *styleProvider = gtk_css_provider_new();
	gtk_css_provider_load_from_data(styleProvider, (const gchar*)builder.ToString().Ptr(), -1, 0);
	gtk_style_context_add_provider(style, (GtkStyleProvider*)styleProvider, GTK_STYLE_PROVIDER_PRIORITY_USER);
#else
	if (bgColor)
	{
		GdkRGBA color;
		color.red = (bgColor & 0xff0000) / 16711680.0;
		color.green = (bgColor & 0xff00) / 65280.0;
		color.blue = (bgColor & 0xff) / 255.0;
		color.alpha = ((bgColor >> 24) & 0xff) / 255.0;
		gtk_widget_override_background_color((GtkWidget*)this->hwnd.OrNull(), GTK_STATE_FLAG_NORMAL, &color);
	}
	else
	{
		gtk_widget_override_background_color((GtkWidget*)this->hwnd.OrNull(), GTK_STATE_FLAG_NORMAL, 0);
	}
#endif
}

Bool UI::GUIControl::IsFormFocused()
{
	//////////////////////////////////////
	return false;
}

void UI::GUIControl::Focus()
{
	gtk_widget_grab_focus((GtkWidget*)this->hwnd.OrNull());
}

OSInt UI::GUIControl::GetScrollHPos()
{
	return 0;
}

OSInt UI::GUIControl::GetScrollVPos()
{
	return 0;
}

void UI::GUIControl::ScrollTo(OSInt x, OSInt y)
{
}

void UI::GUIControl::OnSizeChanged(Bool updateScn)
{
	if (this->selfResize)
	{
		return;
	}

	gint outW;
	gint outH;
	gtk_widget_get_size_request((GtkWidget*)this->hwnd.OrNull(), &outW, &outH);
	if (outW != -1)
	{
		this->lxPos2 = this->lxPos + outW * this->ddpi / this->hdpi;
	}
	if (outH != -1)
	{
		this->lyPos2 = this->lyPos + outH * this->ddpi / this->hdpi;
	}

	UOSInt i = this->resizeHandlers.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<UIEvent> cb = this->resizeHandlers.GetItem(i);
		cb.func(cb.userObj);
	}
}

void UI::GUIControl::OnPosChanged(Bool updateScn)
{
	/////////////////////////	
}

void UI::GUIControl::OnShow()
{
	/////////////////////////	
}

void UI::GUIControl::OnMonitorChanged()
{
	/////////////////////////	
}

void UI::GUIControl::HandleSizeChanged(UIEvent handler, AnyType userObj)
{
	this->resizeHandlers.Add({handler, userObj});
}

void UI::GUIControl::UpdateFont()
{
	void *font = GetFont();
	if (font)
	{
#if GDK_VERSION_AFTER(3, 16)
		const Char *family = pango_font_description_get_family((PangoFontDescription*)font);
		PangoWeight weight = pango_font_description_get_weight((PangoFontDescription*)font);
		Double height = pango_font_description_get_size((PangoFontDescription*)font) / (Double)PANGO_SCALE;
		
		Text::CSSBuilder builder(Text::CSSBuilder::PM_SPACE);
		builder.NewStyle(nullptr, nullptr);
		UnsafeArray<const UTF8Char> nnfamily;
		if (nnfamily.Set((const UTF8Char*)family)) builder.AddFontFamily(nnfamily);
		if (height != 0) builder.AddFontSize(height, Math::Unit::Distance::DU_PIXEL);
		if (weight != 0) builder.AddFontWeight((Text::CSSBuilder::FontWeight)weight);

		GtkWidget *widget = (GtkWidget*)this->hwnd.OrNull();
		GtkStyleContext *style = gtk_widget_get_style_context(widget);
		GtkCssProvider *styleProvider = gtk_css_provider_new();
		gtk_css_provider_load_from_data(styleProvider, (const gchar*)builder.ToString().Ptr(), -1, 0);
		gtk_style_context_add_provider(style, (GtkStyleProvider*)styleProvider, GTK_STYLE_PROVIDER_PRIORITY_USER);
		gtk_widget_reset_style(widget);
#else		
		gtk_widget_override_font((GtkWidget*)this->hwnd.OrNull(), (PangoFontDescription*)font);
#endif
	}
}

void UI::GUIControl::UpdatePos(Bool redraw)
{
	Bool isForm = false;
	if (this->GetObjectClass().Equals(UTF8STRC("WinForm")))
	{
		isForm = true;
		if (gtk_window_is_maximized((GtkWindow*)this->hwnd.OrNull()))
		{
			return;
		}
	}

	NN<GUIClientControl> nnparent;
	if (this->parent.SetTo(nnparent))
	{
		Math::Coord2DDbl ofst = nnparent->GetClientOfst();
		void *container = nnparent->GetContainer();
		gtk_fixed_move((GtkFixed*)container, (GtkWidget*)this->hwnd.OrNull(), Double2Int32((this->lxPos + ofst.x) * this->hdpi / this->ddpi), Double2Int32((this->lyPos + ofst.y) * this->hdpi / this->ddpi));
		if (this->lxPos2 < this->lxPos)
		{
			this->lxPos2 = this->lxPos;
		}
		if (this->lyPos2 < this->lyPos)
		{
			this->lyPos2 = this->lyPos;
		}
		gtk_widget_set_size_request((GtkWidget*)this->hwnd.OrNull(), Double2Int32((this->lxPos2 - this->lxPos) * this->hdpi / this->ddpi), Double2Int32((this->lyPos2 - this->lyPos) * this->hdpi / this->ddpi));
	}
	else if (isForm)
	{
		Double newW = (this->lxPos2 - this->lxPos) * this->hdpi / this->ddpi;
		Double newH = (this->lyPos2 - this->lyPos) * this->hdpi / this->ddpi;
		Optional<Media::MonitorInfo> monInfo = this->GetMonitorInfo();
		NN<Media::MonitorInfo> nnmonInfo;
		Double newX;
		Double newY;
		gint winX;
		gint winY;
		gtk_window_get_position((GtkWindow*)this->hwnd.OrNull(), &winX, &winY);
		Math::Size2D<UOSInt> winSize = this->GetSizeP();
		if (monInfo.SetTo(nnmonInfo))
		{
			Int32 maxW = nnmonInfo->GetPixelWidth();
			Int32 maxH = nnmonInfo->GetPixelHeight();
			if (newW > maxW)
				newW = maxW;
			if (newH > maxH)
				newH = maxH;
			newX = (OSInt2Double(winX + winX + (OSInt)winSize.GetWidth()) - newW) * 0.5;
			newY = (OSInt2Double(winY + winY + (OSInt)winSize.GetHeight()) - newH) * 0.5;
			if (newY < nnmonInfo->GetTop())
			{
				newY = nnmonInfo->GetTop();
			}
			nnmonInfo.Delete();
		}
		else
		{
			int maxX = 1024;
			int maxY = 768;
			if (newW > maxX)
				newW = maxX;
			if (newH > maxY)
				newH = maxY;
			newX = (OSInt2Double(winX + winX + (OSInt)winSize.GetWidth()) - newW) * 0.5;
			newY = (OSInt2Double(winY + winY + (OSInt)winSize.GetHeight()) - newH) * 0.5;
			if (newY < 0)
			{
				newY = 0;
			}
		}

		gtk_window_move((GtkWindow*)this->hwnd.OrNull(), Double2Int32(newX), Double2Int32(newY));
		if (gtk_window_get_resizable((GtkWindow*)this->hwnd.OrNull()))
		{
			gtk_window_resize((GtkWindow*)this->hwnd.OrNull(), Double2Int32(newW), Double2Int32(newH));
		}
		else
		{
			gtk_window_set_resizable((GtkWindow*)this->hwnd.OrNull(), true);
			gtk_window_set_default_size((GtkWindow*)this->hwnd.OrNull(), Double2Int32(newW), Double2Int32(newH));
			gtk_window_resize((GtkWindow*)this->hwnd.OrNull(), Double2Int32(newW), Double2Int32(newH));
			g_idle_add(GUIControl_SetNoResize, this->hwnd.OrNull());
		}
	}
	else if (this->GetObjectClass().Equals(UTF8STRC("TabPage")))
	{

	}
	else
	{
		printf("%s is not WinForm without parent\r\n", this->GetObjectClass().v.Ptr());
	}
}

void UI::GUIControl::Redraw()
{
	g_idle_add(GUIControl_ToGenDrawSignal, this->hwnd.OrNull());
}

void UI::GUIControl::SetCapture()
{
#if GDK_VERSION_AFTER(3, 20)
	GdkSeat *seat = gdk_display_get_default_seat(gdk_display_get_default());
	if (seat)
	{
		gdk_seat_grab(seat, gtk_widget_get_window((GtkWidget*)this->hwnd.OrNull()), GDK_SEAT_CAPABILITY_ALL_POINTING, TRUE, 0, 0, 0, 0);
	}
#else
	GdkDevice *dev = gtk_get_current_event_device();
	if (dev == 0)
	{
	}
	if (dev)
	{
		gdk_device_grab(dev, gtk_widget_get_window((GtkWidget*)this->hwnd.OrNull()), GDK_OWNERSHIP_WINDOW, TRUE, (GdkEventMask)(GDK_BUTTON_MOTION_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK), 0, GDK_CURRENT_TIME);
	}
#endif
}
void UI::GUIControl::ReleaseCapture()
{
#if GDK_VERSION_AFTER(3, 20)
	GdkSeat *seat = gdk_display_get_default_seat(gdk_display_get_default());
	if (seat)
	{
		gdk_seat_ungrab(seat);
	}
#else
	GdkDevice *dev = gtk_get_current_event_device();
	if (dev == 0)
	{
	}
	if (dev)
	{
		gdk_device_ungrab(dev, GDK_CURRENT_TIME);
	}
#endif
}

void UI::GUIControl::SetCursor(CursorType curType)
{
	///////////////////////////
	//gtk_window_set_cursor();
}

UInt32 GdkRGBA2Color(GdkRGBA *rgba)
{
	UInt32 ia = Math::SDouble2UInt8(rgba->alpha * 255);
	UInt32 ir = Math::SDouble2UInt8(rgba->red * 255);
	UInt32 ig = Math::SDouble2UInt8(rgba->green * 255);
	UInt32 ib = Math::SDouble2UInt8(rgba->blue * 255);
	return (ia << 24) | (ir << 16) | (ig << 8) | ib;
}

UInt32 UI::GUIControl::GetColorBg()
{
	GdkRGBA color;
	GtkStyleContext *style = gtk_widget_get_style_context((GtkWidget*)this->hwnd.OrNull());
	gtk_style_context_get_color(style, GTK_STATE_FLAG_NORMAL, &color);
	if ((color.red + color.green + color.blue) > 1.5)
	{
		return 0xff000000;
	}
	else
	{
		return 0xffffffff;
	}
}

UInt32 UI::GUIControl::GetColorText()
{
	GdkRGBA color;
	GtkStyleContext *style = gtk_widget_get_style_context((GtkWidget*)this->hwnd.OrNull());
	gtk_style_context_get_color(style, GTK_STATE_FLAG_NORMAL, &color);
	return GdkRGBA2Color(&color);
}

UInt32 UI::GUIControl::GetColorTextAlt()
{
	GdkRGBA color;
	GtkStyleContext *style = gtk_widget_get_style_context((GtkWidget*)this->hwnd.OrNull());
	gtk_style_context_get_color(style, GTK_STATE_FLAG_LINK, &color);
	return GdkRGBA2Color(&color);
}

UInt32 UI::GUIControl::GetColorHightlight()
{
	GdkRGBA color;
	GtkStyleContext *style = gtk_widget_get_style_context((GtkWidget*)this->hwnd.OrNull());
	gtk_style_context_get_color(style, GTK_STATE_FLAG_SELECTED, &color);
	if ((color.red + color.green + color.blue) > 1.5)
	{
		return 0xff3333ff;
	}
	else
	{
		return 0xffccccff;
	}
}

UInt32 UI::GUIControl::GetColorHightlightText()
{
	GdkRGBA color;
	GtkStyleContext *style = gtk_widget_get_style_context((GtkWidget*)this->hwnd.OrNull());
	gtk_style_context_get_color(style, GTK_STATE_FLAG_SELECTED, &color);
	return GdkRGBA2Color(&color);
}

Optional<UI::GUIClientControl> UI::GUIControl::GetParent()
{
	return this->parent;
}

Optional<UI::GUIForm> UI::GUIControl::GetRootForm()
{
	UI::GUIControl *ctrl = this;
	Text::CStringNN objCls;
	while (ctrl)
	{
		objCls = ctrl->GetObjectClass();
		if (objCls.Equals(UTF8STRC("WinForm")))
		{
			if (ctrl->GetParent().IsNull())
				return (UI::GUIForm*)ctrl;
		}
		ctrl = ctrl->GetParent().OrNull();
	}
	return 0;
}

Optional<ControlHandle> UI::GUIControl::GetHandle()
{
	return this->hwnd;
}

Optional<ControlHandle> UI::GUIControl::GetDisplayHandle()
{
	return this->hwnd;
}

Optional<MonitorHandle> UI::GUIControl::GetHMonitor()
{
#if GDK_VERSION_AFTER(3, 22)
	GdkDisplay *display = gtk_widget_get_display((GtkWidget*)this->hwnd.OrNull());
	GdkWindow *wnd = gtk_widget_get_window((GtkWidget*)this->hwnd.OrNull());
	if (display == 0)
		return 0;
	MonitorHandle *ret;
	if (wnd == 0)
	{
		ret = (MonitorHandle*)(OSInt)1;
	}
	else
	{
		GdkMonitor *mon = gdk_display_get_monitor_at_window(display, wnd);
		GdkMonitor *mon2;
		ret = (MonitorHandle*)(OSInt)1;
		int i = 0;
		int j = gdk_display_get_n_monitors(display);
		while (i < j)
		{
			mon2 = gdk_display_get_monitor(display, i);
			if (mon == mon2)
			{
				ret = (MonitorHandle*)(OSInt)(1 + i);
				break;
			}
			i++;
		}
	}
	return ret;
#else
	GdkScreen *scn = gtk_widget_get_screen((GtkWidget*)this->hwnd.OrNull());
	GdkWindow *wnd = gtk_widget_get_window((GtkWidget*)this->hwnd.OrNull());
	if (scn == 0)
		return 0;
	if (wnd == 0)
	{
		wnd = gdk_screen_get_active_window(scn);
	}
	MonitorHandle *ret;
	if (wnd == 0)
	{
		ret = (MonitorHandle*)(OSInt)1;
	}
	else
	{
		ret = (MonitorHandle*)(OSInt)(1 + gdk_screen_get_monitor_at_window(scn, wnd));
	}
	return ret;
#endif
}

Optional<Media::MonitorInfo> UI::GUIControl::GetMonitorInfo()
{
	GdkDisplay *display = gtk_widget_get_display((GtkWidget*)this->hwnd.OrNull());
	if (display)
	{
		Media::MonitorInfo *info;
		NEW_CLASS(info, Media::MonitorInfo(0));
		return info;
	}
	else
	{
		return 0;
	}
}

void UI::GUIControl::SetDPI(Double hdpi, Double ddpi)
{
	this->hdpi = hdpi;
	this->ddpi = ddpi;
	this->UpdatePos(true);
	if (this->hFont)
	{
		this->InitFont();
	}
	else
	{
		this->UpdateFont();
	}
}

Double UI::GUIControl::GetHDPI()
{
	return this->hdpi;
}

Double UI::GUIControl::GetDDPI()
{
	return this->ddpi;
}

Optional<Media::DrawFont> UI::GUIControl::CreateDrawFont(NN<Media::DrawImage> img)
{
	PangoFontDescription *f = (PangoFontDescription *)this->GetFont();
	if (f == 0)
		return 0;
	NN<Text::String> nnfontName;
	if (!this->fontName.SetTo(nnfontName))
	{
		PangoFontDescription *fnt = pango_font_description_copy(f);
		const Char *family = pango_font_description_get_family(fnt);
		PangoStyle style = pango_font_description_get_style(fnt);
		PangoWeight weight = pango_font_description_get_weight(fnt);
		Double height = pango_font_description_get_size(fnt) / (Double)PANGO_SCALE;

		if (family == 0)
		{
			family = "Arial";
		}
		Media::DrawFont *font;
		NN<Text::String> fntName = Text::String::NewNotNullSlow((const UTF8Char*)family);
		NEW_CLASS(font, Media::GTKDrawFont(fntName.Ptr(), height, (OSInt)((style & PANGO_STYLE_ITALIC)?CAIRO_FONT_SLANT_ITALIC:CAIRO_FONT_SLANT_NORMAL), (weight < PANGO_WEIGHT_BOLD)?0:1));
		fntName->Release();
		pango_font_description_free(fnt);
		return font;
	}
	else
	{
		return img->NewFontPt(nnfontName->ToCString(), this->fontHeightPt * this->hdpi / this->ddpi, this->fontIsBold?Media::DrawEngine::DFS_BOLD:Media::DrawEngine::DFS_NORMAL, 0).Ptr();
	}
}

UInt32 UI::GUIControl::GUIKey2OSKey(UI::GUIControl::GUIKey guiKey)
{
	switch (guiKey)
	{
	case UI::GUIControl::GK_BACKSPACE:
		return GDK_KEY_BackSpace;
	case UI::GUIControl::GK_TAB:
		return GDK_KEY_Tab;
	case UI::GUIControl::GK_CLEAR:
		return GDK_KEY_Clear;
	case UI::GUIControl::GK_ENTER:
		return GDK_KEY_Return;
	case UI::GUIControl::GK_SHIFT:
		return GDK_KEY_Shift_L;
	case UI::GUIControl::GK_CONTROL:
		return GDK_KEY_Control_L;
	case UI::GUIControl::GK_ALT:
		return GDK_KEY_Alt_L;
	case UI::GUIControl::GK_PAUSE:
		return GDK_KEY_Pause;
	case UI::GUIControl::GK_CAPITAL:
		return GDK_KEY_Caps_Lock;
	case UI::GUIControl::GK_KANA:
		return GDK_KEY_Kana_Shift;
	case UI::GUIControl::GK_JUNJA:
		return GDK_KEY_Romaji;
	case UI::GUIControl::GK_FINAL:
		return GDK_KEY_Muhenkan;
	case UI::GUIControl::GK_KANJI:
		return GDK_KEY_Kanji;
	case UI::GUIControl::GK_ESCAPE:
		return GDK_KEY_Escape;
	case UI::GUIControl::GK_CONVERT:
		return GDK_KEY_Henkan;
	case UI::GUIControl::GK_NONCONVERT:
		return GDK_KEY_Katakana;
	case UI::GUIControl::GK_ACCEPT:
		return GDK_KEY_Select;
	case UI::GUIControl::GK_MODECHANGE:
		return GDK_KEY_Mode_switch;
	case UI::GUIControl::GK_SPACE:
		return GDK_KEY_space;
	case UI::GUIControl::GK_PAGEUP:
		return GDK_KEY_Page_Up;
	case UI::GUIControl::GK_PAGEDOWN:
		return GDK_KEY_Page_Down;
	case UI::GUIControl::GK_END:
		return GDK_KEY_End;
	case UI::GUIControl::GK_HOME:
		return GDK_KEY_Home;
	case UI::GUIControl::GK_LEFT:
		return GDK_KEY_Left;
	case UI::GUIControl::GK_UP:
		return GDK_KEY_Up;
	case UI::GUIControl::GK_RIGHT:
		return GDK_KEY_Right;
	case UI::GUIControl::GK_DOWN:
		return GDK_KEY_Down;
	case UI::GUIControl::GK_SELECT:
		return GDK_KEY_Select;
	case UI::GUIControl::GK_PRINT:
		return GDK_KEY_Print;
	case UI::GUIControl::GK_EXECUTE:
		return GDK_KEY_Execute;
	case UI::GUIControl::GK_PRINTSCREEN:
		return GDK_KEY_3270_PrintScreen;
	case UI::GUIControl::GK_INSERT:
		return GDK_KEY_Insert;
	case UI::GUIControl::GK_DELETE:
		return GDK_KEY_Delete;
	case UI::GUIControl::GK_HELP:
		return GDK_KEY_Help;
	case UI::GUIControl::GK_0:
		return GDK_KEY_0;
	case UI::GUIControl::GK_1:
		return GDK_KEY_1;
	case UI::GUIControl::GK_2:
		return GDK_KEY_2;
	case UI::GUIControl::GK_3:
		return GDK_KEY_3;
	case UI::GUIControl::GK_4:
		return GDK_KEY_4;
	case UI::GUIControl::GK_5:
		return GDK_KEY_5;
	case UI::GUIControl::GK_6:
		return GDK_KEY_6;
	case UI::GUIControl::GK_7:
		return GDK_KEY_7;
	case UI::GUIControl::GK_8:
		return GDK_KEY_8;
	case UI::GUIControl::GK_9:
		return GDK_KEY_9;
	case UI::GUIControl::GK_A:
		return GDK_KEY_a;
	case UI::GUIControl::GK_B:
		return GDK_KEY_b;
	case UI::GUIControl::GK_C:
		return GDK_KEY_c;
	case UI::GUIControl::GK_D:
		return GDK_KEY_d;
	case UI::GUIControl::GK_E:
		return GDK_KEY_e;
	case UI::GUIControl::GK_F:
		return GDK_KEY_f;
	case UI::GUIControl::GK_G:
		return GDK_KEY_g;
	case UI::GUIControl::GK_H:
		return GDK_KEY_h;
	case UI::GUIControl::GK_I:
		return GDK_KEY_i;
	case UI::GUIControl::GK_J:
		return GDK_KEY_j;
	case UI::GUIControl::GK_K:
		return GDK_KEY_k;
	case UI::GUIControl::GK_L:
		return GDK_KEY_l;
	case UI::GUIControl::GK_M:
		return GDK_KEY_m;
	case UI::GUIControl::GK_N:
		return GDK_KEY_n;
	case UI::GUIControl::GK_O:
		return GDK_KEY_o;
	case UI::GUIControl::GK_P:
		return GDK_KEY_p;
	case UI::GUIControl::GK_Q:
		return GDK_KEY_q;
	case UI::GUIControl::GK_R:
		return GDK_KEY_r;
	case UI::GUIControl::GK_S:
		return GDK_KEY_s;
	case UI::GUIControl::GK_T:
		return GDK_KEY_t;
	case UI::GUIControl::GK_U:
		return GDK_KEY_u;
	case UI::GUIControl::GK_V:
		return GDK_KEY_v;
	case UI::GUIControl::GK_W:
		return GDK_KEY_w;
	case UI::GUIControl::GK_X:
		return GDK_KEY_x;
	case UI::GUIControl::GK_Y:
		return GDK_KEY_y;
	case UI::GUIControl::GK_Z:
		return GDK_KEY_z;
	case UI::GUIControl::GK_LWIN:
		return GDK_KEY_Super_L;
	case UI::GUIControl::GK_RWIN:
		return GDK_KEY_Super_R;
	case UI::GUIControl::GK_APPS:
		return GDK_KEY_WWW;
	case UI::GUIControl::GK_SLEEP:
		return GDK_KEY_Sleep;
	case UI::GUIControl::GK_NUMPAD0:
		return GDK_KEY_KP_0;
	case UI::GUIControl::GK_NUMPAD1:
		return GDK_KEY_KP_1;
	case UI::GUIControl::GK_NUMPAD2:
		return GDK_KEY_KP_2;
	case UI::GUIControl::GK_NUMPAD3:
		return GDK_KEY_KP_3;
	case UI::GUIControl::GK_NUMPAD4:
		return GDK_KEY_KP_4;
	case UI::GUIControl::GK_NUMPAD5:
		return GDK_KEY_KP_5;
	case UI::GUIControl::GK_NUMPAD6:
		return GDK_KEY_KP_6;
	case UI::GUIControl::GK_NUMPAD7:
		return GDK_KEY_KP_7;
	case UI::GUIControl::GK_NUMPAD8:
		return GDK_KEY_KP_8;
	case UI::GUIControl::GK_NUMPAD9:
		return GDK_KEY_KP_9;
	case UI::GUIControl::GK_MULTIPLY:
		return GDK_KEY_KP_Multiply;
	case UI::GUIControl::GK_ADD:
		return GDK_KEY_KP_Add;
	case UI::GUIControl::GK_SEPARATOR:
		return GDK_KEY_KP_Separator;
	case UI::GUIControl::GK_SUBTRACT:
		return GDK_KEY_KP_Subtract;
	case UI::GUIControl::GK_DECIMAL:
		return GDK_KEY_KP_Decimal;
	case UI::GUIControl::GK_DIVIDE:
		return GDK_KEY_KP_Divide;
	case UI::GUIControl::GK_F1:
		return GDK_KEY_F1;
	case UI::GUIControl::GK_F2:
		return GDK_KEY_F2;
	case UI::GUIControl::GK_F3:
		return GDK_KEY_F3;
	case UI::GUIControl::GK_F4:
		return GDK_KEY_F4;
	case UI::GUIControl::GK_F5:
		return GDK_KEY_F5;
	case UI::GUIControl::GK_F6:
		return GDK_KEY_F6;
	case UI::GUIControl::GK_F7:
		return GDK_KEY_F7;
	case UI::GUIControl::GK_F8:
		return GDK_KEY_F8;
	case UI::GUIControl::GK_F9:
		return GDK_KEY_F9;
	case UI::GUIControl::GK_F10:
		return GDK_KEY_F10;
	case UI::GUIControl::GK_F11:
		return GDK_KEY_F11;
	case UI::GUIControl::GK_F12:
		return GDK_KEY_F12;
	case UI::GUIControl::GK_F13:
		return GDK_KEY_F13;
	case UI::GUIControl::GK_F14:
		return GDK_KEY_F14;
	case UI::GUIControl::GK_F15:
		return GDK_KEY_F15;
	case UI::GUIControl::GK_F16:
		return GDK_KEY_F16;
	case UI::GUIControl::GK_F17:
		return GDK_KEY_F17;
	case UI::GUIControl::GK_F18:
		return GDK_KEY_F18;
	case UI::GUIControl::GK_F19:
		return GDK_KEY_F19;
	case UI::GUIControl::GK_F20:
		return GDK_KEY_F20;
	case UI::GUIControl::GK_F21:
		return GDK_KEY_F21;
	case UI::GUIControl::GK_F22:
		return GDK_KEY_F22;
	case UI::GUIControl::GK_F23:
		return GDK_KEY_F23;
	case UI::GUIControl::GK_F24:
		return GDK_KEY_F24;
	case UI::GUIControl::GK_NUMLOCK:
		return GDK_KEY_Num_Lock;
	case UI::GUIControl::GK_SCROLLLOCK:
		return GDK_KEY_Scroll_Lock;
	case UI::GUIControl::GK_OEM_1:
	case UI::GUIControl::GK_OEM_PLUS:
	case UI::GUIControl::GK_OEM_COMMA:
	case UI::GUIControl::GK_OEM_MINUS:
	case UI::GUIControl::GK_OEM_PERIOD:
	case UI::GUIControl::GK_OEM_2:
	case UI::GUIControl::GK_OEM_3:
	case UI::GUIControl::GK_OEM_4:
	case UI::GUIControl::GK_OEM_5:
	case UI::GUIControl::GK_OEM_6:
	case UI::GUIControl::GK_OEM_7:
	case UI::GUIControl::GK_NONE:
	default:
		return GDK_KEY_VoidSymbol;
	}
}

UI::GUIControl::GUIKey UI::GUIControl::OSKey2GUIKey(UInt32 osKey)
{
	switch (osKey)
	{
	case GDK_KEY_BackSpace:
		return UI::GUIControl::GK_BACKSPACE;
	case GDK_KEY_Tab:
		return UI::GUIControl::GK_TAB;
	case GDK_KEY_Clear:
		return UI::GUIControl::GK_CLEAR;
	case GDK_KEY_Return:
		return UI::GUIControl::GK_ENTER;
	case GDK_KEY_Shift_L:
		return UI::GUIControl::GK_SHIFT;
	case GDK_KEY_Control_L:
		return UI::GUIControl::GK_CONTROL;
	case GDK_KEY_Alt_L:
		return UI::GUIControl::GK_ALT;
	case GDK_KEY_Pause:
		return UI::GUIControl::GK_PAUSE;
	case GDK_KEY_Caps_Lock:
		return UI::GUIControl::GK_CAPITAL;
	case GDK_KEY_Kana_Shift:
		return UI::GUIControl::GK_KANA;
	case GDK_KEY_Romaji:
		return UI::GUIControl::GK_JUNJA;
	case GDK_KEY_Muhenkan:
		return UI::GUIControl::GK_FINAL;
	case GDK_KEY_Kanji:
		return UI::GUIControl::GK_KANJI;
	case GDK_KEY_Escape:
		return UI::GUIControl::GK_ESCAPE;
	case GDK_KEY_Henkan:
		return UI::GUIControl::GK_CONVERT;
	case GDK_KEY_Katakana:
		return UI::GUIControl::GK_NONCONVERT;
	case GDK_KEY_Mode_switch:
		return UI::GUIControl::GK_MODECHANGE;
	case GDK_KEY_space:
		return UI::GUIControl::GK_SPACE;
	case GDK_KEY_Page_Up:
		return UI::GUIControl::GK_PAGEUP;
	case GDK_KEY_Page_Down:
		return UI::GUIControl::GK_PAGEDOWN;
	case GDK_KEY_End:
		return UI::GUIControl::GK_END;
	case GDK_KEY_Home:
		return UI::GUIControl::GK_HOME;
	case GDK_KEY_Left:
		return UI::GUIControl::GK_LEFT;
	case GDK_KEY_Up:
		return UI::GUIControl::GK_UP;
	case GDK_KEY_Right:
		return UI::GUIControl::GK_RIGHT;
	case GDK_KEY_Down:
		return UI::GUIControl::GK_DOWN;
	case GDK_KEY_Select:
		return UI::GUIControl::GK_SELECT;
	case GDK_KEY_Print:
		return UI::GUIControl::GK_PRINT;
	case GDK_KEY_Execute:
		return UI::GUIControl::GK_EXECUTE;
	case GDK_KEY_3270_PrintScreen:
		return UI::GUIControl::GK_PRINTSCREEN;
	case GDK_KEY_Insert:
		return UI::GUIControl::GK_INSERT;
	case GDK_KEY_Delete:
		return UI::GUIControl::GK_DELETE;
	case GDK_KEY_Help:
		return UI::GUIControl::GK_HELP;
	case GDK_KEY_0:
		return UI::GUIControl::GK_0;
	case GDK_KEY_1:
		return UI::GUIControl::GK_1;
	case GDK_KEY_2:
		return UI::GUIControl::GK_2;
	case GDK_KEY_3:
		return UI::GUIControl::GK_3;
	case GDK_KEY_4:
		return UI::GUIControl::GK_4;
	case GDK_KEY_5:
		return UI::GUIControl::GK_5;
	case GDK_KEY_6:
		return UI::GUIControl::GK_6;
	case GDK_KEY_7:
		return UI::GUIControl::GK_7;
	case GDK_KEY_8:
		return UI::GUIControl::GK_8;
	case GDK_KEY_9:
		return UI::GUIControl::GK_9;
	case GDK_KEY_a:
		return UI::GUIControl::GK_A;
	case GDK_KEY_b:
		return UI::GUIControl::GK_B;
	case GDK_KEY_c:
		return UI::GUIControl::GK_C;
	case GDK_KEY_d:
		return UI::GUIControl::GK_D;
	case GDK_KEY_e:
		return UI::GUIControl::GK_E;
	case GDK_KEY_f:
		return UI::GUIControl::GK_F;
	case GDK_KEY_g:
		return UI::GUIControl::GK_G;
	case GDK_KEY_h:
		return UI::GUIControl::GK_H;
	case GDK_KEY_i:
		return UI::GUIControl::GK_I;
	case GDK_KEY_j:
		return UI::GUIControl::GK_J;
	case GDK_KEY_k:
		return UI::GUIControl::GK_K;
	case GDK_KEY_l:
		return UI::GUIControl::GK_L;
	case GDK_KEY_m:
		return UI::GUIControl::GK_M;
	case GDK_KEY_n:
		return UI::GUIControl::GK_N;
	case GDK_KEY_o:
		return UI::GUIControl::GK_O;
	case GDK_KEY_p:
		return UI::GUIControl::GK_P;
	case GDK_KEY_q:
		return UI::GUIControl::GK_Q;
	case GDK_KEY_r:
		return UI::GUIControl::GK_R;
	case GDK_KEY_s:
		return UI::GUIControl::GK_S;
	case GDK_KEY_t:
		return UI::GUIControl::GK_T;
	case GDK_KEY_u:
		return UI::GUIControl::GK_U;
	case GDK_KEY_v:
		return UI::GUIControl::GK_V;
	case GDK_KEY_w:
		return UI::GUIControl::GK_W;
	case GDK_KEY_x:
		return UI::GUIControl::GK_X;
	case GDK_KEY_y:
		return UI::GUIControl::GK_Y;
	case GDK_KEY_z:
		return UI::GUIControl::GK_Z;
	case GDK_KEY_Super_L:
		return UI::GUIControl::GK_LWIN;
	case GDK_KEY_Super_R:
		return UI::GUIControl::GK_RWIN;
	case GDK_KEY_WWW:
		return UI::GUIControl::GK_APPS;
	case GDK_KEY_Sleep:
		return UI::GUIControl::GK_SLEEP;
	case GDK_KEY_KP_0:
		return UI::GUIControl::GK_NUMPAD0;
	case GDK_KEY_KP_1:
		return UI::GUIControl::GK_NUMPAD1;
	case GDK_KEY_KP_2:
		return UI::GUIControl::GK_NUMPAD2;
	case GDK_KEY_KP_3:
		return UI::GUIControl::GK_NUMPAD3;
	case GDK_KEY_KP_4:
		return UI::GUIControl::GK_NUMPAD4;
	case GDK_KEY_KP_5:
		return UI::GUIControl::GK_NUMPAD5;
	case GDK_KEY_KP_6:
		return UI::GUIControl::GK_NUMPAD6;
	case GDK_KEY_KP_7:
		return UI::GUIControl::GK_NUMPAD7;
	case GDK_KEY_KP_8:
		return UI::GUIControl::GK_NUMPAD8;
	case GDK_KEY_KP_9:
		return UI::GUIControl::GK_NUMPAD9;
	case GDK_KEY_KP_Multiply:
		return UI::GUIControl::GK_MULTIPLY;
	case GDK_KEY_KP_Add:
		return UI::GUIControl::GK_ADD;
	case GDK_KEY_KP_Separator:
		return UI::GUIControl::GK_SEPARATOR;
	case GDK_KEY_KP_Subtract:
		return UI::GUIControl::GK_SUBTRACT;
	case GDK_KEY_KP_Decimal:
		return UI::GUIControl::GK_DECIMAL;
	case GDK_KEY_KP_Divide:
		return UI::GUIControl::GK_DIVIDE;
	case GDK_KEY_F1:
		return UI::GUIControl::GK_F1;
	case GDK_KEY_F2:
		return UI::GUIControl::GK_F2;
	case GDK_KEY_F3:
		return UI::GUIControl::GK_F3;
	case GDK_KEY_F4:
		return UI::GUIControl::GK_F4;
	case GDK_KEY_F5:
		return UI::GUIControl::GK_F5;
	case GDK_KEY_F6:
		return UI::GUIControl::GK_F6;
	case GDK_KEY_F7:
		return UI::GUIControl::GK_F7;
	case GDK_KEY_F8:
		return UI::GUIControl::GK_F8;
	case GDK_KEY_F9:
		return UI::GUIControl::GK_F9;
	case GDK_KEY_F10:
		return UI::GUIControl::GK_F10;
	case GDK_KEY_F11:
		return UI::GUIControl::GK_F11;
	case GDK_KEY_F12:
		return UI::GUIControl::GK_F12;
	case GDK_KEY_F13:
		return UI::GUIControl::GK_F13;
	case GDK_KEY_F14:
		return UI::GUIControl::GK_F14;
	case GDK_KEY_F15:
		return UI::GUIControl::GK_F15;
	case GDK_KEY_F16:
		return UI::GUIControl::GK_F16;
	case GDK_KEY_F17:
		return UI::GUIControl::GK_F17;
	case GDK_KEY_F18:
		return UI::GUIControl::GK_F18;
	case GDK_KEY_F19:
		return UI::GUIControl::GK_F19;
	case GDK_KEY_F20:
		return UI::GUIControl::GK_F20;
	case GDK_KEY_F21:
		return UI::GUIControl::GK_F21;
	case GDK_KEY_F22:
		return UI::GUIControl::GK_F22;
	case GDK_KEY_F23:
		return UI::GUIControl::GK_F23;
	case GDK_KEY_F24:
		return UI::GUIControl::GK_F24;
	case GDK_KEY_Num_Lock:
		return UI::GUIControl::GK_NUMLOCK;
	case GDK_KEY_Scroll_Lock:
		return UI::GUIControl::GK_SCROLLLOCK;
	default:
		return UI::GUIControl::GK_NONE;
	}
}

Text::CStringNN UI::GUIControl::GUIKeyGetName(GUIKey guiKey)
{
	switch (guiKey)
	{
	case UI::GUIControl::GK_BACKSPACE:
		return CSTR("GK_BACKSPACE");
	case UI::GUIControl::GK_TAB:
		return CSTR("GK_TAB");
	case UI::GUIControl::GK_CLEAR:
		return CSTR("GK_CLEAR");
	case UI::GUIControl::GK_ENTER:
		return CSTR("GK_ENTER");
	case UI::GUIControl::GK_SHIFT:
		return CSTR("GK_SHIFT");
	case UI::GUIControl::GK_CONTROL:
		return CSTR("GK_CONTROL");
	case UI::GUIControl::GK_ALT:
		return CSTR("GK_ALT");
	case UI::GUIControl::GK_PAUSE:
		return CSTR("GK_PAUSE");
	case UI::GUIControl::GK_CAPITAL:
		return CSTR("GK_CAPITAL");
	case UI::GUIControl::GK_KANA:
		return CSTR("GK_KANA");
	case UI::GUIControl::GK_JUNJA:
		return CSTR("GK_JUNJA");
	case UI::GUIControl::GK_FINAL:
		return CSTR("GK_FINAL");
	case UI::GUIControl::GK_KANJI:
		return CSTR("GK_KANJI");
	case UI::GUIControl::GK_ESCAPE:
		return CSTR("GK_ESCAPE");
	case UI::GUIControl::GK_CONVERT:
		return CSTR("GK_CONVERT");
#ifndef _WIN32_WCE
	case UI::GUIControl::GK_NONCONVERT:
		return CSTR("GK_NONCONVERT");
	case UI::GUIControl::GK_ACCEPT:
		return CSTR("GK_ACCEPT");
	case UI::GUIControl::GK_MODECHANGE:
		return CSTR("GK_MODECHANGE");
#endif
	case UI::GUIControl::GK_SPACE:
		return CSTR("GK_SPACE");
	case UI::GUIControl::GK_PAGEUP:
		return CSTR("GK_PAGEUP");
	case UI::GUIControl::GK_PAGEDOWN:
		return CSTR("GK_PAGEDOWN");
	case UI::GUIControl::GK_END:
		return CSTR("GK_END");
	case UI::GUIControl::GK_HOME:
		return CSTR("GK_HOME");
	case UI::GUIControl::GK_LEFT:
		return CSTR("GK_LEFT");
	case UI::GUIControl::GK_UP:
		return CSTR("GK_UP");
	case UI::GUIControl::GK_RIGHT:
		return CSTR("GK_RIGHT");
	case UI::GUIControl::GK_DOWN:
		return CSTR("GK_DOWN");
	case UI::GUIControl::GK_SELECT:
		return CSTR("GK_SELECT");
	case UI::GUIControl::GK_PRINT:
		return CSTR("GK_PRINT");
	case UI::GUIControl::GK_EXECUTE:
		return CSTR("GK_EXECUTE");
	case UI::GUIControl::GK_PRINTSCREEN:
		return CSTR("GK_PRINTSCREEN");
	case UI::GUIControl::GK_INSERT:
		return CSTR("GK_INSERT");
	case UI::GUIControl::GK_DELETE:
		return CSTR("GK_DELETE");
	case UI::GUIControl::GK_HELP:
		return CSTR("GK_HELP");
	case UI::GUIControl::GK_0:
		return CSTR("GK_0");
	case UI::GUIControl::GK_1:
		return CSTR("GK_1");
	case UI::GUIControl::GK_2:
		return CSTR("GK_2");
	case UI::GUIControl::GK_3:
		return CSTR("GK_3");
	case UI::GUIControl::GK_4:
		return CSTR("GK_4");
	case UI::GUIControl::GK_5:
		return CSTR("GK_5");
	case UI::GUIControl::GK_6:
		return CSTR("GK_6");
	case UI::GUIControl::GK_7:
		return CSTR("GK_7");
	case UI::GUIControl::GK_8:
		return CSTR("GK_8");
	case UI::GUIControl::GK_9:
		return CSTR("GK_9");
	case UI::GUIControl::GK_A:
		return CSTR("GK_A");
	case UI::GUIControl::GK_B:
		return CSTR("GK_B");
	case UI::GUIControl::GK_C:
		return CSTR("GK_C");
	case UI::GUIControl::GK_D:
		return CSTR("GK_D");
	case UI::GUIControl::GK_E:
		return CSTR("GK_E");
	case UI::GUIControl::GK_F:
		return CSTR("GK_F");
	case UI::GUIControl::GK_G:
		return CSTR("GK_G");
	case UI::GUIControl::GK_H:
		return CSTR("GK_H");
	case UI::GUIControl::GK_I:
		return CSTR("GK_I");
	case UI::GUIControl::GK_J:
		return CSTR("GK_J");
	case UI::GUIControl::GK_K:
		return CSTR("GK_K");
	case UI::GUIControl::GK_L:
		return CSTR("GK_L");
	case UI::GUIControl::GK_M:
		return CSTR("GK_M");
	case UI::GUIControl::GK_N:
		return CSTR("GK_N");
	case UI::GUIControl::GK_O:
		return CSTR("GK_O");
	case UI::GUIControl::GK_P:
		return CSTR("GK_P");
	case UI::GUIControl::GK_Q:
		return CSTR("GK_Q");
	case UI::GUIControl::GK_R:
		return CSTR("GK_R");
	case UI::GUIControl::GK_S:
		return CSTR("GK_S");
	case UI::GUIControl::GK_T:
		return CSTR("GK_T");
	case UI::GUIControl::GK_U:
		return CSTR("GK_U");
	case UI::GUIControl::GK_V:
		return CSTR("GK_V");
	case UI::GUIControl::GK_W:
		return CSTR("GK_W");
	case UI::GUIControl::GK_X:
		return CSTR("GK_X");
	case UI::GUIControl::GK_Y:
		return CSTR("GK_Y");
	case UI::GUIControl::GK_Z:
		return CSTR("GK_Z");
	case UI::GUIControl::GK_LWIN:
		return CSTR("GK_LWIN");
	case UI::GUIControl::GK_RWIN:
		return CSTR("GK_RWIN");
	case UI::GUIControl::GK_APPS:
		return CSTR("GK_APPS");
	case UI::GUIControl::GK_SLEEP:
		return CSTR("GK_SLEEP");
	case UI::GUIControl::GK_NUMPAD0:
		return CSTR("GK_NUMPAD0");
	case UI::GUIControl::GK_NUMPAD1:
		return CSTR("GK_NUMPAD1");
	case UI::GUIControl::GK_NUMPAD2:
		return CSTR("GK_NUMPAD2");
	case UI::GUIControl::GK_NUMPAD3:
		return CSTR("GK_NUMPAD3");
	case UI::GUIControl::GK_NUMPAD4:
		return CSTR("GK_NUMPAD4");
	case UI::GUIControl::GK_NUMPAD5:
		return CSTR("GK_NUMPAD5");
	case UI::GUIControl::GK_NUMPAD6:
		return CSTR("GK_NUMPAD6");
	case UI::GUIControl::GK_NUMPAD7:
		return CSTR("GK_NUMPAD7");
	case UI::GUIControl::GK_NUMPAD8:
		return CSTR("GK_NUMPAD8");
	case UI::GUIControl::GK_NUMPAD9:
		return CSTR("GK_NUMPAD9");
	case UI::GUIControl::GK_MULTIPLY:
		return CSTR("GK_MULTIPLY");
	case UI::GUIControl::GK_ADD:
		return CSTR("GK_ADD");
	case UI::GUIControl::GK_SEPARATOR:
		return CSTR("GK_SEPARATOR");
	case UI::GUIControl::GK_SUBTRACT:
		return CSTR("GK_SUBTRACT");
	case UI::GUIControl::GK_DECIMAL:
		return CSTR("GK_DECIMAL");
	case UI::GUIControl::GK_DIVIDE:
		return CSTR("GK_DIVIDE");
	case UI::GUIControl::GK_F1:
		return CSTR("GK_F1");
	case UI::GUIControl::GK_F2:
		return CSTR("GK_F2");
	case UI::GUIControl::GK_F3:
		return CSTR("GK_F3");
	case UI::GUIControl::GK_F4:
		return CSTR("GK_F4");
	case UI::GUIControl::GK_F5:
		return CSTR("GK_F5");
	case UI::GUIControl::GK_F6:
		return CSTR("GK_F6");
	case UI::GUIControl::GK_F7:
		return CSTR("GK_F7");
	case UI::GUIControl::GK_F8:
		return CSTR("GK_F8");
	case UI::GUIControl::GK_F9:
		return CSTR("GK_F9");
	case UI::GUIControl::GK_F10:
		return CSTR("GK_F10");
	case UI::GUIControl::GK_F11:
		return CSTR("GK_F11");
	case UI::GUIControl::GK_F12:
		return CSTR("GK_F12");
	case UI::GUIControl::GK_F13:
		return CSTR("GK_F13");
	case UI::GUIControl::GK_F14:
		return CSTR("GK_F14");
	case UI::GUIControl::GK_F15:
		return CSTR("GK_F15");
	case UI::GUIControl::GK_F16:
		return CSTR("GK_F16");
	case UI::GUIControl::GK_F17:
		return CSTR("GK_F17");
	case UI::GUIControl::GK_F18:
		return CSTR("GK_F18");
	case UI::GUIControl::GK_F19:
		return CSTR("GK_F19");
	case UI::GUIControl::GK_F20:
		return CSTR("GK_F20");
	case UI::GUIControl::GK_F21:
		return CSTR("GK_F21");
	case UI::GUIControl::GK_F22:
		return CSTR("GK_F22");
	case UI::GUIControl::GK_F23:
		return CSTR("GK_F23");
	case UI::GUIControl::GK_F24:
		return CSTR("GK_F24");
	case UI::GUIControl::GK_NUMLOCK:
		return CSTR("GK_NUMLOCK");
	case UI::GUIControl::GK_SCROLLLOCK:
		return CSTR("GK_SCROLLLOCK");
#ifndef _WIN32_WCE
	case UI::GUIControl::GK_OEM_1:
		return CSTR("GK_OEM_1");
	case UI::GUIControl::GK_OEM_PLUS:
		return CSTR("GK_OEM_PLUS");
	case UI::GUIControl::GK_OEM_COMMA:
		return CSTR("GK_OEM_COMMA");
	case UI::GUIControl::GK_OEM_MINUS:
		return CSTR("GK_OEM_MINUS");
	case UI::GUIControl::GK_OEM_PERIOD:
		return CSTR("GK_OEM_PERIOD");
	case UI::GUIControl::GK_OEM_2:
		return CSTR("GK_OEM_2");
	case UI::GUIControl::GK_OEM_3:
		return CSTR("GK_OEM_3");
	case UI::GUIControl::GK_OEM_4:
		return CSTR("GK_OEM_4");
	case UI::GUIControl::GK_OEM_5:
		return CSTR("GK_OEM_5");
	case UI::GUIControl::GK_OEM_6:
		return CSTR("GK_OEM_6");
	case UI::GUIControl::GK_OEM_7:
		return CSTR("GK_OEM_7");
#endif
	case UI::GUIControl::GK_NONE:
	default:
		return CSTR("GK_NONE");
	}
}

UI::GUIControl::DragErrorType UI::GUIControl::HandleDropEvents(NN<UI::GUIDropHandler> hdlr)
{
	if (this->dropHdlr)
	{
		UI::GTK::GTKDragDrop *dragDrop = (UI::GTK::GTKDragDrop*)this->dropHdlr;
		dragDrop->SetHandler(hdlr);
	}
	else
	{
		UI::GTK::GTKDragDrop *dragDrop;
		NEW_CLASS(dragDrop, UI::GTK::GTKDragDrop(this->hwnd, hdlr));
		this->dropHdlr = dragDrop;
	}
	return UI::GUIControl::DET_NOERROR;
}

void UI::GUIControl::DestroyObject()
{

}
