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
			NotNullPtr<UI::GUIForm> parent;
			UOSInt id;
			UInt32 interval;

		public:
			WinTimer(NotNullPtr<UI::GUIForm> parent, UOSInt id, UInt32 interval, UI::UIEvent handler, void *userObj);
			virtual ~WinTimer();

			UOSInt GetId();
		};
	}
}
#endif