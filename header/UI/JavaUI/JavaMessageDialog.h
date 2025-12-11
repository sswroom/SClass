#ifndef _SM_UI_JAVAUI_JAVAMESSAGEDIALOG
#define _SM_UI_JAVAUI_JAVAMESSAGEDIALOG
#include "UI/GUIControl.h"
#include <jni.h>

namespace UI
{
	namespace JavaUI
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
