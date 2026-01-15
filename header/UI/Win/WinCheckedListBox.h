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

			virtual Bool GetItemChecked(UIntOS index);
			virtual void SetItemChecked(UIntOS index, Bool isChecked);

			virtual IntOS OnNotify(UInt32 code, void *lParam);
		};
	}
}
#endif
