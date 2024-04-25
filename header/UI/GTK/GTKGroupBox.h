#ifndef _SM_UI_GTK_GTKGROUPBOX
#define _SM_UI_GTK_GTKGROUPBOX
#include "UI/GUIGroupBox.h"

namespace UI
{
	namespace GTK
	{
		class GTKGroupBox : public GUIGroupBox
		{
		private:
			void *oriWndProc;

			static OSInt __stdcall GBWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam);
		public:
			GTKGroupBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN name);
			virtual ~GTKGroupBox();

			virtual Math::Coord2DDbl GetClientOfst();
			virtual Math::Size2DDbl GetClientSize();
		};
	}
}
#endif
