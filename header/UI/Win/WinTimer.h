#ifndef _SM_UI_WIN_WINTIMER
#define _SM_UI_WIN_WINTIMER
#include "UI/GUITimer.h"

namespace UI
{
	namespace Win
	{
		class WinTimer : public GUITimer
		{
		private:
			NN<UI::GUIForm> parent;
			UIntOS id;
			UInt32 interval;

		public:
			WinTimer(NN<UI::GUIForm> parent, UIntOS id, UInt32 interval, UI::UIEvent handler, AnyType userObj);
			virtual ~WinTimer();

			UIntOS GetId();
		};
	}
}
#endif
