#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/GUITextBox.h"
#include "UI/GUIClientControl.h"
#include <gtk/gtk.h>

typedef struct
{
	Bool multiLine;
	GtkWidget *widget;
} TextBoxData;

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

void GUITextBox_InitTextBox(TextBoxData *txt, const UTF8Char *lbl, Bool multiLine, UI::GUITextBox *me)
{
	UOSInt lblLeng = Text::StrCharCnt(lbl);
	if (multiLine)
	{
		txt->multiLine = true;
		txt->widget = gtk_text_view_new();
		GtkTextBuffer *buff = gtk_text_view_get_buffer((GtkTextView*)txt->widget);
		gtk_text_buffer_set_text(buff, (const Char*)lbl, (gint)lblLeng);
		g_signal_connect(buff, "changed", G_CALLBACK(GUITextBox_Changed), me);
	}
	else
	{
		txt->multiLine = false;
		txt->widget = gtk_entry_new();
		GtkEntryBuffer *buff = gtk_entry_get_buffer((GtkEntry*)txt->widget);
		gtk_entry_buffer_set_text(buff, (const Char*)lbl, (gint)lblLeng);
		gtk_widget_set_vexpand(txt->widget, false);
		gtk_widget_set_hexpand(txt->widget, false);
		g_signal_connect(buff, "deleted-text", G_CALLBACK(GUITextBox_DelText), me);
		g_signal_connect(buff, "inserted-text", G_CALLBACK(GUITextBox_InsText), me);
	}
}

UI::GUITextBox::GUITextBox(UI::GUICore *ui, UI::GUIClientControl *parent, const UTF8Char *initText) : UI::GUIControl(ui, parent)
{
	NEW_CLASS(this->txtChgHdlrs, Data::ArrayList<UI::UIEvent>());
	NEW_CLASS(this->txtChgObjs, Data::ArrayList<void*>());

	TextBoxData *txt = MemAlloc(TextBoxData, 1);
	GUITextBox_InitTextBox(txt, initText, false, this);
	this->clsData = txt;
	this->hwnd = (ControlHandle*)txt->widget;
	parent->AddChild(this);
	this->Show();
}

UI::GUITextBox::GUITextBox(UI::GUICore *ui, UI::GUIClientControl *parent, const UTF8Char *initText, Bool isMultiline) : UI::GUIControl(ui, parent)
{
	NEW_CLASS(this->txtChgHdlrs, Data::ArrayList<UI::UIEvent>());
	NEW_CLASS(this->txtChgObjs, Data::ArrayList<void*>());

	TextBoxData *txt = MemAlloc(TextBoxData, 1);
	GUITextBox_InitTextBox(txt, initText, isMultiline, this);
	this->clsData = txt;
	if (isMultiline)
	{
		this->hwnd = (ControlHandle*)gtk_scrolled_window_new(0, 0);
		gtk_container_add(GTK_CONTAINER(this->hwnd), txt->widget);
		parent->AddChild(this);
		gtk_widget_show(txt->widget);
	}
	else
	{
		this->hwnd = (ControlHandle*)txt->widget;
		parent->AddChild(this);
	}
	this->Show();
}

UI::GUITextBox::~GUITextBox()
{
	TextBoxData *txt = (TextBoxData*)this->clsData;
	MemFree(txt);
	DEL_CLASS(this->txtChgObjs);
	DEL_CLASS(this->txtChgHdlrs);
//	gtk_widget_destroy((GtkWidget*)this->hwnd);
}

void UI::GUITextBox::EventTextChange()
{
	UOSInt i = this->txtChgHdlrs->GetCount();
	while (i-- > 0)
	{
		this->txtChgHdlrs->GetItem(i)(this->txtChgObjs->GetItem(i));
	}
}

void UI::GUITextBox::SetReadOnly(Bool isReadOnly)
{
	TextBoxData *txt = (TextBoxData*)this->clsData;
	if (txt->multiLine)
	{
		gtk_text_view_set_editable((GtkTextView*)txt->widget, !isReadOnly);
	}
	else
	{
		gtk_editable_set_editable((GtkEditable*)txt->widget, !isReadOnly);
	}
}

void UI::GUITextBox::SetPasswordChar(WChar c)
{
	TextBoxData *txt = (TextBoxData*)this->clsData;
	if (txt->multiLine)
	{
	}
	else
	{
		gtk_entry_set_visibility((GtkEntry*)txt->widget, false);
		gtk_entry_set_invisible_char((GtkEntry*)txt->widget, (gunichar)c);
	}
}

void UI::GUITextBox::SetText(const UTF8Char *lbl)
{
	TextBoxData *txt = (TextBoxData*)this->clsData;
	UOSInt lblLeng = Text::StrCharCnt(lbl);
	if (txt->multiLine)
	{
		GtkTextBuffer *buff = gtk_text_view_get_buffer((GtkTextView*)txt->widget);
		gtk_text_buffer_set_text(buff, (const Char*)lbl, (gint)lblLeng);
	}
	else
	{
		GtkEntryBuffer *buff = gtk_entry_get_buffer((GtkEntry*)txt->widget);
		gtk_entry_buffer_set_text(buff, (const Char*)lbl, (gint)lblLeng);
	}
}

UTF8Char *UI::GUITextBox::GetText(UTF8Char *buff)
{
	TextBoxData *txt = (TextBoxData*)this->clsData;
	const gchar *lbl;
	if (txt->multiLine)
	{
		GtkTextIter startIter;
		GtkTextIter endIter;
		GtkTextBuffer *txtBuff = gtk_text_view_get_buffer((GtkTextView*)txt->widget);
		gtk_text_buffer_get_start_iter(txtBuff, &startIter);
		gtk_text_buffer_get_end_iter(txtBuff, &endIter);
		lbl = gtk_text_buffer_get_text(txtBuff, &startIter, &endIter, TRUE);
		buff = Text::StrConcat(buff, (const UTF8Char*)lbl);
		g_free((gchar*)lbl);
		return buff;
	}
	else
	{
		GtkEntryBuffer *entBuff = gtk_entry_get_buffer((GtkEntry*)txt->widget);
		lbl = gtk_entry_buffer_get_text(entBuff);
		return Text::StrConcat(buff, (const UTF8Char*)lbl);
	}
}

Bool UI::GUITextBox::GetText(Text::StringBuilderUTF *sb)
{
	TextBoxData *txt = (TextBoxData*)this->clsData;
	const gchar *lbl;
	if (txt->multiLine)
	{
		GtkTextIter startIter;
		GtkTextIter endIter;
		GtkTextBuffer *buff = gtk_text_view_get_buffer((GtkTextView*)txt->widget);
		gtk_text_buffer_get_start_iter(buff, &startIter);
		gtk_text_buffer_get_end_iter(buff, &endIter);
		lbl = gtk_text_buffer_get_text(buff, &startIter, &endIter, TRUE);
		sb->Append((const UTF8Char*)lbl);
		g_free((gchar*)lbl);
	}
	else
	{
		GtkEntryBuffer *buff = gtk_entry_get_buffer((GtkEntry*)txt->widget);
		lbl = gtk_entry_buffer_get_text(buff);
		sb->Append((const UTF8Char*)lbl);
	}
	return true;
}

const UTF8Char *UI::GUITextBox::GetObjectClass()
{
	return (const UTF8Char*)"TextBox";
}

OSInt UI::GUITextBox::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GUITextBox::HandleTextChanged(UI::UIEvent hdlr, void *userObj)
{
	this->txtChgHdlrs->Add(hdlr);
	this->txtChgObjs->Add(userObj);
}

void UI::GUITextBox::SelectAll()
{
	TextBoxData *txt = (TextBoxData*)this->clsData;
	if (txt->multiLine)
	{
		GtkTextIter startIter;
		GtkTextIter endIter;
		GtkTextBuffer *buff = gtk_text_view_get_buffer((GtkTextView*)txt->widget);
		gtk_text_iter_set_offset(&startIter, 0);
		gtk_text_iter_forward_to_end(&endIter);
		gtk_text_buffer_select_range(buff, &startIter, &endIter);
	}
	else
	{
		gtk_editable_select_region((GtkEditable*)txt->widget, 0, -1);
	}
}
