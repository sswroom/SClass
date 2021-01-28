#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/GUIDateTimePicker.h"
#include <gtk/gtk.h>

typedef struct
{
	GtkWidget *widget;
	const Char *format;
} DateTimePickerData;

UI::GUIDateTimePicker::GUIDateTimePicker(UI::GUICore *ui, UI::GUIClientControl *parent, SelectType st) : UI::GUIControl(ui, parent)
{
	NEW_CLASS(this->dateChangedHdlrs, Data::ArrayList<DateChangedHandler>());
	NEW_CLASS(this->dateChangedObjs, Data::ArrayList<void*>());

	DateTimePickerData *data = MemAlloc(DateTimePickerData, 1);
	data->format = 0;
	data->widget = gtk_entry_new();
	gtk_widget_set_vexpand(data->widget, false);
	gtk_widget_set_hexpand(data->widget, false);
	this->clsData = data;
	this->hwnd = data->widget;
	parent->AddChild(this);
	this->Show();
	this->SetFormat("yyyy-MM-dd HH:mm:ss");
}

UI::GUIDateTimePicker::~GUIDateTimePicker()
{
	DateTimePickerData *data = (DateTimePickerData*)this->clsData;
	SDEL_TEXT(data->format);
	MemFree(data);
	DEL_CLASS(this->dateChangedHdlrs);
	DEL_CLASS(this->dateChangedObjs);
}


const UTF8Char *UI::GUIDateTimePicker::GetObjectClass()
{
	return (const UTF8Char*)"DateTimePicker";
}

OSInt UI::GUIDateTimePicker::OnNotify(Int32 code, void *lParam)
{
	return 0;
}

void UI::GUIDateTimePicker::SetValue(Data::DateTime *dt)
{
	DateTimePickerData *data = (DateTimePickerData*)this->clsData;
	if (data->format)
	{
		UTF8Char sbuff[64];
		OSInt leng;
		leng = dt->ToString(sbuff, data->format) - sbuff;
		GtkEntryBuffer *buff = gtk_entry_get_buffer((GtkEntry*)data->widget);
		gtk_entry_buffer_set_text(buff, (const Char*)sbuff, leng);
	}
}

void UI::GUIDateTimePicker::GetSelectedTime(Data::DateTime *dt)
{
	DateTimePickerData *data = (DateTimePickerData*)this->clsData;
	GtkEntryBuffer *buff = gtk_entry_get_buffer((GtkEntry*)data->widget);
	dt->SetValue(gtk_entry_buffer_get_text(buff));
}

void UI::GUIDateTimePicker::SetFormat(const Char *format)
{
	DateTimePickerData *data = (DateTimePickerData*)this->clsData;
	SDEL_TEXT(data->format);
	data->format = Text::StrCopyNew(format);
	Data::DateTime dt;
	dt.SetCurrTime();
	this->SetValue(&dt);
}

void UI::GUIDateTimePicker::SetCalShowWeeknum(Bool showWeeknum)
{
	this->showWeeknum = showWeeknum;
}

void UI::GUIDateTimePicker::HandleDateChange(DateChangedHandler hdlr, void *obj)
{
	this->dateChangedHdlrs->Add(hdlr);
	this->dateChangedObjs->Add(obj);
}
