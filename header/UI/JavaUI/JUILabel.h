#ifndef _SM_UI_JAVAUI_JUILABEL
#define _SM_UI_JAVAUI_JUILABEL
#include "Java/JavaJLabel.h"
#include "UI/GUILabel.h"

namespace UI
{
	namespace JavaUI
	{
		class JUILabel : public UI::GUILabel
		{
		private:
			NN<Java::JavaJLabel> lbl;
			Bool hasTextColor;
			UInt32 textColor;
		public:
			JUILabel(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN initText);
			virtual ~JUILabel();

			virtual void SetText(Text::CStringNN text);
			virtual OSInt OnNotify(UInt32 code, void *lParam);
			virtual Bool HasTextColor();
			virtual UInt32 GetTextColor();
			virtual void SetTextColor(UInt32 textColor);
		};
	}
}

#endif
