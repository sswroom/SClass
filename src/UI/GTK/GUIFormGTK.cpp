#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListStringNN.h"
#include "Math/Math_C.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/TextBinEnc/URIEncoding.h"
#include "UI/GUIForm.h"
#include "UI/GTK/GTKTimer.h"
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

gboolean GUIForm_SetNoResize(gpointer user_data)
{
	gtk_window_set_resizable((GtkWindow*)user_data, false);
	return false;
}


void GUIForm_OnFileDrop(GtkWidget *widget, GdkDragContext *context, gint x, gint y, GtkSelectionData *data, guint info, guint time, gpointer userData)
{
	UI::GUIForm *me = (UI::GUIForm *)userData;
	Text::StringBuilderUTF8 sb;
	Data::ArrayListStringNN files;
	sb.AppendSlow((const UTF8Char*)gtk_selection_data_get_data(data));
	Text::PString sarr[2];
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	sarr[1] = sb;
	while (true)
	{
		i = Text::StrSplitP(sarr, 2, sarr[1], '\n');
		if (sarr[0].StartsWith(UTF8STRC("file://")))
		{
			j = sarr[0].leng;
			if (sarr[0].v[j - 1] == '\r')
				sarr[0].v[j - 1] = 0;
			sptr = Text::TextBinEnc::URIEncoding::URIDecode(sbuff, &sarr[0].v[7]);
			files.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
		}
		if (i <= 1)
			break;
	}
	i = files.GetCount();
	if (i > 0)
	{
		me->OnFileDrop(files.ToArray());
		while (i-- > 0)
		{
			OPTSTR_DEL(files.GetItem(i));
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
		gtk_window_add_accel_group(GTK_WINDOW((GtkWidget*)this->hwnd.OrNull()), accel_group);
	}
	else
	{
		accel_group = (GtkAccelGroup*)this->hAcc;
	}
	NN<UI::GUIMainMenu> menu;
	if (this->menu.SetTo(menu))
	{
		UOSInt i;
		Data::ArrayListNN<UI::GUIMenu::ShortcutKey> keys;
		NN<UI::GUIMenu::ShortcutKey> key;
		menu->GetAllKeys(keys);
		i = keys.GetCount();
		while (i-- > 0)
		{
			key = keys.GetItemNoCheck(i);
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

UI::GUIForm::GUIForm(Optional<GUIClientControl> parent, Double initW, Double initH, NN<GUICore> ui) : UI::GUIClientControl(ui, parent)
{
	this->exitOnClose = false;
	this->isDialog = false;
	this->dialogResult = DR_UNKNOWN;
	this->okBtn = nullptr;
	this->cancelBtn = nullptr;
	this->closingHdlr = 0;
	this->menu = nullptr;
	this->hAcc = 0;

	this->hwnd = (ControlHandle*)gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect((GtkWidget*)this->hwnd.OrNull(), "destroy", G_CALLBACK(GUIForm_EventDestroy), this);

	this->selfResize = true;
	Math::Size2DDbl sz;
	Double initX;
	Double initY;
	NN<GUIClientControl> nnparent;
	if (parent.SetTo(nnparent))
	{
		sz = nnparent->GetClientSize();
		initX = (sz.x - initW) * 0.5;
		initY = (sz.y - initH) * 0.5;
	}
	else
	{
		GdkDisplay *display = gdk_display_get_default();
		GdkScreen *scn;
		GdkRectangle rect;
		gint x = 0;
		gint y = 0;
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
/*		sz.x = rect.width;
		sz.y = rect.height;
		initW = initW * this->hdpi / this->ddpi;
		initH = initH * this->hdpi / this->ddpi;
		if (initW > w)
		{
			initW = w;
		}
		if (initH > h)
		{
			initH = h;
		}
		initX = x + ((w - initW) * 0.5);
		initY = y + ((h - initH) * 0.5);
	}
	gtk_window_move((GtkWindow*)this->hwnd, Double2Int32(initX), Double2Int32(initY));
	gtk_window_set_default_size((GtkWindow*)this->hwnd, Double2Int32(initW), Double2Int32(initH));
	g_signal_connect((GtkWindow*)this->hwnd, "draw", G_CALLBACK(GUIForm_Draw), this);
	initX = initX * this->ddpi / this->hdpi;
	initY = initY * this->ddpi / this->hdpi;
	initW = initW * this->ddpi / this->hdpi;
	initH = initH * this->ddpi / this->hdpi;*/
		sz = Math::Size2DDbl(rect.width, rect.height) * 96.0 / this->hdpi;
		initX = (rect.x * 96.0 / this->hdpi) + ((sz.x - initW) * 0.5);
		initY = (rect.y * 96.0 / this->hdpi) + ((sz.y - initH) * 0.5);
	}
	gtk_window_move((GtkWindow*)this->hwnd.OrNull(), Double2Int32(initX), Double2Int32(initY));
	gtk_window_set_default_size((GtkWindow*)this->hwnd.OrNull(), Double2Int32(initW * this->hdpi / 96.0), Double2Int32(initH * this->hdpi / 96.0));
	g_signal_connect((GtkWindow*)this->hwnd.OrNull(), "draw", G_CALLBACK(GUIForm_Draw), this);
	this->lxPos = initX;
	this->lyPos = initY;
	this->lxPos2 = initX + initW;
	this->lyPos2 = initY + initH;
	this->selfResize = false;
}

UI::GUIForm::~GUIForm()
{
	UOSInt i;
	i = this->timers.GetCount();
	while (i-- > 0)
	{
		this->timers.GetItem(i).Delete();
	}
	this->isDialog = true;
	GtkWidget *widget = (GtkWidget*)this->hwnd.OrNull();
	this->hwnd = nullptr;
	if (widget)
	{
		gtk_widget_destroy(widget);
	}
	this->menu.Delete();
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
		gtk_window_maximize((GtkWindow*)this->hwnd.OrNull());
		break;
	case FS_NORMAL:
		gtk_window_unmaximize((GtkWindow*)this->hwnd.OrNull());
		gtk_window_deiconify((GtkWindow*)this->hwnd.OrNull());
		break;
	case FS_MINIMIZED:
		gtk_window_iconify((GtkWindow*)this->hwnd.OrNull());
		break;
	}
}

UI::GUIForm::DialogResult UI::GUIForm::ShowDialog(Optional<UI::GUIForm> owner)
{
	NN<UI::GUIForm> frm;
	if (owner.SetTo(frm))
	{
		frm->SetEnabled(false);
	}
	this->isDialog = true;
	this->Show();
	while (this->isDialog)
	{
		gtk_main_iteration();
	}
	this->hwnd = nullptr;
	if (owner.SetTo(frm))
	{
		frm->SetEnabled(true);
		frm->MakeForeground();
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
	gtk_window_set_keep_above((GtkWindow*)this->hwnd.OrNull(), alwaysOnTop);
}

void UI::GUIForm::MakeForeground()
{
	gtk_window_activate_focus((GtkWindow*)this->hwnd.OrNull());
}

void UI::GUIForm::Close()
{
#if GDK_VERSION_CUR_STABLE >= G_ENCODE_VERSION (3, 10)
	gtk_window_close((GtkWindow*)this->hwnd.OrNull());
#else
	gtk_widget_destroy((GtkWidget*)this->hwnd);
#endif
	UOSInt i;
	i = this->closeHandlers.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<FormClosedEvent> cb = this->closeHandlers.GetItem(i);
		cb.func(cb.userObj, *this);
	}
	this->closeHandlers.Clear();
}

void UI::GUIForm::SetText(Text::CStringNN text)
{
	gtk_window_set_title((GtkWindow*)this->hwnd.OrNull(), (const Char*)text.v.Ptr());
}

Math::Size2D<UOSInt> UI::GUIForm::GetSizeP()
{
	GdkWindow *wnd = gtk_widget_get_window((GtkWidget*)this->hwnd.OrNull());
	if (wnd)
	{
		GdkRectangle rect;
		gdk_window_get_frame_extents(wnd, &rect);
		return Math::Size2D<UOSInt>((UInt32)rect.width, (UInt32)rect.height);
	}
	else
	{
		int w;
		int h;
		gtk_window_get_size((GtkWindow*)this->hwnd.OrNull(), &w, &h);
		return Math::Size2D<UOSInt>((UInt32)w, (UInt32)h);
	}
}

void UI::GUIForm::SetExitOnClose(Bool exitOnClose)
{
	this->exitOnClose = exitOnClose;
}

void UI::GUIForm::SetNoResize(Bool noResize)
{
	if (noResize)
	{
/*		if (this->lxPos2 < this->lxPos)
		{
			this->lxPos2 = this->lxPos;
		}
		if (this->lyPos2 < this->lyPos)
		{
			this->lyPos2 = this->lyPos;
		}
		gtk_widget_set_size_request((GtkWidget*)this->hwnd, Double2Int32((this->lxPos2 - this->lxPos) * this->hdpi / 96.0), Double2Int32((this->lyPos2 - this->lyPos) * this->hdpi / 96.0));*/
		g_idle_add(GUIForm_SetNoResize, this->hwnd.OrNull());
	}
	else
	{
		gtk_window_set_resizable((GtkWindow*)this->hwnd.OrNull(), TRUE);
	}
}

NN<UI::GUITimer> UI::GUIForm::AddTimer(UInt32 interval, UI::UIEvent handler, AnyType userObj)
{
	NN<UI::GTK::GTKTimer> tmr;
	NEW_CLASSNN(tmr, UI::GTK::GTKTimer(interval, handler, userObj));
	this->timers.Add(tmr);
	return tmr;
}

void UI::GUIForm::RemoveTimer(NN<UI::GUITimer> tmr)
{
	UOSInt i;
	i = this->timers.GetCount();	
	while (i-- > 0)
	{
		if (tmr.Ptr() == this->timers.GetItem(i).OrNull())
		{
			this->timers.RemoveAt(i);
			tmr.Delete();
			break;
		}
	}
}

void UI::GUIForm::SetMenu(NN<UI::GUIMainMenu> menu)
{
	this->menu.Delete();
	if (this->container == 0) this->InitContainer();
	NN<ClientControlData> data = this->container.GetNN<ClientControlData>();
	GtkWidget *menuBar = (GtkWidget*)menu->GetHMenu();
//	gtk_container_add(GTK_CONTAINER((GtkWidget*)this->hwnd), menuBar);
	g_object_ref(data->scrolledWin);
	gtk_container_remove(GTK_CONTAINER(data->vbox), data->scrolledWin);
	gtk_box_pack_start((GtkBox*)data->vbox, menuBar, false, false, 0);
	gtk_box_pack_start(GTK_BOX(data->vbox), data->scrolledWin, true, true, 0);
	g_object_unref(data->scrolledWin);
	gtk_widget_show(menuBar);
	this->menu = menu;
	menu->SetMenuForm(this);
	this->UpdateHAcc();
}

Optional<UI::GUIMainMenu> UI::GUIForm::GetMenu()
{
	return this->menu;
}

void UI::GUIForm::UpdateMenu()
{
	this->UpdateHAcc();
}

void UI::GUIForm::SetDefaultButton(NN<UI::GUIButton> btn)
{
	this->okBtn = btn;
//	btn->SetDefaultBtnLook();
}

void UI::GUIForm::SetCancelButton(NN<UI::GUIButton> btn)
{
	this->cancelBtn = btn;
}

Optional<UI::GUIButton> UI::GUIForm::GetDefaultButton()
{
	return this->okBtn;
}

Optional<UI::GUIButton> UI::GUIForm::GetCancelButton()
{
	return this->cancelBtn;
}

Math::Size2DDbl UI::GUIForm::GetClientSize()
{
	int width;
	int height;
	gtk_window_get_size((GtkWindow*)this->hwnd.OrNull(), &width, &height);
	NN<UI::GUIMainMenu> menu;
	if (this->menu.SetTo(menu))
	{
		GtkWidget *menuBar = (GtkWidget*)menu->GetHMenu();
		if (gtk_widget_get_visible(menuBar))
		{
			gint iheight = gtk_widget_get_allocated_height(menuBar);
			if (iheight > 0)
			{
				height -= iheight;
			}
		}
	}
	return Math::Size2DDbl(width, height) * this->ddpi / this->hdpi;
}

Bool UI::GUIForm::IsChildVisible()
{
	return true;
}

Text::CStringNN UI::GUIForm::GetObjectClass() const
{
	return CSTR("WinForm");
}

OSInt UI::GUIForm::OnNotify(UInt32 code, void *lParam)
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
	gtk_window_get_size((GtkWindow*)this->hwnd.OrNull(), &outW, &outH);
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
		NN<ClientControlData> data;
		if (this->container.GetOpt<ClientControlData>().SetTo(data))
		{
			this->selfResize = true;
			if (outW < 3)
			{
				outW = 3;
			}
			if (outH < 28)
			{
				outH = 28;
			}
//			gtk_widget_set_size_request(data->scrolledWin, outW - 3, outH - 28);
			gtk_widget_set_size_request(data->container, outW - 3, outH - 28);
			this->selfResize = false;
		}
		this->UpdateChildrenSize(false);
		this->selfResize = true;
		UOSInt i = this->resizeHandlers.GetCount();
		while (i-- > 0)
		{
			Data::CallbackStorage<UIEvent> cb = this->resizeHandlers.GetItem(i);
			cb.func(cb.userObj);
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
	UOSInt i;
	i = this->menuClickedHandlers.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<MenuEvent> cb = this->menuClickedHandlers.GetItem(i);
		cb.func(cb.userObj, cmdId);
	}
}

void UI::GUIForm::ShowMouseCursor(Bool toShow)
{
	GdkCursor *cursor = gdk_cursor_new_from_name(gdk_display_get_default(), toShow?"default":"none");
	gdk_window_set_cursor(gtk_widget_get_window((GtkWidget*)this->hwnd.OrNull()), cursor);
}

void UI::GUIForm::HandleFormClosed(FormClosedEvent handler, AnyType userObj)
{
	this->closeHandlers.Add({handler, userObj});
}

void UI::GUIForm::HandleDropFiles(FileEvent handler, AnyType userObj)
{
	this->dropFileHandlers.Add({handler, userObj});
	static GtkTargetEntry target_table[] = {
	{ (gchar*)"text/uri-list", 0, 0 }
	};

	gtk_drag_dest_set((GtkWidget*)this->hwnd.OrNull(), GTK_DEST_DEFAULT_ALL, target_table, 1, GDK_ACTION_COPY);
	g_signal_connect((GtkWindow*)this->hwnd.OrNull(), "drag-data-received", G_CALLBACK(GUIForm_OnFileDrop), this);
}

void UI::GUIForm::HandleMenuClicked(MenuEvent handler, AnyType userObj)
{
	this->menuClickedHandlers.Add({handler, userObj});
}

void UI::GUIForm::HandleKeyDown(KeyEvent handler, AnyType userObj)
{
	this->keyDownHandlers.Add({handler, userObj});
}

void UI::GUIForm::HandleKeyUp(KeyEvent handler, AnyType userObj)
{
	this->keyUpHandlers.Add({handler, userObj});
}

void UI::GUIForm::SetClosingHandler(FormClosingEvent handler, AnyType userObj)
{
	this->closingHdlr = {handler, userObj};
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
	Data::ArrayIterator<NN<GUIControl>> it = this->children.Iterator();
	while (it.HasNext())
	{
		it.Next()->SetDPI(hdpi, ddpi);
	}
	this->UpdateChildrenSize(true);
}

void UI::GUIForm::EventClosed()
{
	UOSInt i;
	i = this->closeHandlers.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<FormClosedEvent> cb = this->closeHandlers.GetItem(i);
		cb.func(cb.userObj, *this);
	}
	this->closeHandlers.Clear();
	if (this->exitOnClose)
	{
		gtk_main_quit();
	}
	if (!this->isDialog)
	{
		this->DestroyObject();
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

void UI::GUIForm::OnDisplaySizeChange(UOSInt dispWidth, UOSInt dispHeight)
{
}

void UI::GUIForm::OnFileDrop(Data::DataArray<NN<Text::String>> files)
{
	UOSInt i;
	i = this->dropFileHandlers.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<FileEvent> cb = this->dropFileHandlers.GetItem(i);
		cb.func(cb.userObj, files);
	}
}

void UI::GUIForm::ToFullScn()
{
	NN<UI::GUIMainMenu> menu;
	if (this->menu.SetTo(menu))
	{
		GtkWidget *menuBar = (GtkWidget*)menu->GetHMenu();
		gtk_widget_set_visible(menuBar, FALSE);
	}
	gtk_window_fullscreen((GtkWindow*)this->hwnd.OrNull());
}

void UI::GUIForm::FromFullScn()
{
	gtk_window_unfullscreen((GtkWindow*)this->hwnd.OrNull());
	NN<UI::GUIMainMenu> menu;
	if (this->menu.SetTo(menu))
	{
		GtkWidget *menuBar = (GtkWidget*)menu->GetHMenu();
		gtk_widget_show(menuBar);
	}
}

NN<UI::GUICore> UI::GUIForm::GetUI()
{
	return this->ui;
}
