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

			virtual Bool GetItemChecked(UIntOS index);
			virtual void SetItemChecked(UIntOS index, Bool isChecked);

			virtual IntOS OnNotify(UInt32 code, void *lParam);
		};
	}
}
#endif
