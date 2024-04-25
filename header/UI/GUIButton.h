#ifndef _SM_UI_GUIBUTTON
#define _SM_UI_GUIBUTTON
#include "AnyType.h"
#include "Data/CallbackStorage.h"
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUIButton : public GUIControl
	{
	public:
		typedef void (__stdcall *UpDownEvent)(AnyType userObj, Bool isDown);
	private:
		Data::ArrayList<Data::CallbackStorage<UIEvent>> btnClkHandlers;
		Data::ArrayList<Data::CallbackStorage<UpDownEvent>> btnUpDownHandlers;
		Data::ArrayList<Data::CallbackStorage<UIEvent>> btnFocusLostHandlers;
	public:
		GUIButton(NN<GUICore> ui, NN<UI::GUIClientControl> parent);
		virtual ~GUIButton();

		virtual void SetText(Text::CStringNN text) = 0;
		virtual void SetFont(const UTF8Char *name, UOSInt nameLen, Double fontHeightPt, Bool isBold) = 0;
		virtual OSInt OnNotify(UInt32 code, void *lParam) = 0;

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
