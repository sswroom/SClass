#ifndef _SM_UI_GTK_GTKTIMER
#define _SM_UI_GTK_GTKTIMER
#include "UI/GUITimer.h"

namespace UI
{
	namespace GTK
	{
		class GTKTimer : public GUITimer
		{
		private:
			UOSInt id;
			UInt32 interval;

			static Int32 OnTick(void *userObj);
		public:
			GTKTimer(UInt32 interval, UI::UIEvent handler, void *userObj);
			virtual ~GTKTimer();

			UOSInt GetId();
		};
	}
}
#endif
