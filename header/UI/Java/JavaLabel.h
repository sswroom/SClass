#ifndef _SM_UI_JAVA_JAVALABEL
#define _SM_UI_JAVA_JAVALABEL
#include "UI/GUILabel.h"

namespace UI
{
	namespace Java
	{
		class JavaLabel : public UI::GUILabel
		{
		private:
			Bool hasTextColor;
			UInt32 textColor;
		public:
			JavaLabel(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Text::CStringNN initText);
			virtual ~JavaLabel();

			virtual void SetText(Text::CStringNN text);
			virtual OSInt OnNotify(UInt32 code, void *lParam);
			Bool HasTextColor();
			UInt32 GetTextColor();
			void SetTextColor(UInt32 textColor);
		};
	}
}

#endif
