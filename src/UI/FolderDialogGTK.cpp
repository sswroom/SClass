#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/FolderDialog.h"
#include <gtk/gtk.h>

#define MAXFILENAMESIZE 512

Int32 __stdcall UI::FolderDialog::BrowseCB(void *hwnd, UInt32 uMsg, OSInt lParam, OSInt lpData)
{
	return 0;
}

UI::FolderDialog::FolderDialog(const WChar *compName, const WChar *appName, const WChar *dialogName)
{
	this->reg = 0;
	this->dirName = 0;
	this->message = 0;
}

UI::FolderDialog::~FolderDialog()
{
	SDEL_STRING(this->dirName);
	SDEL_TEXT(this->message);
}

void UI::FolderDialog::SetFolder(Text::CString dirName)
{
	SDEL_STRING(this->dirName);
	this->dirName = Text::String::New(dirName);
}

Text::String *UI::FolderDialog::GetFolder()
{
	return this->dirName;
}

void UI::FolderDialog::SetMessage(const UTF8Char *message)
{
	SDEL_TEXT(this->message);
	this->message = Text::StrCopyNew(message);
}

Bool UI::FolderDialog::ShowDialog(ControlHandle *ownerHandle)
{
	GtkWidget *dialog;
	GtkFileChooser *chooser;
	dialog = gtk_file_chooser_dialog_new("Select Folder", 0, GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,  "Cancel", GTK_RESPONSE_CANCEL, "Select", GTK_RESPONSE_ACCEPT, (void*)0);
	chooser = GTK_FILE_CHOOSER(dialog);
	if (this->dirName)
	{
		gtk_file_chooser_set_filename(chooser, (const Char*)this->dirName);
	}
	Bool ret = false;
	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
	{
		ret = true;
		char *csptr = gtk_file_chooser_get_filename(chooser);
		SDEL_STRING(this->dirName);
		this->dirName = Text::String::NewNotNullSlow((const UTF8Char*)csptr);
		g_free(csptr);
	}
	gtk_widget_destroy(dialog);
	return ret;
}
