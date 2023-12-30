#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Text/CSSBuilder.h"
#include "UI/GTK/GTKProgressBar.h"

#define GDK_VERSION_AFTER(major, minor) (GDK_MAJOR_VERSION > major || (GDK_MAJOR_VERSION == major && GDK_MINOR_VERSION >= minor))

Int32 UI::GTK::GTKProgressBar::SignalTick(void *userObj)
{
	UI::GTK::GTKProgressBar *me = (UI::GTK::GTKProgressBar*)userObj;
	if (me->cntUpdated)
	{
		Double f;
		me->cntUpdated = false;
		if (me->totalCnt == 0)
		{
			f = 0;
		}
		else
		{
			f = (Double)me->currCnt / (Double)me->totalCnt;
		}
		gtk_progress_bar_set_fraction(me->bar, f);
	}
	return 1;
}

UI::GTK::GTKProgressBar::GTKProgressBar(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, UInt64 totalCnt) : UI::GUIProgressBar(ui, parent)
{
	this->hwnd = (ControlHandle*)gtk_progress_bar_new();
	gtk_progress_bar_set_show_text((GtkProgressBar*)this->hwnd, false);
#if GDK_VERSION_AFTER(3, 16)
	Text::CSSBuilder builder(Text::CSSBuilder::PM_SPACE);
	builder.NewStyle(CSTR("progress, trough"), CSTR_NULL);
	builder.AddMinHeight(24, Math::Unit::Distance::DU_PIXEL);
	GtkWidget *widget = (GtkWidget*)this->hwnd;
	GtkStyleContext *style = gtk_widget_get_style_context(widget);
	GtkCssProvider *styleProvider = gtk_css_provider_new();
	gtk_css_provider_load_from_data(styleProvider, (const gchar*)builder.ToString(), -1, 0);
	gtk_style_context_add_provider(style, (GtkStyleProvider*)styleProvider, GTK_STYLE_PROVIDER_PRIORITY_USER);
	gtk_widget_reset_style(widget);
#endif
	parent->AddChild(*this);
	this->Show();
	this->totalCnt = totalCnt;
	this->bar = (GtkProgressBar*)this->hwnd;
	this->currCnt = 0;
	this->cntUpdated = false;
	this->timerId = g_timeout_add(500, SignalTick, this);
}

UI::GTK::GTKProgressBar::~GTKProgressBar()
{
	g_source_remove((guint)this->timerId);
}

OSInt UI::GTK::GTKProgressBar::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GTK::GTKProgressBar::ProgressStart(Text::CString name, UInt64 count)
{
	this->currCnt = 0;
	this->totalCnt = count;
	this->cntUpdated = true;
}

void UI::GTK::GTKProgressBar::ProgressUpdate(UInt64 currCount, UInt64 newTotalCount)
{
	this->currCnt = currCount;
	this->totalCnt = newTotalCount;
	this->cntUpdated = true;
}

void UI::GTK::GTKProgressBar::ProgressEnd()
{
	this->currCnt = 0;
	this->cntUpdated = true;
}

