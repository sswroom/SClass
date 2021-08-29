#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "UI/GUIMenu.h"

#include <windows.h>
#undef MK_ALT
#undef MK_CONTROL
#undef MK_SHIFT

UTF8Char *UI::GUIMenu::ToKeyDisplay(UTF8Char *sbuff, KeyModifier keyModifier, UI::GUIControl::GUIKey shortcutKey)
{
	if (keyModifier & KM_CONTROL)
	{
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"Ctrl+");
	}
	if (keyModifier & KM_ALT)
	{
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"Alt+");
	}
	if (keyModifier & KM_SHIFT)
	{
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"Shift+");
	}
	if (keyModifier & KM_WIN)
	{
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"Win+");
	}
	switch (shortcutKey)
	{
	case UI::GUIControl::GK_BACKSPACE:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Backspace]");
		break;
	case UI::GUIControl::GK_TAB:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Tab]");
		break;
	case UI::GUIControl::GK_CLEAR:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Clear]");
		break;
	case UI::GUIControl::GK_ENTER:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Enter]");
		break;
	case UI::GUIControl::GK_SHIFT:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Shift]");
		break;
	case UI::GUIControl::GK_CONTROL:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Ctrl]");
		break;
	case UI::GUIControl::GK_ALT:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Alt]");
		break;
	case UI::GUIControl::GK_PAUSE:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Pause]");
		break;
	case UI::GUIControl::GK_CAPITAL:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Caps Lock]");
		break;
	case UI::GUIControl::GK_KANA:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Kana]");
		break;
	case UI::GUIControl::GK_JUNJA:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Junja]");
		break;
	case UI::GUIControl::GK_FINAL:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Final]");
		break;
	case UI::GUIControl::GK_KANJI:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Kanji]");
		break;
	case UI::GUIControl::GK_ESCAPE:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[ESC]");
		break;
	case UI::GUIControl::GK_CONVERT:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[IME Convert]");
		break;
	case UI::GUIControl::GK_NONCONVERT:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[IME Non-convert]");
		break;
	case UI::GUIControl::GK_ACCEPT:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Accept]");
		break;
	case UI::GUIControl::GK_MODECHANGE:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Mode Change]");
		break;
	case UI::GUIControl::GK_SPACE:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Space]");
		break;
	case UI::GUIControl::GK_PAGEUP:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Pg Up]");
		break;
	case UI::GUIControl::GK_PAGEDOWN:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Pg Dn]");
		break;
	case UI::GUIControl::GK_END:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[End]");
		break;
	case UI::GUIControl::GK_HOME:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Home]");
		break;
	case UI::GUIControl::GK_LEFT:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Left]");
		break;
	case UI::GUIControl::GK_UP:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Up]");
		break;
	case UI::GUIControl::GK_RIGHT:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Right]");
		break;
	case UI::GUIControl::GK_DOWN:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Down]");
		break;
	case UI::GUIControl::GK_SELECT:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Select]");
		break;
	case UI::GUIControl::GK_PRINT:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Print]");
		break;
	case UI::GUIControl::GK_EXECUTE:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Execute]");
		break;
	case UI::GUIControl::GK_PRINTSCREEN:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Prt Scn]");
		break;
	case UI::GUIControl::GK_INSERT:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Insert]");
		break;
	case UI::GUIControl::GK_DELETE:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Delete]");
		break;
	case UI::GUIControl::GK_HELP:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Help]");
		break;
	case UI::GUIControl::GK_0:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"0");
		break;
	case UI::GUIControl::GK_1:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"1");
		break;
	case UI::GUIControl::GK_2:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"2");
		break;
	case UI::GUIControl::GK_3:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"3");
		break;
	case UI::GUIControl::GK_4:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"4");
		break;
	case UI::GUIControl::GK_5:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"5");
		break;
	case UI::GUIControl::GK_6:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"6");
		break;
	case UI::GUIControl::GK_7:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"7");
		break;
	case UI::GUIControl::GK_8:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"8");
		break;
	case UI::GUIControl::GK_9:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"9");
		break;
	case UI::GUIControl::GK_A:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"A");
		break;
	case UI::GUIControl::GK_B:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"B");
		break;
	case UI::GUIControl::GK_C:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"C");
		break;
	case UI::GUIControl::GK_D:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"D");
		break;
	case UI::GUIControl::GK_E:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"E");
		break;
	case UI::GUIControl::GK_F:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"F");
		break;
	case UI::GUIControl::GK_G:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"G");
		break;
	case UI::GUIControl::GK_H:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"H");
		break;
	case UI::GUIControl::GK_I:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"I");
		break;
	case UI::GUIControl::GK_J:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"J");
		break;
	case UI::GUIControl::GK_K:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"K");
		break;
	case UI::GUIControl::GK_L:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"L");
		break;
	case UI::GUIControl::GK_M:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"M");
		break;
	case UI::GUIControl::GK_N:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"N");
		break;
	case UI::GUIControl::GK_O:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"O");
		break;
	case UI::GUIControl::GK_P:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"P");
		break;
	case UI::GUIControl::GK_Q:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"Q");
		break;
	case UI::GUIControl::GK_R:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"R");
		break;
	case UI::GUIControl::GK_S:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"S");
		break;
	case UI::GUIControl::GK_T:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"T");
		break;
	case UI::GUIControl::GK_U:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"U");
		break;
	case UI::GUIControl::GK_V:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"V");
		break;
	case UI::GUIControl::GK_W:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"W");
		break;
	case UI::GUIControl::GK_X:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"X");
		break;
	case UI::GUIControl::GK_Y:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"Y");
		break;
	case UI::GUIControl::GK_Z:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"Z");
		break;
	case UI::GUIControl::GK_LWIN:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Left Win]");
		break;
	case UI::GUIControl::GK_RWIN:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Right Win]");
		break;
	case UI::GUIControl::GK_APPS:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Apps]");
		break;
	case UI::GUIControl::GK_SLEEP:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Sleep]");
		break;
	case UI::GUIControl::GK_NUMPAD0:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Num0]");
		break;
	case UI::GUIControl::GK_NUMPAD1:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Num1]");
		break;
	case UI::GUIControl::GK_NUMPAD2:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Num2]");
		break;
	case UI::GUIControl::GK_NUMPAD3:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Num3]");
		break;
	case UI::GUIControl::GK_NUMPAD4:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Num4]");
		break;
	case UI::GUIControl::GK_NUMPAD5:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Num5]");
		break;
	case UI::GUIControl::GK_NUMPAD6:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Num6]");
		break;
	case UI::GUIControl::GK_NUMPAD7:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Num7]");
		break;
	case UI::GUIControl::GK_NUMPAD8:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Num8]");
		break;
	case UI::GUIControl::GK_NUMPAD9:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Num9]");
		break;
	case UI::GUIControl::GK_MULTIPLY:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"*");
		break;
	case UI::GUIControl::GK_ADD:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"+");
		break;
	case UI::GUIControl::GK_SEPARATOR:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"|");
		break;
	case UI::GUIControl::GK_SUBTRACT:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"-");
		break;
	case UI::GUIControl::GK_DECIMAL:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)".");
		break;
	case UI::GUIControl::GK_DIVIDE:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"/");
		break;
	case UI::GUIControl::GK_F1:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"F1");
		break;
	case UI::GUIControl::GK_F2:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"F2");
		break;
	case UI::GUIControl::GK_F3:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"F3");
		break;
	case UI::GUIControl::GK_F4:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"F4");
		break;
	case UI::GUIControl::GK_F5:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"F5");
		break;
	case UI::GUIControl::GK_F6:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"F6");
		break;
	case UI::GUIControl::GK_F7:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"F7");
		break;
	case UI::GUIControl::GK_F8:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"F8");
		break;
	case UI::GUIControl::GK_F9:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"F9");
		break;
	case UI::GUIControl::GK_F10:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"F10");
		break;
	case UI::GUIControl::GK_F11:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"F11");
		break;
	case UI::GUIControl::GK_F12:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"F12");
		break;
	case UI::GUIControl::GK_F13:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"F13");
		break;
	case UI::GUIControl::GK_F14:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"F14");
		break;
	case UI::GUIControl::GK_F15:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"F15");
		break;
	case UI::GUIControl::GK_F16:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"F16");
		break;
	case UI::GUIControl::GK_F17:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"F17");
		break;
	case UI::GUIControl::GK_F18:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"F18");
		break;
	case UI::GUIControl::GK_F19:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"F19");
		break;
	case UI::GUIControl::GK_F20:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"F20");
		break;
	case UI::GUIControl::GK_F21:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"F21");
		break;
	case UI::GUIControl::GK_F22:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"F22");
		break;
	case UI::GUIControl::GK_F23:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"F23");
		break;
	case UI::GUIControl::GK_F24:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"F24");
		break;
	case UI::GUIControl::GK_NUMLOCK:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Num Lock]");
		break;
	case UI::GUIControl::GK_SCROLLLOCK:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[Scroll Lock]");
		break;
	case UI::GUIControl::GK_OEM_1:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[;]");
		break;
	case UI::GUIControl::GK_OEM_PLUS:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[+]");
		break;
	case UI::GUIControl::GK_OEM_COMMA:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[,]");
		break;
	case UI::GUIControl::GK_OEM_MINUS:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[-]");
		break;
	case UI::GUIControl::GK_OEM_PERIOD:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[.]");
		break;
	case UI::GUIControl::GK_OEM_2:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[/]");
		break;
	case UI::GUIControl::GK_OEM_3:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[-]");
		break;
	case UI::GUIControl::GK_OEM_4:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"'['");
		break;
	case UI::GUIControl::GK_OEM_5:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[\\]");
		break;
	case UI::GUIControl::GK_OEM_6:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"']'");
		break;
	case UI::GUIControl::GK_OEM_7:
		sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"[']");
		break;
	case UI::GUIControl::GK_NONE:
		break;
	}
	return sbuff;
}

UI::GUIMenu::GUIMenu(Bool isPopup)
{
	NEW_CLASS(this->subMenus, Data::ArrayList<UI::GUIMenu*>());
	NEW_CLASS(this->keys, Data::ArrayList<ShortcutKey*>());
	this->hdpi = 96.0;

	if (isPopup)
	{
		this->hMenu = CreatePopupMenu();
	}
	else
	{
		this->hMenu = CreateMenu();
	}
	this->itemCnt = 0;
	this->mnuForm = 0;
}

UI::GUIMenu::~GUIMenu()
{
	UI::GUIMenu *item;
	UOSInt i;
	i = this->keys->GetCount();
	while (i-- > 0)
	{
		MemFree(this->keys->GetItem(i));
	}
	DEL_CLASS(this->keys);
	i = this->subMenus->GetCount();
	while (i-- > 0)
	{
		item = this->subMenus->GetItem(i);
		DEL_CLASS(item);
	}
	DEL_CLASS(this->subMenus);
	DestroyMenu((HMENU)this->hMenu);
}

UOSInt UI::GUIMenu::AddItem(const UTF8Char *name, UInt16 cmdId, KeyModifier keyModifier, UI::GUIControl::GUIKey shortcutKey)
{
	UOSInt id = this->itemCnt++;
	if (shortcutKey)
	{
		WChar sbuff[256];
		UTF8Char u8buff[64];
		WChar *sptr;
		sptr = Text::StrUTF8_WChar(sbuff, name, 0);
		sptr = Text::StrConcat(sptr, L"\t");
		ToKeyDisplay(u8buff, keyModifier, shortcutKey);
		sptr = Text::StrUTF8_WChar(sptr, u8buff, 0);
		AppendMenuW((HMENU)this->hMenu, 0, cmdId, sbuff);
		ShortcutKey *key = MemAlloc(ShortcutKey, 1);
		key->cmdId = cmdId;
		key->keyModifier = keyModifier;
		key->shortcutKey = shortcutKey;
		this->keys->Add(key);
	}
	else
	{
		const WChar *wptr = Text::StrToWCharNew(name);
		AppendMenuW((HMENU)this->hMenu, 0, cmdId, wptr);
		Text::StrDelNew(wptr);
	}
	return id;
}

void UI::GUIMenu::AddSeperator()
{
	UOSInt id = this->itemCnt++;
	AppendMenuW((HMENU)this->hMenu, MF_SEPARATOR, id, 0);
}

UI::GUIMenu *UI::GUIMenu::AddSubMenu(const UTF8Char *name)
{
	UI::GUIMenu *subMenu;
	NEW_CLASS(subMenu, UI::GUIMenu(true));
	this->subMenus->Add(subMenu);
	
	const WChar *wptr = Text::StrToWCharNew(name);
	AppendMenuW((HMENU)this->hMenu, MF_POPUP, (UOSInt)subMenu->hMenu, wptr);
	Text::StrDelNew(wptr);

	return subMenu;
}

void *UI::GUIMenu::GetHMenu()
{
	return this->hMenu;
}

UOSInt UI::GUIMenu::GetAllKeys(Data::ArrayList<ShortcutKey*> *keys)
{
	UOSInt keyCnt = this->keys->GetCount();
	keys->AddAll(this->keys);
	UOSInt j = this->subMenus->GetCount();
	UOSInt i = 0;
	while (i < j)
	{
		keyCnt += this->subMenus->GetItem(i)->GetAllKeys(keys);
		i++;
	}
	return keyCnt;
}

void UI::GUIMenu::SetItemEnabled(UInt16 cmdId, Bool enabled)
{
	EnableMenuItem((HMENU)this->hMenu, cmdId, enabled?MF_ENABLED:MF_GRAYED);
}

void UI::GUIMenu::ClearItems()
{
	UI::GUIMenu *item;
	UOSInt i;
	i = this->keys->GetCount();
	while (i-- > 0)
	{
		MemFree(this->keys->GetItem(i));
	}
	this->keys->Clear();
	i = this->subMenus->GetCount();
	while (i-- > 0)
	{
		item = this->subMenus->GetItem(i);
		DEL_CLASS(item);
	}
	this->subMenus->Clear();

#ifdef _WIN32_WCE
	i = 0;
	while (DeleteMenu((HMENU)this->hMenu, (UInt32)i++, MF_BYPOSITION) != 0);
#else
	i = (UInt32)GetMenuItemCount((HMENU)this->hMenu);
	while (i-- > 0)
	{
		DeleteMenu((HMENU)this->hMenu, (UInt32)i, MF_BYPOSITION);
	}
#endif
}

void UI::GUIMenu::SetDPI(Double hdpi, Double ddpi)
{
	this->hdpi = hdpi;
	this->ddpi = ddpi;
}

void UI::GUIMenu::SetMenuForm(GUIForm *mnuForm)
{
	this->mnuForm = mnuForm;
}
