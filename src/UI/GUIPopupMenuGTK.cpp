#include "Stdafx.h"
#include "MyMemory.h"
#include "UI/GUIPopupMenu.h"

#include <gtk/gtk.h>

UI::GUIPopupMenu::GUIPopupMenu() : UI::GUIMenu(true)
{
}

void UI::GUIPopupMenu::ShowMenu(UI::GUIControl *ctrl, OSInt x, OSInt y)
{
	UI::GUIForm *frm = ctrl->GetRootForm();
	if (frm)
	{
		gint tranX;
		gint tranY;
		if (gtk_widget_translate_coordinates((GtkWidget*)ctrl->GetHandle(), (GtkWidget*)frm->GetHandle(), x, y, &tranX, &tranY))
		{
			x = tranX;
			y = tranY;
		}
		this->SetMenuForm(frm);
		ctrl = frm;
	}
	GdkWindow *window = gtk_widget_get_window((GtkWidget*)ctrl->GetHandle());

	GdkRectangle rect;
	rect.x = x;
	rect.y = y;
	rect.width = 0;
	rect.height = 0;
	gtk_menu_popup_at_rect((GtkMenu*)this->hMenu, window, &rect, GDK_GRAVITY_SOUTH_WEST, GDK_GRAVITY_NORTH_WEST, 0);
}
