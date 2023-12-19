#ifndef _SM_UI_GUICHECKBOX
#define _SM_UI_GUICHECKBOX
#include "Data/ArrayList.h"
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUICheckBox : public GUIControl
	{
	public:
		typedef void (__stdcall *CheckedChangeHandler)(void *userObj, Bool newState);
	protected:
		Bool checked;
		Data::ArrayList<CheckedChangeHandler> checkedChangeHdlrs;
		Data::ArrayList<void*> checkedChangeObjs;
	public:
		GUICheckBox(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent);
		virtual ~GUICheckBox();

		virtual Text::CStringNN GetObjectClass() const;
		void EventCheckedChange(Bool newState);
		void HandleCheckedChange(CheckedChangeHandler hdlr, void *obj);

		virtual OSInt OnNotify(UInt32 code, void *lParam) = 0;
		virtual Bool IsChecked() = 0;
		virtual void SetChecked(Bool checked) = 0;
	};
}
#endif
