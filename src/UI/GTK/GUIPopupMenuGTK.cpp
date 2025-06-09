#include "Stdafx.h"
#include "MyMemory.h"
#include "UI/GUIPopupMenu.h"

#include <gtk/gtk.h>

UI::GUIPopupMenu::GUIPopupMenu() : UI::GUIMenu(true)
{
}

void UI::GUIPopupMenu::ShowMenu(NN<UI::GUIControl> ctrl, Math::Coord2D<OSInt> scnPos)
{
	NN<UI::GUIForm> frm;
	if (ctrl->GetRootForm().SetTo(frm))
	{
		gint tranX;
		gint tranY;
		if (gtk_widget_translate_coordinates((GtkWidget*)ctrl->GetHandle().OrNull(), (GtkWidget*)frm->GetHandle().OrNull(), (gint)scnPos.x, (gint)scnPos.y, &tranX, &tranY))
		{
			scnPos.x = tranX;
			scnPos.y = tranY;
		}
		this->SetMenuForm(frm);
		ctrl = frm;
	}
	GdkWindow *window = gtk_widget_get_window((GtkWidget*)ctrl->GetHandle().OrNull());

	GdkRectangle rect;
	rect.x = (int)scnPos.x;
	rect.y = (int)scnPos.y;
	rect.width = 0;
	rect.height = 0;
	gtk_menu_popup_at_rect((GtkMenu*)this->hMenu, window, &rect, GDK_GRAVITY_SOUTH_WEST, GDK_GRAVITY_NORTH_WEST, 0);
}
