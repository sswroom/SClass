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
			JNIEnv *env;
			jobject tmr;
			UInt32 interval;

		public:
			JavaTimer(JNIEnv *env, UInt32 interval, UI::UIEvent handler, void *userObj);
			virtual ~JavaTimer();
		};
	}
}
#endif
