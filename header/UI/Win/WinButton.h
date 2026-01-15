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
			IntOS btnId;
			void *oriWndProc;
			static IntOS nextId;

			static IntOS __stdcall BTNWndProc(void *hWnd, UInt32 msg, UInt32 wParam, IntOS lParam);
		public:
			WinButton(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN txt);
			virtual ~WinButton();

			virtual void SetText(Text::CStringNN text);
			virtual void SetFont(Text::CString fontName, Double fontHeightPt, Bool isBold);

			virtual IntOS OnNotify(UInt32 code, void *lParam);
			IntOS GetBtnId();
			void SetDefaultBtnLook();
		};
	}
}
#endif
