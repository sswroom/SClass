#ifndef _SM_UI_JAVA_JAVARADIOBUTTON
#define _SM_UI_JAVA_JAVARADIOBUTTON
#include "UI/GUIRadioButton.h"

namespace UI
{
	namespace Java
	{
		class JavaRadioButton : public GUIRadioButton
		{
		private:
			Bool selected;

			void ChangeSelected(Bool selVal);
		public:
			JavaRadioButton(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Text::CStringNN initText, Bool selected);
			virtual ~JavaRadioButton();

			virtual OSInt OnNotify(UInt32 code, void *lParam);
			virtual Bool IsSelected();
			virtual void Select();
		};
	}
}
#endif
