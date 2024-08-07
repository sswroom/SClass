#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/GTK/GTKDateTimePicker.h"

UI::GTK::GTKDateTimePicker::GTKDateTimePicker(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent) : UI::GUIDateTimePicker(ui, parent)
{
	this->format = 0;
	this->widget = gtk_entry_new();
	gtk_widget_set_vexpand(this->widget, false);
	gtk_widget_set_hexpand(this->widget, false);
	this->hwnd = (ControlHandle*)this->widget;
	parent->AddChild(*this);
	this->Show();
	this->SetFormat("yyyy-MM-dd HH:mm:ss");
}

UI::GTK::GTKDateTimePicker::~GTKDateTimePicker()
{
	SDEL_TEXTC(this->format);
}

OSInt UI::GTK::GTKDateTimePicker::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GTK::GTKDateTimePicker::SetValue(NN<Data::DateTime> dt)
{
	UnsafeArray<const Char> nnformat;
	if (this->format.SetTo(nnformat))
	{
		UTF8Char sbuff[64];
		OSInt leng;
		leng = dt->ToString(sbuff, nnformat.Ptr()) - sbuff;
		GtkEntryBuffer *buff = gtk_entry_get_buffer((GtkEntry*)this->widget);
		gtk_entry_buffer_set_text(buff, (const Char*)sbuff, (gint)leng);
	}
}

void UI::GTK::GTKDateTimePicker::SetValue(const Data::Timestamp &ts)
{
	UnsafeArray<const Char> nnformat;
	if (this->format.SetTo(nnformat))
	{
		UTF8Char sbuff[64];
		OSInt leng;
		leng = ts.ToString(sbuff, nnformat.Ptr()) - sbuff;
		GtkEntryBuffer *buff = gtk_entry_get_buffer((GtkEntry*)this->widget);
		gtk_entry_buffer_set_text(buff, (const Char*)sbuff, (gint)leng);
	}
}

void UI::GTK::GTKDateTimePicker::GetSelectedTime(NN<Data::DateTime> dt)
{
	GtkEntryBuffer *buff = gtk_entry_get_buffer((GtkEntry*)this->widget);
	const UTF8Char *s = (const UTF8Char*)gtk_entry_buffer_get_text(buff);
	dt->SetValue(Text::CStringNN::FromPtr(s));
}

Data::Timestamp UI::GTK::GTKDateTimePicker::GetSelectedTime()
{
	GtkEntryBuffer *buff = gtk_entry_get_buffer((GtkEntry*)this->widget);
	const UTF8Char *s = (const UTF8Char*)gtk_entry_buffer_get_text(buff);
	return Data::Timestamp::FromStr(Text::CStringNN::FromPtr(s), Data::DateTimeUtil::GetLocalTzQhr());
}

void UI::GTK::GTKDateTimePicker::SetFormat(const Char *format)
{
	SDEL_TEXTC(this->format);
	this->format = Text::StrCopyNewCh(format);
	Data::DateTime dt;
	dt.SetCurrTime();
	this->SetValue(dt);
}

void UI::GTK::GTKDateTimePicker::SetCalShowWeeknum(Bool showWeeknum)
{
	this->showWeeknum = showWeeknum;
}
