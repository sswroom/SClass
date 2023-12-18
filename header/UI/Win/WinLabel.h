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
			WinLabel(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Text::CStringNN initText);
			virtual ~WinLabel();

			virtual void SetText(Text::CStringNN text);
			virtual OSInt OnNotify(UInt32 code, void *lParam);
			Bool HasTextColor();
			UInt32 GetTextColor();
			void SetTextColor(UInt32 textColor);
		};
	}
}

#endif
