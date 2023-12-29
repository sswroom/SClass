#ifndef _SM_UI_WIN_WINRADIOBUTTON
#define _SM_UI_WIN_WINRADIOBUTTON
#include "UI/GUIRadioButton.h"

namespace UI
{
	namespace Win
	{
		class WinRadioButton : public GUIRadioButton
		{
		private:
			Bool selected;

			void ChangeSelected(Bool selVal);
		public:
			WinRadioButton(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Text::CStringNN initText, Bool selected);
			virtual ~WinRadioButton();

			virtual OSInt OnNotify(UInt32 code, void *lParam);
			virtual Bool IsSelected();
			virtual void Select();
		};
	}
}
#endif
