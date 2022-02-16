#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "UI/GUIProgressBar.h"

#include <gtk/gtk.h>

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

UI::GUIProgressBar::GUIProgressBar(UI::GUICore *ui, UI::GUIClientControl *parent, UInt64 totalCnt) : UI::GUIControl(ui, parent)
{
	this->hwnd = (ControlHandle*)gtk_progress_bar_new();
	gtk_progress_bar_set_show_text((GtkProgressBar*)this->hwnd, false);
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

Text::CString UI::GUIProgressBar::GetObjectClass()
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

