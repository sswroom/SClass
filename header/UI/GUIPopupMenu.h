#ifndef _SM_UI_GUIPOPUPMENU
#define _SM_UI_GUIPOPUPMENU
#include "UI/GUIMenu.h"
#include "UI/GUIForm.h"

namespace UI
{
	class GUIPopupMenu : public GUIMenu
	{
	private:
		Math::Coord2D<IntOS> scnPos;
	public:
		GUIPopupMenu();
		void ShowMenu(NN<UI::GUIControl> ctrl, Math::Coord2D<IntOS> scnPos);
		Math::Coord2D<IntOS> GetScnPos() const { return this->scnPos; }
	};
}
#endif
