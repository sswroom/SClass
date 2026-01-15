#ifndef _SM_UI_WIN_WINLABEL
#define _SM_UI_WIN_WINLABEL
#include "UI/GUILabel.h"

namespace UI
{
	namespace Win
	{
		class WinLabel : public UI::GUILabel
		{
		private:
			Bool hasTextColor;
			UInt32 textColor;
		public:
			WinLabel(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN initText);
			virtual ~WinLabel();

			virtual void SetText(Text::CStringNN text);
			virtual IntOS OnNotify(UInt32 code, void *lParam);
			virtual Bool HasTextColor();
			virtual UInt32 GetTextColor();
			virtual void SetTextColor(UInt32 textColor);
		};
	}
}

#endif
