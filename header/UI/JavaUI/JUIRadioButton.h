#ifndef _SM_UI_JAVAUI_JUIRADIOBUTTON
#define _SM_UI_JAVAUI_JUIRADIOBUTTON
#include "UI/GUIRadioButton.h"

namespace UI
{
	namespace JavaUI
	{
		class JUIRadioButton : public GUIRadioButton
		{
		private:
			Bool selected;

			void ChangeSelected(Bool selVal);
		public:
			JUIRadioButton(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN initText, Bool selected);
			virtual ~JUIRadioButton();

			virtual IntOS OnNotify(UInt32 code, void *lParam);
			virtual Bool IsSelected();
			virtual void Select();
		};
	}
}
#endif
