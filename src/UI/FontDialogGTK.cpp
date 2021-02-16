#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Text/MyString.h"
#include "UI/FontDialog.h"
#include <gtk/gtk.h>

UI::FontDialog::FontDialog()
{
	this->fontName = 0;
	this->fontSizePt = 0;
	this->isBold = false;
	this->isItalic = false;
}

UI::FontDialog::FontDialog(const UTF8Char *fontName, Double fontSizePt, Bool isBold, Bool isItalic)
{
	this->fontName = Text::StrCopyNew(fontName);
	this->fontSizePt = fontSizePt;
	this->isBold = isBold;
	this->isItalic = isItalic;
}

UI::FontDialog::~FontDialog()
{
	if (this->fontName)
	{
		Text::StrDelNew(this->fontName);
	}
}

Bool UI::FontDialog::ShowDialog(void *ownerHandle)
{
	GtkWidget *dlg = gtk_font_chooser_dialog_new("Select Font", (GtkWindow*)ownerHandle);
	if (this->fontName)
	{
		gtk_font_chooser_set_font((GtkFontChooser*)dlg, (const Char*)this->fontName);
	}

	Bool ret = false;
	gint dlgRet = gtk_dialog_run(GTK_DIALOG(dlg));
	if (dlgRet == GTK_RESPONSE_ACCEPT || dlgRet == GTK_RESPONSE_OK)
	{
		PangoFontDescription *fontDesc = gtk_font_chooser_get_font_desc((GtkFontChooser*)dlg);
		if (fontDesc)
		{
			const char *family = pango_font_description_get_family(fontDesc);
			Text::StrDelNew(this->fontName);
			this->fontName = Text::StrCopyNew((const UTF8Char*)family);
			this->isBold = pango_font_description_get_weight(fontDesc) >= PANGO_WEIGHT_BOLD;
			this->isItalic = pango_font_description_get_style(fontDesc) == PANGO_STYLE_ITALIC;
			this->fontSizePt = pango_font_description_get_size(fontDesc) / (Double)PANGO_SCALE;
			ret = true;
		}
	}
	gtk_widget_destroy(dlg);
	return ret;
}

const UTF8Char *UI::FontDialog::GetFontName()
{
	return this->fontName;
}

Double UI::FontDialog::GetFontSizePt()
{
	return this->fontSizePt;
}

Bool UI::FontDialog::IsBold()
{
	return this->isBold;
}

Bool UI::FontDialog::IsItalic()
{
	return this->isItalic;
}
