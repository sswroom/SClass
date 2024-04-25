#ifndef _SM_UI_WIN_WINCHECKEDLISTBOX
#define _SM_UI_WIN_WINCHECKEDLISTBOX
#include "UI/GUICheckedListBox.h"

namespace UI
{
	namespace Win
	{
		class WinCheckedListBox : public GUICheckedListBox
		{
		public:
			WinCheckedListBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent);
			virtual ~WinCheckedListBox();

			virtual Bool GetItemChecked(UOSInt index);
			virtual void SetItemChecked(UOSInt index, Bool isChecked);

			virtual OSInt OnNotify(UInt32 code, void *lParam);
		};
	}
}
#endif
