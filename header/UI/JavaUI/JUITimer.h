#ifndef _SM_UI_JAVAUI_JUITIMER
#define _SM_UI_JAVAUI_JUITIMER
#include "UI/GUITimer.h"
#include <jni.h>

namespace UI
{
	namespace JavaUI
	{
		class JUITimer : public GUITimer
		{
		private:
			jobject tmr;
			UInt32 interval;

		public:
			JUITimer(UInt32 interval, UI::UIEvent handler, AnyType userObj);
			virtual ~JUITimer();
		};
	}
}
#endif
