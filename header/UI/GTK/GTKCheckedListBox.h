#ifndef _SM_UI_GTK_GTKCHECKEDLISTBOX
#define _SM_UI_GTK_GTKCHECKEDLISTBOX
#include "UI/GUICheckedListBox.h"

namespace UI
{
	namespace GTK
	{
		class GTKCheckedListBox : public GUICheckedListBox
		{
		public:
			GTKCheckedListBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent);
			virtual ~GTKCheckedListBox();

			virtual Bool GetItemChecked(UOSInt index);
			virtual void SetItemChecked(UOSInt index, Bool isChecked);

			virtual OSInt OnNotify(UInt32 code, void *lParam);
		};
	}
}
#endif
