#ifndef _SM_UI_GUICHECKBOX
#define _SM_UI_GUICHECKBOX
#include "AnyType.h"
#include "Data/ArrayListObj.hpp"
#include "Data/CallbackStorage.h"
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUICheckBox : public GUIControl
	{
	public:
		typedef void (CALLBACKFUNC CheckedChangeHandler)(AnyType userObj, Bool newState);
	protected:
		Bool checked;
		Data::ArrayListObj<Data::CallbackStorage<CheckedChangeHandler>> checkedChangeHdlrs;
	public:
		GUICheckBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent);
		virtual ~GUICheckBox();

		virtual Text::CStringNN GetObjectClass() const;
		void EventCheckedChange(Bool newState);
		void HandleCheckedChange(CheckedChangeHandler hdlr, AnyType obj);

		virtual IntOS OnNotify(UInt32 code, void *lParam) = 0;
		virtual Bool IsChecked() = 0;
		virtual void SetChecked(Bool checked) = 0;
	};
}
#endif
