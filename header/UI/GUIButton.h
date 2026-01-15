#ifndef _SM_UI_GUIBUTTON
#define _SM_UI_GUIBUTTON
#include "AnyType.h"
#include "Data/ArrayListObj.hpp"
#include "Data/CallbackStorage.h"
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUIButton : public GUIControl
	{
	public:
		typedef void (CALLBACKFUNC UpDownEvent)(AnyType userObj, Bool isDown);
	private:
		Data::ArrayListObj<Data::CallbackStorage<UIEvent>> btnClkHandlers;
		Data::ArrayListObj<Data::CallbackStorage<UpDownEvent>> btnUpDownHandlers;
		Data::ArrayListObj<Data::CallbackStorage<UIEvent>> btnFocusLostHandlers;
	public:
		GUIButton(NN<GUICore> ui, NN<UI::GUIClientControl> parent);
		virtual ~GUIButton();

		virtual void SetText(Text::CStringNN text) = 0;
		virtual void SetFont(Text::CString fontName, Double fontHeightPt, Bool isBold) = 0;
		virtual IntOS OnNotify(UInt32 code, void *lParam) = 0;

		virtual Text::CStringNN GetObjectClass() const;

		void EventFocus();
		void EventFocusLost();
		void EventButtonClick();
		void EventButtonDown();
		void EventButtonUp();

		void HandleFocusLost(UIEvent handler, AnyType userObj);
		void HandleButtonClick(UIEvent handler, AnyType userObj);
		void HandleButtonUpDown(UpDownEvent handler, AnyType userObj);
	};
}
#endif
