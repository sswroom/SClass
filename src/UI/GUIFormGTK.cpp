#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/TextEnc/URIEncoding.h"
#include "UI/GUIForm.h"
#include "UI/GUITimer.h"
#include <gtk/gtk.h>

typedef struct
{
	GtkWidget *container;
	GtkWidget *scrolledWin;
	GtkWidget *vbox;
} ClientControlData;

gboolean GUIForm_Draw(GtkWidget *widget, cairo_t *cr, gpointer data)
{
	UI::GUIForm *me = (UI::GUIForm *)data;
	return me->OnPaint();
}

void GUIForm_OnFileDrop(GtkWidget *widget, GdkDragContext *context, gint x, gint y, GtkSelectionData *data, guint info, guint time, gpointer userData)
{
	UI::GUIForm *me = (UI::GUIForm *)userData;
	Text::StringBuilderUTF8 sb;
	Data::ArrayList<const UTF8Char *> files;
	sb.Append((const UTF8Char*)gtk_selection_data_get_data(data));
	UTF8Char *sarr[2];
	UTF8Char sbuff[512];
	UOSInt i;
	UOSInt j;
	sarr[1] = sb.ToString();
	while (true)
	{
		i = Text::StrSplit(sarr, 2, sarr[1], '\n');
		if (Text::StrStartsWith(sarr[0], (const UTF8Char*)"file://"))
		{
			j = Text::StrCharCnt(sarr[0]);
			if (sarr[0][j - 1] == '\r')
				sarr[0][j - 1] = 0;
			Text::TextEnc::URIEncoding::URIDecode(sbuff, &sarr[0][7]);
			files.Add(Text::StrCopyNew(sbuff));
		}
		if (i <= 1)
			break;
	}
	i = files.GetCount();
	if (i > 0)
	{
		me->OnFileDrop(files.GetArray(&j), i);
		while (i-- > 0)
		{
			Text::StrDelNew(files.GetItem(i));
		}
	}
}

void GUIForm_EventDestroy(void *window, void *userObj)
{
	UI::GUIForm *me = (UI::GUIForm*)userObj;
	me->EventClosed();
}

void UI::GUIForm::UpdateHAcc()
{
	GtkAccelGroup *accel_group;
	if (this->hAcc == 0)
	{
		accel_group = gtk_accel_group_new();
		this->hAcc = accel_group;
		gtk_window_add_accel_group(GTK_WINDOW((GtkWidget*)this->hwnd), accel_group);
	}
	else
	{
		accel_group = (GtkAccelGroup*)this->hAcc;
	}
	if (this->menu)
	{
		UOSInt i;
		Data::ArrayList<UI::GUIMenu::ShortcutKey*> keys;
		UI::GUIMenu::ShortcutKey *key;
		this->menu->GetAllKeys(&keys);
		i = keys.GetCount();
		while (i-- > 0)
		{
			key = keys.GetItem(i);
			GdkModifierType mods = (GdkModifierType)0;
			if (key->keyModifier & UI::GUIMenu::KM_ALT)
				mods = (GdkModifierType)(mods | GDK_MOD1_MASK);
			if (key->keyModifier & UI::GUIMenu::KM_CONTROL)
				mods = (GdkModifierType)(mods | GDK_CONTROL_MASK);
			if (key->keyModifier & UI::GUIMenu::KM_SHIFT)
				mods = (GdkModifierType)(mods | GDK_SHIFT_MASK);
			gtk_widget_add_accelerator((GtkWidget*)key->menuItem, "activate", accel_group, UI::GUIControl::GUIKey2OSKey(key->shortcutKey), mods, GTK_ACCEL_VISIBLE);
		}
	}
}

UI::GUIForm::GUIForm(GUIClientControl *parent, Double initW, Double initH, GUICore *ui) : UI::GUIClientControl(ui, parent)
{
	this->exitOnClose = false;
	this->isDialog = false;
	this->dialogResult = DR_UNKNOWN;
	this->timers = 0;
	this->okBtn = 0;
	this->cancelBtn = 0;
	NEW_CLASS(this->closeHandlers, Data::ArrayList<FormClosedEvent>());
	NEW_CLASS(this->closeHandlersObj, Data::ArrayList<void *>());
	NEW_CLASS(this->dropFileHandlers, Data::ArrayList<FileEvent>());
	NEW_CLASS(this->dropFileHandlersObj, Data::ArrayList<void *>());
	NEW_CLASS(this->menuClickedHandlers, Data::ArrayList<MenuEvent>());
	NEW_CLASS(this->menuClickedHandlersObj, Data::ArrayList<void *>());
	NEW_CLASS(this->keyDownHandlers, Data::ArrayList<KeyEvent>());
	NEW_CLASS(this->keyDownHandlersObj, Data::ArrayList<void *>());
	NEW_CLASS(this->keyUpHandlers, Data::ArrayList<KeyEvent>());
	NEW_CLASS(this->keyUpHandlersObj, Data::ArrayList<void *>());
	NEW_CLASS(this->timers, Data::ArrayList<UI::GUITimer*>());
	this->closingHdlr = 0;
	this->closingHdlrObj = 0;
	this->menu = 0;
	this->hAcc = 0;

	this->hwnd = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect((GtkWidget*)this->hwnd, "destroy", G_CALLBACK(GUIForm_EventDestroy), this);

	this->selfResize = true;
	Double w;
	Double h;
	Double initX;
	Double initY;
	if (parent)
	{
		parent->GetClientSize(&w, &h);
		initX = (w - initW) * 0.5;
		initY = (h - initH) * 0.5;
	}
	else
	{
		GdkDisplay *display = gdk_display_get_default();
		GdkScreen *scn;
		GdkRectangle rect;
		gint x;
		gint y;
#if GDK_MAJOR_VERSION > 3 || (GDK_MAJOR_VERSION == 3 && GDK_MINOR_VERSION >= 22)
		GdkSeat *seat = gdk_display_get_default_seat(display);
		GdkDevice *dev = gdk_seat_get_pointer(seat);
		if (dev)
		{
			gdk_device_get_position(dev, &scn, &x, &y);
		}
		GdkMonitor *mon = gdk_display_get_monitor_at_point(display, x, y);
		gdk_monitor_get_geometry(mon, &rect);
#else
		gdk_display_get_pointer(display, &scn, &x, &y, 0);
		gint monNum = gdk_screen_get_monitor_at_point(scn, x, y);
		gdk_screen_get_monitor_geometry(scn, monNum, &rect);
#endif
		w = rect.width * 96.0 / this->hdpi;
		h = rect.height * 96.0 / this->hdpi;
		initX = (rect.x * 96.0 / this->hdpi) + ((w - initW) * 0.5);
		initY = (rect.y * 96.0 / this->hdpi) + ((h - initH) * 0.5);
	}
	gtk_window_move((GtkWindow*)this->hwnd, Math::Double2Int32(initX), Math::Double2Int32(initY));
	gtk_window_set_default_size((GtkWindow*)this->hwnd, Math::Double2Int32(initW * this->hdpi / 96.0), Math::Double2Int32(initH * this->hdpi / 96.0));
	g_signal_connect((GtkWindow*)this->hwnd, "draw", G_CALLBACK(GUIForm_Draw), this);
	this->lxPos = initX;
	this->lyPos = initY;
	this->lxPos2 = initX + initW;
	this->lyPos2 = initY + initH;
	this->selfResize = false;
}

UI::GUIForm::~GUIForm()
{
	GUITimer *tmr;
	if (this->timers != 0)
	{
		OSInt i;
		i = this->timers->GetCount();
		while (i-- > 0)
		{
			tmr = this->timers->GetItem(i);
			DEL_CLASS(tmr);
		}
		DEL_CLASS(this->timers);
	}
	DEL_CLASS(this->dropFileHandlers);
	DEL_CLASS(this->dropFileHandlersObj);
	DEL_CLASS(this->closeHandlers);
	DEL_CLASS(this->closeHandlersObj);
	DEL_CLASS(this->menuClickedHandlers);
	DEL_CLASS(this->menuClickedHandlersObj);
	DEL_CLASS(this->keyDownHandlers);
	DEL_CLASS(this->keyDownHandlersObj);
	DEL_CLASS(this->keyUpHandlers);
	DEL_CLASS(this->keyUpHandlersObj);
	if (this->menu)
	{
		DEL_CLASS(this->menu);
		this->menu = 0;
	}
	gtk_widget_destroy((GtkWidget*)this->hwnd);
}

/*void UI::GUIForm::SetText(const WChar *text)
{
	const Char *title = Text::StrToUTF8New(text);
	gtk_window_set_title((GtkWindow*)this->hwnd, title);
	Text::StrDelNew(title);
}*/

void UI::GUIForm::SetFormState(FormState fs)
{
	switch (fs)
	{
	case FS_MAXIMIZED:
		gtk_window_maximize((GtkWindow*)this->hwnd);
		break;
	case FS_NORMAL:
		gtk_window_unmaximize((GtkWindow*)this->hwnd);
		gtk_window_deiconify((GtkWindow*)this->hwnd);
		break;
	case FS_MINIMIZED:
		gtk_window_iconify((GtkWindow*)this->hwnd);
		break;
	}
}

UI::GUIForm::DialogResult UI::GUIForm::ShowDialog(UI::GUIForm *owner)
{
	if (owner)
	{
		owner->SetEnabled(false);
	}
	this->isDialog = true;
	this->Show();
	while (this->isDialog)
	{
		gtk_main_iteration();
	}
	if (owner)
	{
		owner->SetEnabled(true);
		owner->MakeForeground();
	}
	return this->dialogResult;
}

void UI::GUIForm::SetDialogResult(DialogResult dr)
{
	if (this->isDialog)
	{
		this->dialogResult = dr;
		this->Close();
	}
}

void UI::GUIForm::SetAlwaysOnTop(Bool alwaysOnTop)
{
	gtk_window_set_keep_above((GtkWindow*)this->hwnd, alwaysOnTop);
}

void UI::GUIForm::MakeForeground()
{
	gtk_window_activate_focus((GtkWindow*)this->hwnd);
}

void UI::GUIForm::Close()
{
	gtk_window_close((GtkWindow*)this->hwnd);
	OSInt i;
	i = this->closeHandlers->GetCount();
	while (i-- > 0)
	{
		this->closeHandlers->GetItem(i)(this->closeHandlersObj->GetItem(i), this);
	}
	this->closeHandlers->Clear();
	this->closeHandlersObj->Clear();
}

void UI::GUIForm::SetText(const UTF8Char *text)
{
	gtk_window_set_title((GtkWindow*)this->hwnd, (const Char*)text);
}

void UI::GUIForm::GetSizeP(UOSInt *width, UOSInt *height)
{
	GdkWindow *wnd = gtk_widget_get_window((GtkWidget*)this->hwnd);
	if (wnd)
	{
		GdkRectangle rect;
		gdk_window_get_frame_extents(wnd, &rect);
		if (width)
			*width = rect.width;
		if (height)
			*height = rect.height;
	}
	else
	{
		int w;
		int h;
		gtk_window_get_size((GtkWindow*)this->hwnd, &w, &h);
		if (width)
			*width = w;
		if (height)
			*height = h;
	}
}

void UI::GUIForm::SetExitOnClose(Bool exitOnClose)
{
	this->exitOnClose = exitOnClose;
}

void UI::GUIForm::SetNoResize(Bool noResize)
{
	gtk_window_set_resizable((GtkWindow*)this->hwnd, noResize?FALSE:TRUE);
	if (noResize)
	{
		gtk_widget_set_size_request((GtkWidget*)this->hwnd, Math::Double2Int32((this->lxPos2 - this->lxPos)* this->hdpi / 96.0), Math::Double2Int32((this->lyPos2 - this->lyPos) * this->hdpi / 96.0));
	}
}

UI::GUITimer *UI::GUIForm::AddTimer(Int32 interval, UI::UIEvent handler, void *userObj)
{
	if (this->timers == 0)
	{
		NEW_CLASS(this->timers, Data::ArrayList<UI::GUITimer*>());
	}
	UI::GUITimer *tmr;
	NEW_CLASS(tmr, UI::GUITimer(this->ui, this, 0, interval, handler, userObj));
	this->timers->Add(tmr);
	return tmr;
}

void UI::GUIForm::RemoveTimer(UI::GUITimer *tmr)
{
	if (this->timers)
	{
		OSInt i;
		i = this->timers->GetCount();	
		while (i-- > 0)
		{
			if (tmr == this->timers->GetItem(i))
			{
				this->timers->RemoveAt(i);
				DEL_CLASS(tmr);
				break;
			}
		}
	}
}

void UI::GUIForm::SetMenu(UI::GUIMainMenu *menu)
{
	if (this->menu)
	{
		DEL_CLASS(this->menu);
	}
	if (this->container == 0) this->InitContainer();
	ClientControlData *data = (ClientControlData*)this->container;
	GtkWidget *menuBar = (GtkWidget*)menu->GetHMenu();
//	gtk_container_add(GTK_CONTAINER((GtkWidget*)this->hwnd), menuBar);
	g_object_ref(data->scrolledWin);
	gtk_container_remove(GTK_CONTAINER(data->vbox), data->scrolledWin);
	gtk_box_pack_start((GtkBox*)data->vbox, menuBar, false, false, 0);
	gtk_box_pack_start(GTK_BOX(data->vbox), data->scrolledWin, true, true, 0);
	g_object_unref(data->scrolledWin);
	gtk_widget_show(menuBar);
	this->menu = menu;
	this->menu->SetMenuForm(this);
	this->UpdateHAcc();
}

UI::GUIMainMenu *UI::GUIForm::GetMenu()
{
	return this->menu;
}

void UI::GUIForm::UpdateMenu()
{
	this->UpdateHAcc();
}

void UI::GUIForm::SetDefaultButton(UI::GUIButton *btn)
{
	this->okBtn = btn;
//	btn->SetDefaultBtnLook();
}

void UI::GUIForm::SetCancelButton(UI::GUIButton *btn)
{
	this->cancelBtn = btn;
}


void UI::GUIForm::GetClientSize(Double *w, Double *h)
{
	int width;
	int height;
	gtk_window_get_size((GtkWindow*)this->hwnd, &width, &height);
	if (this->menu)
	{
		GtkWidget *menuBar = (GtkWidget*)this->menu->GetHMenu();
		gint iheight = gtk_widget_get_allocated_height(menuBar);
		if (iheight > 0)
		{
			height -= iheight;
		}
	}
	*w = width * this->ddpi / this->hdpi;
	*h = height * this->ddpi / this->hdpi;
}

Bool UI::GUIForm::IsChildVisible()
{
	return true;
}

const UTF8Char *UI::GUIForm::GetObjectClass()
{
	return (const UTF8Char*)"WinForm";
}

OSInt UI::GUIForm::OnNotify(Int32 code, void *lParam)
{
	return 0;
}

void UI::GUIForm::OnSizeChanged(Bool updateScn)
{
	if (this->selfResize)
	{
		return;
	}
	Bool sizeChg = false;

	Double newSize;
	gint outW;
	gint outH;
	gtk_window_get_size((GtkWindow*)this->hwnd, &outW, &outH);
	if (outW != -1)
	{
		newSize = this->lxPos + outW * this->ddpi / this->hdpi;
		if (newSize != this->lxPos2)
		{
			this->lxPos2 = newSize;
			sizeChg = true;
		}
	}
	if (outH != -1)
	{
		newSize = this->lyPos + outH * this->ddpi / this->hdpi;
		if (newSize != this->lyPos2)
		{
			this->lyPos2 = newSize;
			sizeChg = true;
		}
	}
	if (sizeChg)
	{
		if (this->container)
		{
			ClientControlData *data = (ClientControlData*)this->container;
			this->selfResize = true;
//			gtk_widget_set_size_request(data->scrolledWin, outW - 3, outH - 28);
			gtk_widget_set_size_request(data->container, outW - 3, outH - 28);
			this->selfResize = false;
		}
		this->UpdateChildrenSize(false);
		this->selfResize = true;
		OSInt i = this->resizeHandlers->GetCount();
		while (i-- > 0)
		{
			this->resizeHandlers->GetItem(i)(this->resizeHandlersObjs->GetItem(i));
		}
		this->selfResize = false;
	}
}

Bool UI::GUIForm::OnPaint()
{
	return false;
}

void UI::GUIForm::EventMenuClicked(UInt16 cmdId)
{
	OSInt i;
	i = this->menuClickedHandlers->GetCount();
	while (i-- > 0)
	{
		this->menuClickedHandlers->GetItem(i)(this->menuClickedHandlersObj->GetItem(i), cmdId);
	}
}

void UI::GUIForm::ShowMouseCursor(Bool toShow)
{
	GdkCursor *cursor = gdk_cursor_new_from_name(gdk_display_get_default(), toShow?"default":"none");
	gdk_window_set_cursor(gtk_widget_get_window((GtkWidget*)this->hwnd), cursor);
}

void UI::GUIForm::HandleFormClosed(FormClosedEvent handler, void *userObj)
{
	this->closeHandlers->Add(handler);
	this->closeHandlersObj->Add(userObj);
}

void UI::GUIForm::HandleDropFiles(FileEvent handler, void *userObj)
{
	this->dropFileHandlers->Add(handler);
	this->dropFileHandlersObj->Add(userObj);
	static GtkTargetEntry target_table[] = {
	{ (gchar*)"text/uri-list", 0, 0 }
	};

	gtk_drag_dest_set((GtkWidget*)this->hwnd, GTK_DEST_DEFAULT_ALL, target_table, 1, GDK_ACTION_COPY);
	g_signal_connect((GtkWindow*)this->hwnd, "drag-data-received", G_CALLBACK(GUIForm_OnFileDrop), this);
}

void UI::GUIForm::HandleMenuClicked(MenuEvent handler, void *userObj)
{
	this->menuClickedHandlers->Add(handler);
	this->menuClickedHandlersObj->Add(userObj);
}

void UI::GUIForm::HandleKeyDown(KeyEvent handler, void *userObj)
{
	this->keyDownHandlers->Add(handler);
	this->keyDownHandlersObj->Add(userObj);
}

void UI::GUIForm::HandleKeyUp(KeyEvent handler, void *userObj)
{
	this->keyUpHandlers->Add(handler);
	this->keyUpHandlersObj->Add(userObj);
}

void UI::GUIForm::SetClosingHandler(FormClosingEvent handler, void *userObj)
{
	this->closingHdlr = handler;
	this->closingHdlrObj = userObj;
}

void UI::GUIForm::SetDPI(Double hdpi, Double ddpi)
{
	this->hdpi = hdpi;
	this->ddpi = ddpi;
	this->UpdatePos(true);
	if (this->hFont)
	{
		this->InitFont();
	}
	else
	{
		this->UpdateFont();
	}

/*	if (this->menu)
	{
		this->menu->SetDPI(hdpi, ddpi);
	}*/
	OSInt i = this->children->GetCount();
	while (i-- > 0)
	{
		this->children->GetItem(i)->SetDPI(hdpi, ddpi);
	}
	this->UpdateChildrenSize(true);
}

void UI::GUIForm::EventClosed()
{
	OSInt i;
	i = this->closeHandlers->GetCount();
	while (i-- > 0)
	{
		this->closeHandlers->GetItem(i)(this->closeHandlersObj->GetItem(i), this);
	}
	this->closeHandlers->Clear();
	this->closeHandlersObj->Clear();
	if (this->exitOnClose)
	{
		gtk_main_quit();
	}
	if (!this->isDialog)
	{
		DEL_CLASS(this);
	}
	else
	{
		this->isDialog = false;
	}
}

void UI::GUIForm::OnFocus()
{
}

void UI::GUIForm::OnFocusLost()
{
}

void UI::GUIForm::OnDisplaySizeChange(OSInt dispWidth, OSInt dispHeight)
{
}

void UI::GUIForm::OnFileDrop(const UTF8Char **files, OSInt nFiles)
{
	OSInt i;
	i = this->dropFileHandlers->GetCount();
	while (i-- > 0)
	{
		this->dropFileHandlers->GetItem(i)(this->dropFileHandlersObj->GetItem(i), files, nFiles);
	}
}

void UI::GUIForm::ToFullScn()
{
	gtk_window_fullscreen((GtkWindow*)this->hwnd);
}

void UI::GUIForm::FromFullScn()
{
	gtk_window_unfullscreen((GtkWindow*)this->hwnd);
}

UI::GUICore *UI::GUIForm::GetUI()
{
	return this->ui;
}
