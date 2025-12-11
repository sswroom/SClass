#ifndef _SM_UI_JAVAUI_JAVACHECKBOX
#define _SM_UI_JAVAUI_JAVACHECKBOX
#include "UI/GUICheckBox.h"

namespace UI
{
	namespace JavaUI
	{
		class JavaCheckBox : public GUICheckBox
		{
		public:
			JavaCheckBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN initText, Bool checked);
			virtual ~JavaCheckBox();

			virtual OSInt OnNotify(UInt32 code, void *lParam);
			virtual Bool IsChecked();
			virtual void SetChecked(Bool checked);
		};
	}
}
#endif
