#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/GUIButton.h"
#include "UI/GUIClientControl.h"
#include "UI/GUIForm.h"
#include "UI/GTK/GTKTextBox.h"

void UI::GTK::GTKTextBox::SignalChanged(GtkTextBuffer *textbuffer, gpointer user_data)
{
	UI::GTK::GTKTextBox *me = (UI::GTK::GTKTextBox*)user_data;
	me->EventTextChange();
}

void UI::GTK::GTKTextBox::SignalDelText(GtkEntryBuffer *buffer, guint position, guint n_chars, gpointer user_data)
{
	UI::GTK::GTKTextBox *me = (UI::GTK::GTKTextBox*)user_data;
	me->EventTextChange();
}

void UI::GTK::GTKTextBox::SignalInsText(GtkEntryBuffer *buffer, guint position, char *chars, guint n_chars, gpointer user_data)
{
	UI::GTK::GTKTextBox *me = (UI::GTK::GTKTextBox*)user_data;
	me->EventTextChange();
}

gboolean UI::GTK::GTKTextBox::SignalKeyDown(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	UI::GTK::GTKTextBox *me = (UI::GTK::GTKTextBox*)user_data;
	return me->EventKeyDown(event->key.keyval);
}

void UI::GTK::GTKTextBox::InitTextBox(Text::CStringNN lbl, Bool multiLine)
{
	if (multiLine)
	{
		this->multiLine = true;
		this->widget = gtk_text_view_new();
		GtkTextBuffer *buff = gtk_text_view_get_buffer((GtkTextView*)this->widget);
		gtk_text_buffer_set_text(buff, (const Char*)lbl.v, (gint)lbl.leng);
		g_signal_connect(buff, "changed", G_CALLBACK(SignalChanged), this);
		g_signal_connect(this->widget, "key-press-event", G_CALLBACK(SignalKeyDown), this);
	}
	else
	{
		this->multiLine = false;
		this->widget = gtk_entry_new();
		GtkEntryBuffer *buff = gtk_entry_get_buffer((GtkEntry*)this->widget);
		gtk_entry_buffer_set_text(buff, (const Char*)lbl.v, (gint)lbl.leng);
		gtk_widget_set_vexpand(this->widget, false);
		gtk_widget_set_hexpand(this->widget, false);
		g_signal_connect(buff, "deleted-text", G_CALLBACK(SignalDelText), this);
		g_signal_connect(buff, "inserted-text", G_CALLBACK(SignalInsText), this);
		g_signal_connect(this->widget, "key-press-event", G_CALLBACK(SignalKeyDown), this);
	}
}

UI::GTK::GTKTextBox::GTKTextBox(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Text::CStringNN initText) : UI::GUITextBox(ui, parent)
{
	this->InitTextBox(initText, false);
	this->hwnd = (ControlHandle*)this->widget;
	parent->AddChild(*this);
	this->Show();
}

UI::GTK::GTKTextBox::GTKTextBox(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Text::CStringNN initText, Bool isMultiline) : UI::GUITextBox(ui, parent)
{
	this->InitTextBox(initText, isMultiline);
	if (isMultiline)
	{
		this->hwnd = (ControlHandle*)gtk_scrolled_window_new(0, 0);
		gtk_container_add(GTK_CONTAINER(this->hwnd), this->widget);
		parent->AddChild(*this);
		gtk_widget_show(this->widget);
	}
	else
	{
		this->hwnd = (ControlHandle*)this->widget;
		parent->AddChild(*this);
	}
	this->Show();
}

UI::GTK::GTKTextBox::~GTKTextBox()
{
//	gtk_widget_destroy((GtkWidget*)this->hwnd);
}

Bool UI::GTK::GTKTextBox::EventKeyDown(UInt32 osKey)
{
	Bool ret = this->GUITextBox::EventKeyDown(osKey);
	if (!ret)
	{
		UI::GUIControl::GUIKey key = UI::GUIControl::OSKey2GUIKey(osKey);
		if (key == UI::GUIControl::GK_ESCAPE)
		{
			NotNullPtr<UI::GUIButton> btn;
			if (this->GetRootForm()->GetCancelButton().SetTo(btn))
			{
				btn->EventButtonClick();
				ret = true;
			}
		}
		else if (key == UI::GUIControl::GK_ENTER)
		{
			if (!this->multiLine)
			{
				NotNullPtr<UI::GUIButton> btn;
				if (this->GetRootForm()->GetDefaultButton().SetTo(btn))
				{
					btn->EventButtonClick();
					ret = true;
				}
			}
		}
	}
	return ret;
}

void UI::GTK::GTKTextBox::SetReadOnly(Bool isReadOnly)
{
	if (this->multiLine)
	{
		gtk_text_view_set_editable((GtkTextView*)this->widget, !isReadOnly);
	}
	else
	{
		gtk_editable_set_editable((GtkEditable*)this->widget, !isReadOnly);
	}
}

void UI::GTK::GTKTextBox::SetPasswordChar(UTF32Char c)
{
	if (this->multiLine)
	{
	}
	else
	{
		gtk_entry_set_visibility((GtkEntry*)this->widget, false);
		gtk_entry_set_invisible_char((GtkEntry*)this->widget, (gunichar)c);
	}
}

void UI::GTK::GTKTextBox::SetText(Text::CStringNN lbl)
{
	UOSInt lblLeng = lbl.leng;
	if (this->multiLine)
	{
		GtkTextBuffer *buff = gtk_text_view_get_buffer((GtkTextView*)this->widget);
		gtk_text_buffer_set_text(buff, (const Char*)lbl.v, (gint)lblLeng);
	}
	else
	{
		GtkEntryBuffer *buff = gtk_entry_get_buffer((GtkEntry*)this->widget);
		gtk_entry_buffer_set_text(buff, (const Char*)lbl.v, (gint)lblLeng);
	}
}

UTF8Char *UI::GTK::GTKTextBox::GetText(UTF8Char *buff)
{
	const gchar *lbl;
	if (this->multiLine)
	{
		GtkTextIter startIter;
		GtkTextIter endIter;
		GtkTextBuffer *txtBuff = gtk_text_view_get_buffer((GtkTextView*)this->widget);
		gtk_text_buffer_get_start_iter(txtBuff, &startIter);
		gtk_text_buffer_get_end_iter(txtBuff, &endIter);
		lbl = gtk_text_buffer_get_text(txtBuff, &startIter, &endIter, TRUE);
		buff = Text::StrConcat(buff, (const UTF8Char*)lbl);
		g_free((gchar*)lbl);
		return buff;
	}
	else
	{
		GtkEntryBuffer *entBuff = gtk_entry_get_buffer((GtkEntry*)this->widget);
		lbl = gtk_entry_buffer_get_text(entBuff);
		return Text::StrConcat(buff, (const UTF8Char*)lbl);
	}
}

Bool UI::GTK::GTKTextBox::GetText(NotNullPtr<Text::StringBuilderUTF8> sb)
{
	const gchar *lbl;
	if (this->multiLine)
	{
		GtkTextIter startIter;
		GtkTextIter endIter;
		GtkTextBuffer *buff = gtk_text_view_get_buffer((GtkTextView*)this->widget);
		gtk_text_buffer_get_start_iter(buff, &startIter);
		gtk_text_buffer_get_end_iter(buff, &endIter);
		lbl = gtk_text_buffer_get_text(buff, &startIter, &endIter, TRUE);
		sb->AppendSlow((const UTF8Char*)lbl);
		g_free((gchar*)lbl);
		return true;
	}
	else
	{
		GtkEntryBuffer *buff = gtk_entry_get_buffer((GtkEntry*)this->widget);
		lbl = gtk_entry_buffer_get_text(buff);
		if (lbl == 0)
		{
			return false;
		}
		sb->AppendSlow((const UTF8Char*)lbl);
		return true;
	}
}

OSInt UI::GTK::GTKTextBox::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GTK::GTKTextBox::SetWordWrap(Bool wordWrap)
{
	if (this->multiLine)
	{
		gtk_text_view_set_wrap_mode((GtkTextView*)this->widget, wordWrap?GTK_WRAP_WORD:GTK_WRAP_NONE);
	}
}

void UI::GTK::GTKTextBox::SelectAll()
{
	if (this->multiLine)
	{
		GtkTextIter startIter;
		GtkTextIter endIter;
		GtkTextMark *mark;
		GtkTextBuffer *buff = gtk_text_view_get_buffer((GtkTextView*)this->widget);
		mark = gtk_text_buffer_get_insert(buff);
		gtk_text_buffer_get_iter_at_mark(buff, &startIter, mark);
		gtk_text_buffer_get_iter_at_mark(buff, &endIter, mark);
		gtk_text_iter_set_offset(&startIter, 0);
		gtk_text_iter_forward_to_end(&endIter);
		gtk_text_buffer_select_range(buff, &startIter, &endIter);
	}
	else
	{
		gtk_editable_select_region((GtkEditable*)this->widget, 0, -1);
	}
}
