#ifndef _SM_UI_WIN_WINCHECKBOX
#define _SM_UI_WIN_WINCHECKBOX
#include "UI/GUICheckBox.h"

namespace UI
{
	namespace Win
	{
		class WinCheckBox : public GUICheckBox
		{
		private:
			Bool checked;
		public:
			WinCheckBox(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Text::CStringNN initText, Bool checked);
			virtual ~WinCheckBox();

			virtual OSInt OnNotify(UInt32 code, void *lParam);
			virtual Bool IsChecked();
			virtual void SetChecked(Bool checked);
		};
	}
}
#endif
