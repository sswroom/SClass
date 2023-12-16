#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/GTK/GTKMessageDialog.h"
#include <gtk/gtk.h>

void UI::GTK::GTKMessageDialog::ShowOK(Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl)
{
	GtkWidget *dlg;
	GtkWindow *wnd = 0;
	NotNullPtr<GUIControl> nnctrl;
	if (ctrl.SetTo(nnctrl))
		wnd = (GtkWindow*)nnctrl->GetHandle();
	dlg = gtk_message_dialog_new(wnd, GTK_DIALOG_MODAL, GTK_MESSAGE_OTHER, GTK_BUTTONS_OK, "%s", message.v);
	gtk_window_set_title((GtkWindow*)dlg, (const Char*)title.v);
	gtk_dialog_run((GtkDialog*)dlg);
	gtk_widget_destroy(dlg);
}

Bool UI::GTK::GTKMessageDialog::ShowYesNo(Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl)
{
	GtkWidget *dlg;
	GtkWindow *wnd = 0;
	NotNullPtr<GUIControl> nnctrl;
	if (ctrl.SetTo(nnctrl))
		wnd = (GtkWindow*)nnctrl->GetHandle();
	dlg = gtk_message_dialog_new(wnd, GTK_DIALOG_MODAL, GTK_MESSAGE_OTHER, GTK_BUTTONS_YES_NO, "%s", message.v);
	gtk_window_set_title((GtkWindow*)dlg, (const Char*)title.v);
	Int32 result = gtk_dialog_run((GtkDialog*)dlg);
	gtk_widget_destroy(dlg);
	return result == GTK_RESPONSE_YES;
}
