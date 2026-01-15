#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/GUIForm.h"
#include "UI/GUIMenu.h"

#include <gtk/gtk.h>

typedef struct
{
	UI::GUIMenu *mnu;
	GtkWidget *mnuItem;
	UInt16 cmdId;
} MenuItemInfo;

void GUIMenu_RemoveChild(GtkWidget *widget, gpointer data)
{
	gtk_container_remove(GTK_CONTAINER((GtkWidget*)data), widget);
}

void GUIMenu_Clicked(GtkMenuItem *widget, gpointer data)
{
	MenuItemInfo *menuItem = (MenuItemInfo*)data;
	menuItem->mnu->EventMenuClick(menuItem->cmdId);
}

UnsafeArray<UTF8Char> UI::GUIMenu::ToKeyDisplay(UnsafeArray<UTF8Char> sbuff, KeyModifier keyModifier, UI::GUIControl::GUIKey shortcutKey)
{
	if (keyModifier & KM_CONTROL)
	{
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("Ctrl+"));
	}
	if (keyModifier & KM_ALT)
	{
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("Alt+"));
	}
	if (keyModifier & KM_SHIFT)
	{
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("Shift+"));
	}
	if (keyModifier & KM_WIN)
	{
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("Win+"));
	}
	switch (shortcutKey)
	{
	case UI::GUIControl::GK_BACKSPACE:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Backspace]"));
		break;
	case UI::GUIControl::GK_TAB:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Tab]"));
		break;
	case UI::GUIControl::GK_CLEAR:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Clear]"));
		break;
	case UI::GUIControl::GK_ENTER:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Enter]"));
		break;
	case UI::GUIControl::GK_SHIFT:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Shift]"));
		break;
	case UI::GUIControl::GK_CONTROL:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Ctrl]"));
		break;
	case UI::GUIControl::GK_ALT:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Alt]"));
		break;
	case UI::GUIControl::GK_PAUSE:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Pause]"));
		break;
	case UI::GUIControl::GK_CAPITAL:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Caps Lock]"));
		break;
	case UI::GUIControl::GK_KANA:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Kana]"));
		break;
	case UI::GUIControl::GK_JUNJA:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Junja]"));
		break;
	case UI::GUIControl::GK_FINAL:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Final]"));
		break;
	case UI::GUIControl::GK_KANJI:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Kanji]"));
		break;
	case UI::GUIControl::GK_ESCAPE:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[ESC]"));
		break;
	case UI::GUIControl::GK_CONVERT:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[IME Convert]"));
		break;
	case UI::GUIControl::GK_NONCONVERT:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[IME Non-convert]"));
		break;
	case UI::GUIControl::GK_ACCEPT:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Accept]"));
		break;
	case UI::GUIControl::GK_MODECHANGE:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Mode Change]"));
		break;
	case UI::GUIControl::GK_SPACE:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Space]"));
		break;
	case UI::GUIControl::GK_PAGEUP:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Pg Up]"));
		break;
	case UI::GUIControl::GK_PAGEDOWN:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Pg Dn]"));
		break;
	case UI::GUIControl::GK_END:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[End]"));
		break;
	case UI::GUIControl::GK_HOME:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Home]"));
		break;
	case UI::GUIControl::GK_LEFT:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Left]"));
		break;
	case UI::GUIControl::GK_UP:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Up]"));
		break;
	case UI::GUIControl::GK_RIGHT:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Right]"));
		break;
	case UI::GUIControl::GK_DOWN:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Down]"));
		break;
	case UI::GUIControl::GK_SELECT:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Select]"));
		break;
	case UI::GUIControl::GK_PRINT:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Print]"));
		break;
	case UI::GUIControl::GK_EXECUTE:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Execute]"));
		break;
	case UI::GUIControl::GK_PRINTSCREEN:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Prt Scn]"));
		break;
	case UI::GUIControl::GK_INSERT:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Insert]"));
		break;
	case UI::GUIControl::GK_DELETE:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Delete]"));
		break;
	case UI::GUIControl::GK_HELP:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Help]"));
		break;
	case UI::GUIControl::GK_0:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("0"));
		break;
	case UI::GUIControl::GK_1:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("1"));
		break;
	case UI::GUIControl::GK_2:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("2"));
		break;
	case UI::GUIControl::GK_3:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("3"));
		break;
	case UI::GUIControl::GK_4:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("4"));
		break;
	case UI::GUIControl::GK_5:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("5"));
		break;
	case UI::GUIControl::GK_6:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("6"));
		break;
	case UI::GUIControl::GK_7:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("7"));
		break;
	case UI::GUIControl::GK_8:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("8"));
		break;
	case UI::GUIControl::GK_9:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("9"));
		break;
	case UI::GUIControl::GK_A:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("A"));
		break;
	case UI::GUIControl::GK_B:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("B"));
		break;
	case UI::GUIControl::GK_C:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("C"));
		break;
	case UI::GUIControl::GK_D:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("D"));
		break;
	case UI::GUIControl::GK_E:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("E"));
		break;
	case UI::GUIControl::GK_F:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("F"));
		break;
	case UI::GUIControl::GK_G:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("G"));
		break;
	case UI::GUIControl::GK_H:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("H"));
		break;
	case UI::GUIControl::GK_I:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("I"));
		break;
	case UI::GUIControl::GK_J:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("J"));
		break;
	case UI::GUIControl::GK_K:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("K"));
		break;
	case UI::GUIControl::GK_L:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("L"));
		break;
	case UI::GUIControl::GK_M:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("M"));
		break;
	case UI::GUIControl::GK_N:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("N"));
		break;
	case UI::GUIControl::GK_O:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("O"));
		break;
	case UI::GUIControl::GK_P:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("P"));
		break;
	case UI::GUIControl::GK_Q:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("Q"));
		break;
	case UI::GUIControl::GK_R:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("R"));
		break;
	case UI::GUIControl::GK_S:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("S"));
		break;
	case UI::GUIControl::GK_T:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("T"));
		break;
	case UI::GUIControl::GK_U:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("U"));
		break;
	case UI::GUIControl::GK_V:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("V"));
		break;
	case UI::GUIControl::GK_W:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("W"));
		break;
	case UI::GUIControl::GK_X:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("X"));
		break;
	case UI::GUIControl::GK_Y:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("Y"));
		break;
	case UI::GUIControl::GK_Z:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("Z"));
		break;
	case UI::GUIControl::GK_LWIN:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Left Win]"));
		break;
	case UI::GUIControl::GK_RWIN:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Right Win]"));
		break;
	case UI::GUIControl::GK_APPS:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Apps]"));
		break;
	case UI::GUIControl::GK_SLEEP:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Sleep]"));
		break;
	case UI::GUIControl::GK_NUMPAD0:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Num0]"));
		break;
	case UI::GUIControl::GK_NUMPAD1:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Num1]"));
		break;
	case UI::GUIControl::GK_NUMPAD2:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Num2]"));
		break;
	case UI::GUIControl::GK_NUMPAD3:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Num3]"));
		break;
	case UI::GUIControl::GK_NUMPAD4:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Num4]"));
		break;
	case UI::GUIControl::GK_NUMPAD5:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Num5]"));
		break;
	case UI::GUIControl::GK_NUMPAD6:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Num6]"));
		break;
	case UI::GUIControl::GK_NUMPAD7:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Num7]"));
		break;
	case UI::GUIControl::GK_NUMPAD8:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Num8]"));
		break;
	case UI::GUIControl::GK_NUMPAD9:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Num9]"));
		break;
	case UI::GUIControl::GK_MULTIPLY:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("*"));
		break;
	case UI::GUIControl::GK_ADD:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("+"));
		break;
	case UI::GUIControl::GK_SEPARATOR:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("|"));
		break;
	case UI::GUIControl::GK_SUBTRACT:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("-"));
		break;
	case UI::GUIControl::GK_DECIMAL:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("."));
		break;
	case UI::GUIControl::GK_DIVIDE:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("/"));
		break;
	case UI::GUIControl::GK_F1:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("F1"));
		break;
	case UI::GUIControl::GK_F2:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("F2"));
		break;
	case UI::GUIControl::GK_F3:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("F3"));
		break;
	case UI::GUIControl::GK_F4:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("F4"));
		break;
	case UI::GUIControl::GK_F5:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("F5"));
		break;
	case UI::GUIControl::GK_F6:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("F6"));
		break;
	case UI::GUIControl::GK_F7:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("F7"));
		break;
	case UI::GUIControl::GK_F8:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("F8"));
		break;
	case UI::GUIControl::GK_F9:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("F9"));
		break;
	case UI::GUIControl::GK_F10:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("F10"));
		break;
	case UI::GUIControl::GK_F11:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("F11"));
		break;
	case UI::GUIControl::GK_F12:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("F12"));
		break;
	case UI::GUIControl::GK_F13:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("F13"));
		break;
	case UI::GUIControl::GK_F14:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("F14"));
		break;
	case UI::GUIControl::GK_F15:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("F15"));
		break;
	case UI::GUIControl::GK_F16:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("F16"));
		break;
	case UI::GUIControl::GK_F17:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("F17"));
		break;
	case UI::GUIControl::GK_F18:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("F18"));
		break;
	case UI::GUIControl::GK_F19:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("F19"));
		break;
	case UI::GUIControl::GK_F20:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("F20"));
		break;
	case UI::GUIControl::GK_F21:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("F21"));
		break;
	case UI::GUIControl::GK_F22:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("F22"));
		break;
	case UI::GUIControl::GK_F23:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("F23"));
		break;
	case UI::GUIControl::GK_F24:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("F24"));
		break;
	case UI::GUIControl::GK_NUMLOCK:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Num Lock]"));
		break;
	case UI::GUIControl::GK_SCROLLLOCK:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[Scroll Lock]"));
		break;
	case UI::GUIControl::GK_OEM_1:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[;]"));
		break;
	case UI::GUIControl::GK_OEM_PLUS:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[+]"));
		break;
	case UI::GUIControl::GK_OEM_COMMA:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[,]"));
		break;
	case UI::GUIControl::GK_OEM_MINUS:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[-]"));
		break;
	case UI::GUIControl::GK_OEM_PERIOD:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[.]"));
		break;
	case UI::GUIControl::GK_OEM_2:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[/]"));
		break;
	case UI::GUIControl::GK_OEM_3:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[-]"));
		break;
	case UI::GUIControl::GK_OEM_4:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("'['"));
		break;
	case UI::GUIControl::GK_OEM_5:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[\\]"));
		break;
	case UI::GUIControl::GK_OEM_6:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("']'"));
		break;
	case UI::GUIControl::GK_OEM_7:
		sbuff = Text::StrConcatC(sbuff, UTF8STRC("[']"));
		break;
	case UI::GUIControl::GK_NONE:
	default:
		break;
	}
	return sbuff;
}

UI::GUIMenu::GUIMenu(Bool isPopup)
{
	this->hdpi = 96.0;
	this->mnuForm = nullptr;

	if (isPopup)
	{
		this->hMenu = gtk_menu_new();
	}
	else
	{
		this->hMenu = gtk_menu_bar_new();
	}
	this->itemCnt = 0;
}

UI::GUIMenu::~GUIMenu()
{
	NN<MenuItemInfo> menuItem;
	UOSInt i;
	this->keys.MemFreeAll();
	this->subMenus.DeleteAll();
	i = this->items.GetCount();
	while (i-- > 0)
	{
		menuItem = this->items.GetItem(i).GetNN<MenuItemInfo>();
		MemFreeNN(menuItem);
	}
}

UOSInt UI::GUIMenu::AddItem(Text::CStringNN name, UInt16 cmdId, KeyModifier keyModifier, UI::GUIControl::GUIKey shortcutKey)
{
	UOSInt id = this->itemCnt++;
	Char buff[128];
	Char *sptr = buff;
	Char c;
	Bool hasUL = false;
	NN<MenuItemInfo> mnuItem;
	GtkWidget *menuItem;
	name.ConcatTo((UTF8Char*)buff);
	while (true)
	{
		c = *sptr++;
		if (c == '&')
		{
			sptr[-1] = '_';
			hasUL = true;
		}
		else if (c == 0)
		{
			break;
		}
	}
	menuItem = gtk_menu_item_new_with_label(buff);
	if (hasUL)
	{
		gtk_menu_item_set_use_underline(GTK_MENU_ITEM(menuItem), TRUE);
	}
	mnuItem = MemAllocNN(MenuItemInfo);
	mnuItem->mnu = this;
	mnuItem->cmdId = cmdId;
	mnuItem->mnuItem = menuItem;
	this->items.Add(mnuItem);

	if (shortcutKey)
	{
		NN<ShortcutKey> key = MemAllocNN(ShortcutKey);
		key->cmdId = cmdId;
		key->keyModifier = keyModifier;
		key->shortcutKey = shortcutKey;
		key->menuItem = menuItem;
		this->keys.Add(key);
	}
	gtk_menu_shell_append(GTK_MENU_SHELL(this->hMenu), menuItem);
	g_signal_connect(menuItem, "activate", G_CALLBACK(GUIMenu_Clicked), mnuItem.Ptr());
	gtk_widget_show(menuItem);
	return id;
}

void UI::GUIMenu::AddSeperator()
{
	this->itemCnt++;
	GtkWidget *menuItem;
	menuItem = gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(this->hMenu), menuItem);
	gtk_widget_show(menuItem);
}

NN<UI::GUIMenu> UI::GUIMenu::AddSubMenu(Text::CStringNN name)
{
	NN<UI::GUIMenu> subMenu;
	NEW_CLASSNN(subMenu, UI::GUIMenu(true));
	this->subMenus.Add(subMenu);

	Char buff[128];
	Char *sptr = buff;
	Char c;
	Bool hasUL = false;
	GtkWidget *menuItem;
	name.ConcatTo((UTF8Char*)buff);
	while (true)
	{
		c = *sptr;
		if (c == '&')
		{
			*sptr = '_';
			hasUL = true;
			sptr++;
		}
		else if (c == 0)
		{
			break;
		}
		else
		{
			sptr++;
		}
	}
	menuItem = gtk_menu_item_new_with_label(buff);
	if (hasUL)
	{
		gtk_menu_item_set_use_underline(GTK_MENU_ITEM(menuItem), TRUE);
	}
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuItem), (GtkWidget*)subMenu->hMenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(this->hMenu), menuItem);
	gtk_widget_show(menuItem);
	return subMenu;
}

void *UI::GUIMenu::GetHMenu()
{
	return this->hMenu;
}

UOSInt UI::GUIMenu::GetAllKeys(NN<Data::ArrayListNN<ShortcutKey>> keys)
{
	UOSInt keyCnt = this->keys.GetCount();
	keys->AddAll(this->keys);
	NN<GUIMenu> menu;
	UOSInt j = this->subMenus.GetCount();
	UOSInt i = 0;
	while (i < j)
	{
		if (this->subMenus.GetItem(i).SetTo(menu))
		{
			keyCnt += menu->GetAllKeys(keys);
		}
		i++;
	}
	return keyCnt;
}

void UI::GUIMenu::SetItemEnabled(UInt16 cmdId, Bool enabled)
{
	//EnableMenuItem((HMENU)this->hMenu, cmdId, enabled?MF_ENABLED:MF_GRAYED);
}

void UI::GUIMenu::ClearItems()
{
	NN<MenuItemInfo> menuItem;
	UOSInt i;
	this->keys.MemFreeAll();
	this->subMenus.DeleteAll();
	i = this->items.GetCount();
	while (i-- > 0)
	{
		menuItem = this->items.GetItem(i).GetNN<MenuItemInfo>();
		MemFreeNN(menuItem);
	}
	this->items.Clear();
	gtk_container_foreach(GTK_CONTAINER((GtkWidget*)this->hMenu), GUIMenu_RemoveChild, this->hMenu);
}

void UI::GUIMenu::SetDPI(Double hdpi, Double ddpi)
{
	this->hdpi = hdpi;
	this->ddpi = ddpi;
}

void UI::GUIMenu::SetMenuForm(Optional<GUIForm> mnuForm)
{
	NN<UI::GUIMenu> item;
	UOSInt i;
	this->mnuForm = mnuForm;
	i = this->subMenus.GetCount();
	while (i-- > 0)
	{
		if (this->subMenus.GetItem(i).SetTo(item))
		{
			item->SetMenuForm(mnuForm);
		}
	}
}

void UI::GUIMenu::EventMenuClick(UInt16 cmdId)
{
	NN<GUIForm> frm;
	if (this->mnuForm.SetTo(frm))
	{
		frm->EventMenuClicked(cmdId);
	}
}

