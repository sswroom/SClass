#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Text/CSSBuilder.h"
#include "UI/GUIProgressBar.h"

#include <gtk/gtk.h>
#define GDK_VERSION_AFTER(major, minor) (GDK_MAJOR_VERSION > major || (GDK_MAJOR_VERSION == major && GDK_MINOR_VERSION >= minor))

typedef struct
{
	GtkProgressBar *bar;
	UInt64 currCnt;
	UInt64 totalCnt;
	UOSInt timerId;
	Bool cntUpdated;
} ClassData;

Int32 GUIProgressBar_OnTick(void *userObj)
{
	ClassData *data = (ClassData*)userObj;
	if (data->cntUpdated)
	{
		Double f;
		data->cntUpdated = false;
		if (data->totalCnt == 0)
		{
			f = 0;
		}
		else
		{
			f = (Double)data->currCnt / (Double)data->totalCnt;
		}
		gtk_progress_bar_set_fraction(data->bar, f);
	}
	return 1;
}

UI::GUIProgressBar::GUIProgressBar(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, UInt64 totalCnt) : UI::GUIControl(ui, parent)
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
	parent->AddChild(this);
	this->Show();
	this->totalCnt = totalCnt;
	ClassData *data = MemAlloc(ClassData, 1);
	data->bar = (GtkProgressBar*)this->hwnd;
	data->currCnt = 0;
	data->totalCnt = totalCnt;
	data->cntUpdated = false;
	data->timerId = g_timeout_add(500, GUIProgressBar_OnTick, data);
	this->clsData = data;
}

UI::GUIProgressBar::~GUIProgressBar()
{
	ClassData *data = (ClassData*)this->clsData;
	g_source_remove((guint)data->timerId);
	MemFree(data);
}

Text::CStringNN UI::GUIProgressBar::GetObjectClass() const
{
	return CSTR("ProgressBar");
}

OSInt UI::GUIProgressBar::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GUIProgressBar::ProgressStart(Text::CString name, UInt64 count)
{
	ClassData *data = (ClassData*)this->clsData;
	data->currCnt = 0;
	data->totalCnt = count;
	this->totalCnt = count;
	data->cntUpdated = true;
}

void UI::GUIProgressBar::ProgressUpdate(UInt64 currCount, UInt64 newTotalCount)
{
	ClassData *data = (ClassData*)this->clsData;
	data->currCnt = currCount;
	data->totalCnt = newTotalCount;
	data->cntUpdated = true;
}

void UI::GUIProgressBar::ProgressEnd()
{
	ClassData *data = (ClassData*)this->clsData;
	data->currCnt = 0;
	data->cntUpdated = true;
}

