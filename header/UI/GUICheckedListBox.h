#ifndef _SM_UI_GUICHECKEDLISTBOX
#define _SM_UI_GUICHECKEDLISTBOX
#include "UI/GUIListView.h"

namespace UI
{
	class GUICheckedListBox : public GUIListView
	{
	public:
		GUICheckedListBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent);
		virtual ~GUICheckedListBox();

		virtual Text::CStringNN GetObjectClass() const;

		virtual Bool GetItemChecked(UIntOS index) = 0;
		virtual void SetItemChecked(UIntOS index, Bool isChecked) = 0;
		virtual IntOS OnNotify(UInt32 code, void *lParam) = 0;
	};
}
#endif
