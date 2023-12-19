#ifndef _SM_UI_GUICHECKEDLISTBOX
#define _SM_UI_GUICHECKEDLISTBOX
#include "UI/GUIListView.h"

namespace UI
{
	class GUICheckedListBox : public GUIListView
	{
	public:
		GUICheckedListBox(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent);
		virtual ~GUICheckedListBox();

		virtual Text::CStringNN GetObjectClass() const;

		virtual Bool GetItemChecked(UOSInt index) = 0;
		virtual void SetItemChecked(UOSInt index, Bool isChecked) = 0;
		virtual OSInt OnNotify(UInt32 code, void *lParam) = 0;
	};
}
#endif
