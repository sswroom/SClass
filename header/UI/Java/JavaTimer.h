#ifndef _SM_UI_JAVA_JAVATIMER
#define _SM_UI_JAVA_JAVATIMER
#include "UI/GUITimer.h"
#include <jni.h>

namespace UI
{
	namespace Java
	{
		class JavaTimer : public GUITimer
		{
		private:
			jobject tmr;
			UInt32 interval;

		public:
			JavaTimer(UInt32 interval, UI::UIEvent handler, AnyType userObj);
			virtual ~JavaTimer();
		};
	}
}
#endif
