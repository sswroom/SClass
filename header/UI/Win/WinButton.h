#ifndef _SM_UI_WIN_WINBUTTON
#define _SM_UI_WIN_WINBUTTON
#include "UI/GUIButton.h"

namespace UI
{
	namespace Win
	{
		class WinButton : public UI::GUIButton
		{
		private:
			OSInt btnId;
			void *oriWndProc;
			static OSInt nextId;

			static OSInt __stdcall BTNWndProc(void *hWnd, UInt32 msg, UInt32 wParam, OSInt lParam);
		public:
			WinButton(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN txt);
			virtual ~WinButton();

			virtual void SetText(Text::CStringNN text);
			virtual void SetFont(UnsafeArrayOpt<const UTF8Char> name, UOSInt nameLen, Double fontHeightPt, Bool isBold);

			virtual OSInt OnNotify(UInt32 code, void *lParam);
			OSInt GetBtnId();
			void SetDefaultBtnLook();
		};
	}
}
#endif
