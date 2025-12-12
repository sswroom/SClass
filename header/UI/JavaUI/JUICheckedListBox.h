#ifndef _SM_UI_JAVAUI_JUICHECKEDLISTBOX
#define _SM_UI_JAVAUI_JUICHECKEDLISTBOX
#include "UI/GUICheckedListBox.h"

namespace UI
{
	namespace JavaUI
	{
		class JUICheckedListBox : public GUICheckedListBox
		{
		public:
			JUICheckedListBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent);
			virtual ~JUICheckedListBox();

			virtual Bool GetItemChecked(UOSInt index);
			virtual void SetItemChecked(UOSInt index, Bool isChecked);

			virtual OSInt OnNotify(UInt32 code, void *lParam);
		};
	}
}
#endif
