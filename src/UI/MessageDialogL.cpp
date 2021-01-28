#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/MessageDialog.h"
#include <gtk/gtk.h>

void UI::MessageDialog::ShowDialog(const UTF8Char *message, const UTF8Char *title, UI::GUIControl *ctrl)
{
	GtkWidget *dlg;
	GtkWindow *wnd = 0;
	if (ctrl)
		wnd = (GtkWindow*)ctrl->GetHandle();
	dlg = gtk_message_dialog_new(wnd, GTK_DIALOG_MODAL, GTK_MESSAGE_OTHER, GTK_BUTTONS_OK, "%s", message);
	gtk_window_set_title((GtkWindow*)dlg, (const Char*)title);
	gtk_dialog_run((GtkDialog*)dlg);
	gtk_widget_destroy(dlg);
}

Bool UI::MessageDialog::ShowYesNoDialog(const UTF8Char *message, const UTF8Char *title, UI::GUIControl *ctrl)
{
	GtkWidget *dlg;
	GtkWindow *wnd = 0;
	if (ctrl)
		wnd = (GtkWindow*)ctrl->GetHandle();
	dlg = gtk_message_dialog_new(wnd, GTK_DIALOG_MODAL, GTK_MESSAGE_OTHER, GTK_BUTTONS_YES_NO, "%s", message);
	gtk_window_set_title((GtkWindow*)dlg, (const Char*)title);
	Int32 result = gtk_dialog_run((GtkDialog*)dlg);
	gtk_widget_destroy(dlg);
	return result == GTK_RESPONSE_YES;
}
