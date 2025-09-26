#include "Stdafx.h"
#include "MyMemory.h"
#include "UI/GUIPopupMenu.h"

#include <gtk/gtk.h>

UI::GUIPopupMenu::GUIPopupMenu() : UI::GUIMenu(true)
{
	this->scnPos = Math::Coord2D<OSInt>(0, 0);
}

void __stdcall GUIPopupMenu_Position(GtkMenu *menu, gint *x, gint *y, gboolean *push_in, gpointer user_data)
{
	UI::GUIPopupMenu *me = (UI::GUIPopupMenu*)user_data;
	Math::Coord2D<OSInt> scnPos = me->GetScnPos();
	*x = (gint)scnPos.x;
	*y = (gint)scnPos.y;
	*push_in = true;
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

#if GTK_CHECK_VERSION(3, 22, 0)
	GdkRectangle rect;
	rect.x = (int)scnPos.x;
	rect.y = (int)scnPos.y;
	rect.width = 0;
	rect.height = 0;
	gtk_menu_popup_at_rect((GtkMenu*)this->hMenu, window, &rect, GDK_GRAVITY_SOUTH_WEST, GDK_GRAVITY_NORTH_WEST, 0);
#else
	this->scnPos = scnPos;
	gtk_menu_popup((GtkMenu*)this->hMenu, 0, 0, GUIPopupMenu_Position, this, GDK_LEFTBUTTON, gtk_get_current_event_time());
#endif
}
