#ifndef _SM_UI_GUIRADIOBUTTON
#define _SM_UI_GUIRADIOBUTTON
#include "AnyType.h"
#include "Data/CallbackStorage.h"
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUIRadioButton : public GUIControl
	{
	public:
		typedef void (__stdcall *SelectedChangeHandler)(AnyType userObj, Bool newState);
	private:
		Data::ArrayList<Data::CallbackStorage<SelectedChangeHandler>> selectedChangeHdlrs;

	public:
		GUIRadioButton(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent);
		virtual ~GUIRadioButton();

		virtual Text::CStringNN GetObjectClass() const;
		void EventSelectedChange(Bool newState);
		void HandleSelectedChange(SelectedChangeHandler hdlr, AnyType userObj);

		virtual OSInt OnNotify(UInt32 code, void *lParam) = 0;
		virtual Bool IsSelected() = 0;
		virtual void Select() = 0;
	};
}

#endif
