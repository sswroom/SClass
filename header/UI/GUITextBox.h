#ifndef _SM_UI_GUITEXTBOX
#define _SM_UI_GUITEXTBOX
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUITextBox : public GUIControl
	{
	private:
		Data::ArrayList<Data::CallbackStorage<UI::UIEvent>> txtChgHdlrs;
		Data::ArrayList<Data::CallbackStorage<UI::KeyEvent>> keyDownHdlrs;

	public:
		GUITextBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent);
		virtual ~GUITextBox();

		virtual Text::CStringNN GetObjectClass() const;
		virtual void EventTextChange();
		virtual UI::EventState EventKeyDown(UInt32 osKey);
		virtual void HandleTextChanged(UI::UIEvent hdlr, AnyType userObj);
		virtual void HandleKeyDown(UI::KeyEvent hdlr, AnyType userObj);

		virtual void SetReadOnly(Bool isReadOnly) = 0;
		virtual void SetPasswordChar(UTF32Char c) = 0;

		virtual void SetText(Text::CStringNN text) = 0;
		virtual UnsafeArrayOpt<UTF8Char> GetText(UnsafeArray<UTF8Char> buff) = 0;
		virtual Bool GetText(NN<Text::StringBuilderUTF8> sb) = 0;
		virtual OSInt OnNotify(UInt32 code, void *lParam) = 0;

		virtual void SetWordWrap(Bool wordWrap) = 0;
		virtual void SelectAll() = 0;
	};
}
#endif
