#ifndef _SM_UI_GUIRADIOBUTTON
#define _SM_UI_GUIRADIOBUTTON
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUIRadioButton : public GUIControl
	{
	public:
		typedef void (__stdcall *SelectedChangeHandler)(void *userObj, Bool newState);
	private:
		Data::ArrayList<SelectedChangeHandler> selectedChangeHdlrs;
		Data::ArrayList<void*> selectedChangeObjs;

	public:
		GUIRadioButton(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent);
		virtual ~GUIRadioButton();

		virtual Text::CStringNN GetObjectClass() const;
		void EventSelectedChange(Bool newState);
		void HandleSelectedChange(SelectedChangeHandler hdlr, void *userObj);

		virtual OSInt OnNotify(UInt32 code, void *lParam) = 0;
		virtual Bool IsSelected() = 0;
		virtual void Select() = 0;
	};
}

#endif
