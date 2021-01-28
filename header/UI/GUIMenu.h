#ifndef _SM_UI_GUIMENU
#define _SM_UI_GUIMENU
#include "Data/ArrayList.h"
#include "UI/GUIControl.h"

namespace UI
{
	class GUIForm;

	class GUIMenu
	{
	public:
		typedef enum
		{
			KM_NONE = 0,
			KM_ALT = 1,
			KM_CONTROL = 2,
			KM_SHIFT = 4,
			KM_WIN = 8
		} KeyModifier;

		typedef struct
		{
			KeyModifier keyModifier;
			UI::GUIControl::GUIKey shortcutKey;
			UInt16 cmdId;
			void *menuItem;
		} ShortcutKey;

	private:
		Data::ArrayList<GUIMenu*> *subMenus;
		UOSInt itemCnt;
		Data::ArrayList<ShortcutKey*> *keys;
		Data::ArrayList<void *> *items;
		Double hdpi;
		Double ddpi;

		GUIForm *mnuForm;

	protected:
		void *hMenu;

		static UTF8Char *ToKeyDisplay(UTF8Char *sbuff, KeyModifier keyModifier, UI::GUIControl::GUIKey shortcutKey);
		GUIMenu(Bool isPopup);
	public:
		virtual ~GUIMenu();

		UOSInt AddItem(const UTF8Char *name, UInt16 cmdId, KeyModifier keyModifier, UI::GUIControl::GUIKey shortcutKey);
		void AddSeperator();
		GUIMenu *AddSubMenu(const UTF8Char *name);
		void *GetHMenu();
		OSInt GetAllKeys(Data::ArrayList<ShortcutKey*> *keys);
		OSInt GetShortcutKeyCnt();
		ShortcutKey *GetShortcutKey(OSInt index);
		void SetItemEnabled(UInt16 cmdId, Bool enabled); /////////////////////////
		void ClearItems();
		void SetDPI(Double hdpi, Double ddpi);

		void SetMenuForm(GUIForm *mnuForm);
		void EventMenuClick(UInt16 cmdId);
	};
}
#endif
