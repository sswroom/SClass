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
		Data::ArrayList<GUIMenu*> subMenus;
		UOSInt itemCnt;
		Data::ArrayList<ShortcutKey*> keys;
		Data::ArrayList<void *> items;
		Double hdpi;
		Double ddpi;

		Optional<GUIForm> mnuForm;

	protected:
		void *hMenu;

		static UTF8Char *ToKeyDisplay(UTF8Char *sbuff, KeyModifier keyModifier, UI::GUIControl::GUIKey shortcutKey);
		GUIMenu(Bool isPopup);
	public:
		virtual ~GUIMenu();

		UOSInt AddItem(Text::CString name, UInt16 cmdId, KeyModifier keyModifier, UI::GUIControl::GUIKey shortcutKey);
		void AddSeperator();
		GUIMenu *AddSubMenu(Text::CString name);
		void *GetHMenu();
		UOSInt GetAllKeys(NotNullPtr<Data::ArrayList<ShortcutKey*>> keys);
		UOSInt GetShortcutKeyCnt();
		ShortcutKey *GetShortcutKey(UOSInt index);
		void SetItemEnabled(UInt16 cmdId, Bool enabled); /////////////////////////
		void ClearItems();
		void SetDPI(Double hdpi, Double ddpi);

		void SetMenuForm(Optional<GUIForm> mnuForm);
		void EventMenuClick(UInt16 cmdId);
	};
}
#endif
