#ifndef _SM_UI_GUICHECKBOX
#define _SM_UI_GUICHECKBOX
#include "AnyType.h"
#include "Data/ArrayList.h"
#include "Data/CallbackStorage.h"
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUICheckBox : public GUIControl
	{
	public:
		typedef void (__stdcall *CheckedChangeHandler)(AnyType userObj, Bool newState);
	protected:
		Bool checked;
		Data::ArrayList<Data::CallbackStorage<CheckedChangeHandler>> checkedChangeHdlrs;
	public:
		GUICheckBox(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent);
		virtual ~GUICheckBox();

		virtual Text::CStringNN GetObjectClass() const;
		void EventCheckedChange(Bool newState);
		void HandleCheckedChange(CheckedChangeHandler hdlr, AnyType obj);

		virtual OSInt OnNotify(UInt32 code, void *lParam) = 0;
		virtual Bool IsChecked() = 0;
		virtual void SetChecked(Bool checked) = 0;
	};
}
#endif
