#ifndef _SM_UI_JAVA_JAVACHECKBOX
#define _SM_UI_JAVA_JAVACHECKBOX
#include "UI/GUICheckBox.h"

namespace UI
{
	namespace Java
	{
		class JavaCheckBox : public GUICheckBox
		{
		public:
			JavaCheckBox(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Text::CStringNN initText, Bool checked);
			virtual ~JavaCheckBox();

			virtual OSInt OnNotify(UInt32 code, void *lParam);
			virtual Bool IsChecked();
			virtual void SetChecked(Bool checked);
		};
	}
}
#endif
