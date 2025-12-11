#ifndef _SM_UI_JAVAUI_JAVALABEL
#define _SM_UI_JAVAUI_JAVALABEL
#include "Java/JavaJLabel.h"
#include "UI/GUILabel.h"

namespace UI
{
	namespace JavaUI
	{
		class JavaLabel : public UI::GUILabel
		{
		private:
			NN<::Java::JavaJLabel> lbl;
			Bool hasTextColor;
			UInt32 textColor;
		public:
			JavaLabel(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN initText);
			virtual ~JavaLabel();

			virtual void SetText(Text::CStringNN text);
			virtual OSInt OnNotify(UInt32 code, void *lParam);
			virtual Bool HasTextColor();
			virtual UInt32 GetTextColor();
			virtual void SetTextColor(UInt32 textColor);
		};
	}
}

#endif
