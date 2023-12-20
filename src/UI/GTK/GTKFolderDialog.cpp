#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/GTK/GTKFolderDialog.h"
#include <gtk/gtk.h>

#define MAXFILENAMESIZE 512

UI::GTK::GTKFolderDialog::GTKFolderDialog()
{
}

UI::GTK::GTKFolderDialog::~GTKFolderDialog()
{
}

Bool UI::GTK::GTKFolderDialog::ShowDialog(ControlHandle *ownerHandle)
{
	GtkWidget *dialog;
	GtkFileChooser *chooser;
	NotNullPtr<Text::String> s;
	const Char *msg = "Select Folder";
	if (this->message.SetTo(s))
	{
		msg = (const Char*)s->v;
	}
	dialog = gtk_file_chooser_dialog_new(msg, 0, GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,  "Cancel", GTK_RESPONSE_CANCEL, "Select", GTK_RESPONSE_ACCEPT, (void*)0);
	chooser = GTK_FILE_CHOOSER(dialog);
	if (this->dirName.SetTo(s))
	{
		gtk_file_chooser_set_filename(chooser, (const Char*)s->v);
	}
	Bool ret = false;
	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
	{
		ret = true;
		char *csptr = gtk_file_chooser_get_filename(chooser);
		OPTSTR_DEL(this->dirName);
		this->dirName = Text::String::NewNotNullSlow((const UTF8Char*)csptr);
		g_free(csptr);
	}
	gtk_widget_destroy(dialog);
	return ret;
}
