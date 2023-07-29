#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/GUIButton.h"
#include "UI/GUIClientControl.h"
#include "UI/GUIForm.h"
#include "UI/GUITextBox.h"
#include <gtk/gtk.h>

struct UI::GUITextBox::ClassData
{
	Bool multiLine;
	GtkWidget *widget;
};

void GUITextBox_Changed(GtkTextBuffer *textbuffer, gpointer user_data)
{
	UI::GUITextBox *me = (UI::GUITextBox*)user_data;
	me->EventTextChange();
}

void GUITextBox_DelText(GtkEntryBuffer *buffer, guint position, guint n_chars, gpointer user_data)
{
	UI::GUITextBox *me = (UI::GUITextBox*)user_data;
	me->EventTextChange();
}

void GUITextBox_InsText(GtkEntryBuffer *buffer, guint position, char *chars, guint n_chars, gpointer user_data)
{
	UI::GUITextBox *me = (UI::GUITextBox*)user_data;
	me->EventTextChange();
}

gboolean GUITextBox_KeyDown(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	UI::GUITextBox *me = (UI::GUITextBox*)user_data;
	return me->EventKeyDown(event->key.keyval);
}

void GUITextBox_InitTextBox(UI::GUITextBox::ClassData *txt, Text::CString lbl, Bool multiLine, UI::GUITextBox *me)
{
	if (multiLine)
	{
		txt->multiLine = true;
		txt->widget = gtk_text_view_new();
		GtkTextBuffer *buff = gtk_text_view_get_buffer((GtkTextView*)txt->widget);
		gtk_text_buffer_set_text(buff, (const Char*)lbl.v, (gint)lbl.leng);
		g_signal_connect(buff, "changed", G_CALLBACK(GUITextBox_Changed), me);
		g_signal_connect(txt->widget, "key-press-event", G_CALLBACK(GUITextBox_KeyDown), me);
	}
	else
	{
		txt->multiLine = false;
		txt->widget = gtk_entry_new();
		GtkEntryBuffer *buff = gtk_entry_get_buffer((GtkEntry*)txt->widget);
		gtk_entry_buffer_set_text(buff, (const Char*)lbl.v, (gint)lbl.leng);
		gtk_widget_set_vexpand(txt->widget, false);
		gtk_widget_set_hexpand(txt->widget, false);
		g_signal_connect(buff, "deleted-text", G_CALLBACK(GUITextBox_DelText), me);
		g_signal_connect(buff, "inserted-text", G_CALLBACK(GUITextBox_InsText), me);
		g_signal_connect(txt->widget, "key-press-event", G_CALLBACK(GUITextBox_KeyDown), me);
	}
}

UI::GUITextBox::GUITextBox(NotNullPtr<UI::GUICore> ui, UI::GUIClientControl *parent, Text::CString initText) : UI::GUIControl(ui, parent)
{
	this->clsData = MemAlloc(ClassData, 1);
	GUITextBox_InitTextBox(this->clsData, initText, false, this);
	this->hwnd = (ControlHandle*)this->clsData->widget;
	parent->AddChild(this);
	this->Show();
}

UI::GUITextBox::GUITextBox(NotNullPtr<UI::GUICore> ui, UI::GUIClientControl *parent, Text::CString initText, Bool isMultiline) : UI::GUIControl(ui, parent)
{
	this->clsData = MemAlloc(ClassData, 1);
	GUITextBox_InitTextBox(this->clsData, initText, isMultiline, this);
	if (isMultiline)
	{
		this->hwnd = (ControlHandle*)gtk_scrolled_window_new(0, 0);
		gtk_container_add(GTK_CONTAINER(this->hwnd), this->clsData->widget);
		parent->AddChild(this);
		gtk_widget_show(this->clsData->widget);
	}
	else
	{
		this->hwnd = (ControlHandle*)this->clsData->widget;
		parent->AddChild(this);
	}
	this->Show();
}

UI::GUITextBox::~GUITextBox()
{
	MemFree(this->clsData);
//	gtk_widget_destroy((GtkWidget*)this->hwnd);
}

void UI::GUITextBox::EventTextChange()
{
	UOSInt i = this->txtChgHdlrs.GetCount();
	while (i-- > 0)
	{
		this->txtChgHdlrs.GetItem(i)(this->txtChgObjs.GetItem(i));
	}
}

Bool UI::GUITextBox::EventKeyDown(UInt32 osKey)
{
	Bool ret = false;
	UOSInt i = this->keyDownHdlrs.GetCount();
	while (i-- > 0)
	{
		if ((ret = this->keyDownHdlrs.GetItem(i)(this->keyDownObjs.GetItem(i), osKey)))
		{
			break;
		}
	}
	if (!ret)
	{
		UI::GUIControl::GUIKey key = UI::GUIControl::OSKey2GUIKey(osKey);
		if (key == UI::GUIControl::GK_ESCAPE)
		{
			UI::GUIButton *btn = this->GetRootForm()->GetCancelButton();
			if (btn)
			{
				btn->EventButtonClick();
				ret = true;
			}
		}
		else if (key == UI::GUIControl::GK_ENTER)
		{
			if (!this->clsData->multiLine)
			{
				UI::GUIButton *btn = this->GetRootForm()->GetDefaultButton();
				if (btn)
				{
					btn->EventButtonClick();
					ret = true;
				}
			}
		}
	}
	return ret;
}

void UI::GUITextBox::SetReadOnly(Bool isReadOnly)
{
	if (this->clsData->multiLine)
	{
		gtk_text_view_set_editable((GtkTextView*)this->clsData->widget, !isReadOnly);
	}
	else
	{
		gtk_editable_set_editable((GtkEditable*)this->clsData->widget, !isReadOnly);
	}
}

void UI::GUITextBox::SetPasswordChar(WChar c)
{
	if (this->clsData->multiLine)
	{
	}
	else
	{
		gtk_entry_set_visibility((GtkEntry*)this->clsData->widget, false);
		gtk_entry_set_invisible_char((GtkEntry*)this->clsData->widget, (gunichar)c);
	}
}

void UI::GUITextBox::SetText(Text::CString lbl)
{
	UOSInt lblLeng = lbl.leng;
	if (this->clsData->multiLine)
	{
		GtkTextBuffer *buff = gtk_text_view_get_buffer((GtkTextView*)this->clsData->widget);
		gtk_text_buffer_set_text(buff, (const Char*)lbl.v, (gint)lblLeng);
	}
	else
	{
		GtkEntryBuffer *buff = gtk_entry_get_buffer((GtkEntry*)this->clsData->widget);
		gtk_entry_buffer_set_text(buff, (const Char*)lbl.v, (gint)lblLeng);
	}
}

UTF8Char *UI::GUITextBox::GetText(UTF8Char *buff)
{
	const gchar *lbl;
	if (this->clsData->multiLine)
	{
		GtkTextIter startIter;
		GtkTextIter endIter;
		GtkTextBuffer *txtBuff = gtk_text_view_get_buffer((GtkTextView*)this->clsData->widget);
		gtk_text_buffer_get_start_iter(txtBuff, &startIter);
		gtk_text_buffer_get_end_iter(txtBuff, &endIter);
		lbl = gtk_text_buffer_get_text(txtBuff, &startIter, &endIter, TRUE);
		buff = Text::StrConcat(buff, (const UTF8Char*)lbl);
		g_free((gchar*)lbl);
		return buff;
	}
	else
	{
		GtkEntryBuffer *entBuff = gtk_entry_get_buffer((GtkEntry*)this->clsData->widget);
		lbl = gtk_entry_buffer_get_text(entBuff);
		return Text::StrConcat(buff, (const UTF8Char*)lbl);
	}
}

Bool UI::GUITextBox::GetText(NotNullPtr<Text::StringBuilderUTF8> sb)
{
	const gchar *lbl;
	if (this->clsData->multiLine)
	{
		GtkTextIter startIter;
		GtkTextIter endIter;
		GtkTextBuffer *buff = gtk_text_view_get_buffer((GtkTextView*)this->clsData->widget);
		gtk_text_buffer_get_start_iter(buff, &startIter);
		gtk_text_buffer_get_end_iter(buff, &endIter);
		lbl = gtk_text_buffer_get_text(buff, &startIter, &endIter, TRUE);
		sb->AppendSlow((const UTF8Char*)lbl);
		g_free((gchar*)lbl);
		return true;
	}
	else
	{
		GtkEntryBuffer *buff = gtk_entry_get_buffer((GtkEntry*)this->clsData->widget);
		lbl = gtk_entry_buffer_get_text(buff);
		if (lbl == 0)
		{
			return false;
		}
		sb->AppendSlow((const UTF8Char*)lbl);
		return true;
	}
}

Text::CString UI::GUITextBox::GetObjectClass()
{
	return CSTR("TextBox");
}

OSInt UI::GUITextBox::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GUITextBox::HandleTextChanged(UI::UIEvent hdlr, void *userObj)
{
	this->txtChgHdlrs.Add(hdlr);
	this->txtChgObjs.Add(userObj);
}

void UI::GUITextBox::HandleKeyDown(UI::KeyEvent hdlr, void *userObj)
{
	this->keyDownHdlrs.Add(hdlr);
	this->keyDownObjs.Add(userObj);
}

void UI::GUITextBox::SetWordWrap(Bool wordWrap)
{
	if (this->clsData->multiLine)
	{
		gtk_text_view_set_wrap_mode((GtkTextView*)this->clsData->widget, wordWrap?GTK_WRAP_WORD:GTK_WRAP_NONE);
	}
}

void UI::GUITextBox::SelectAll()
{
	if (this->clsData->multiLine)
	{
		GtkTextIter startIter;
		GtkTextIter endIter;
		GtkTextMark *mark;
		GtkTextBuffer *buff = gtk_text_view_get_buffer((GtkTextView*)this->clsData->widget);
		mark = gtk_text_buffer_get_insert(buff);
		gtk_text_buffer_get_iter_at_mark(buff, &startIter, mark);
		gtk_text_buffer_get_iter_at_mark(buff, &endIter, mark);
		gtk_text_iter_set_offset(&startIter, 0);
		gtk_text_iter_forward_to_end(&endIter);
		gtk_text_buffer_select_range(buff, &startIter, &endIter);
	}
	else
	{
		gtk_editable_select_region((GtkEditable*)this->clsData->widget, 0, -1);
	}
}
