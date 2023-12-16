#ifndef _SM_UI_JAVA_JAVAMESSAGEDIALOG
#define _SM_UI_JAVA_JAVAMESSAGEDIALOG
#include "UI/GUIControl.h"
#include <jni.h>

namespace UI
{
	namespace Java
	{
		class JavaMessageDialog
		{
		public:
			static void ShowOK(JNIEnv *env, Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl);
			static Bool ShowYesNo(JNIEnv *env, Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl);
		};
	}
}
#endif
