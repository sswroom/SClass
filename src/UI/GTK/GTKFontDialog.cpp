#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Text/MyString.h"
#include "UI/GTK/GTKFontDialog.h"
#include <gtk/gtk.h>

UI::GTK::GTKFontDialog::GTKFontDialog(Optional<Text::String> fontName, Double fontSizePt, Bool isBold, Bool isItalic) : GUIFontDialog(fontName, fontSizePt, isBold, isItalic)
{
}

UI::GTK::GTKFontDialog::GTKFontDialog(Text::CString fontName, Double fontSizePt, Bool isBold, Bool isItalic) : GUIFontDialog(fontName, fontSizePt, isBold, isItalic)
{
}


UI::GTK::GTKFontDialog::~GTKFontDialog()
{
}

Bool UI::GTK::GTKFontDialog::ShowDialog(ControlHandle *ownerHandle)
{
	GtkWidget *dlg = gtk_font_chooser_dialog_new("Select Font", (GtkWindow*)ownerHandle);
	NotNullPtr<Text::String> s;
	if (this->fontName.SetTo(s))
	{
		gtk_font_chooser_set_font((GtkFontChooser*)dlg, (const Char*)s->v);
	}

	Bool ret = false;
	gint dlgRet = gtk_dialog_run(GTK_DIALOG(dlg));
	if (dlgRet == GTK_RESPONSE_ACCEPT || dlgRet == GTK_RESPONSE_OK)
	{
		PangoFontDescription *fontDesc = gtk_font_chooser_get_font_desc((GtkFontChooser*)dlg);
		if (fontDesc)
		{
			const char *family = pango_font_description_get_family(fontDesc);
			OPTSTR_DEL(this->fontName);
			this->fontName = Text::String::NewNotNullSlow((const UTF8Char*)family);
			this->isBold = pango_font_description_get_weight(fontDesc) >= PANGO_WEIGHT_BOLD;
			this->isItalic = pango_font_description_get_style(fontDesc) == PANGO_STYLE_ITALIC;
			this->fontSizePt = pango_font_description_get_size(fontDesc) / (Double)PANGO_SCALE;
			ret = true;
		}
	}
	gtk_widget_destroy(dlg);
	return ret;
}
