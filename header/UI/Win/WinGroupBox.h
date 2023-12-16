#ifndef _SM_UI_WIN_WINGROUPBOX
#define _SM_UI_WIN_WINGROUPBOX
#include "UI/GUIGroupBox.h"

namespace UI
{
	namespace Win
	{
		class WinGroupBox : public GUIGroupBox
		{
		private:
			void *oriWndProc;

			static OSInt __stdcall GBWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam);
		public:
			WinGroupBox(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Text::CStringNN name);
			virtual ~WinGroupBox();

			virtual Math::Coord2DDbl GetClientOfst();
			virtual Math::Size2DDbl GetClientSize();
		};
	}
}
#endif
