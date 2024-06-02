#ifndef _SM_UI_JAVA_JAVABUTTON
#define _SM_UI_JAVA_JAVABUTTON
#include "UI/GUIButton.h"

namespace UI
{
	namespace Java
	{
		class JavaButton : public UI::GUIButton
		{
		public:
			JavaButton(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN txt);
			virtual ~JavaButton();

			virtual void SetText(Text::CStringNN text);
			virtual void SetFont(UnsafeArrayOpt<const UTF8Char> name, UOSInt nameLen, Double fontHeightPt, Bool isBold);

			virtual OSInt OnNotify(UInt32 code, void *lParam);
		};
	}
}
#endif
