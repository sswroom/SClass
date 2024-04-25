#ifndef _SM_UI_JAVA_JAVACHECKEDLISTBOX
#define _SM_UI_JAVA_JAVACHECKEDLISTBOX
#include "UI/GUICheckedListBox.h"

namespace UI
{
	namespace Java
	{
		class JavaCheckedListBox : public GUICheckedListBox
		{
		public:
			JavaCheckedListBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent);
			virtual ~JavaCheckedListBox();

			virtual Bool GetItemChecked(UOSInt index);
			virtual void SetItemChecked(UOSInt index, Bool isChecked);

			virtual OSInt OnNotify(UInt32 code, void *lParam);
		};
	}
}
#endif
