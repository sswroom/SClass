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

UI::GUIDateTimePicker::GUIDateTimePicker(NotNullPtr<UI::GUICore> ui, UI::GUIClientControl *parent, SelectType st) : UI::GUIControl(ui, parent)
{
	DateTimePickerData *data = MemAlloc(DateTimePickerData, 1);
	data->format = 0;
	data->widget = gtk_entry_new();
	gtk_widget_set_vexpand(data->widget, false);
	gtk_widget_set_hexpand(data->widget, false);
	this->clsData = data;
	this->hwnd = (ControlHandle*)data->widget;
	parent->AddChild(this);
	this->Show();
	this->SetFormat("yyyy-MM-dd HH:mm:ss");
}

UI::GUIDateTimePicker::~GUIDateTimePicker()
{
	DateTimePickerData *data = (DateTimePickerData*)this->clsData;
	SDEL_TEXT(data->format);
	MemFree(data);
}


Text::CStringNN UI::GUIDateTimePicker::GetObjectClass() const
{
	return CSTR("DateTimePicker");
}

OSInt UI::GUIDateTimePicker::OnNotify(UInt32 code, void *lParam)
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
		gtk_entry_buffer_set_text(buff, (const Char*)sbuff, (gint)leng);
	}
}

void UI::GUIDateTimePicker::SetValue(const Data::Timestamp &ts)
{
	DateTimePickerData *data = (DateTimePickerData*)this->clsData;
	if (data->format)
	{
		UTF8Char sbuff[64];
		OSInt leng;
		leng = ts.ToString(sbuff, data->format) - sbuff;
		GtkEntryBuffer *buff = gtk_entry_get_buffer((GtkEntry*)data->widget);
		gtk_entry_buffer_set_text(buff, (const Char*)sbuff, (gint)leng);
	}
}

void UI::GUIDateTimePicker::GetSelectedTime(Data::DateTime *dt)
{
	DateTimePickerData *data = (DateTimePickerData*)this->clsData;
	GtkEntryBuffer *buff = gtk_entry_get_buffer((GtkEntry*)data->widget);
	const UTF8Char *s = (const UTF8Char*)gtk_entry_buffer_get_text(buff);
	dt->SetValue(Text::CString::FromPtr(s));
}

Data::Timestamp UI::GUIDateTimePicker::GetSelectedTime()
{
	DateTimePickerData *data = (DateTimePickerData*)this->clsData;
	GtkEntryBuffer *buff = gtk_entry_get_buffer((GtkEntry*)data->widget);
	const UTF8Char *s = (const UTF8Char*)gtk_entry_buffer_get_text(buff);
	return Data::Timestamp::FromStr(Text::CString::FromPtr(s), Data::DateTimeUtil::GetLocalTzQhr());
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
	this->dateChangedHdlrs.Add(hdlr);
	this->dateChangedObjs.Add(obj);
}
