#ifndef _SM_UI_GUIPOPUPMENU
#define _SM_UI_GUIPOPUPMENU
#include "UI/GUIMenu.h"
#include "UI/GUIForm.h"

namespace UI
{
	class GUIPopupMenu : public GUIMenu
	{
	public:
		GUIPopupMenu();
		void ShowMenu(UI::GUIControl *ctrl, OSInt x, OSInt y);
	};
}
#endif
