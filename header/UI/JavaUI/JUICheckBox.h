#ifndef _SM_UI_JAVAUI_JUICHECKBOX
#define _SM_UI_JAVAUI_JUICHECKBOX
#include "UI/GUICheckBox.h"

namespace UI
{
	namespace JavaUI
	{
		class JUICheckBox : public GUICheckBox
		{
		public:
			JUICheckBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN initText, Bool checked);
			virtual ~JUICheckBox();

			virtual IntOS OnNotify(UInt32 code, void *lParam);
			virtual Bool IsChecked();
			virtual void SetChecked(Bool checked);
		};
	}
}
#endif
