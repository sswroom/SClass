#ifndef _SM_UI_GTK_GTKLABEL
#define _SM_UI_GTK_GTKLABEL
#include "UI/GUILabel.h"

namespace UI
{
	namespace GTK
	{
		class GTKLabel : public UI::GUILabel
		{
		private:
			Bool hasTextColor;
			UInt32 textColor;
		public:
			GTKLabel(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN initText);
			virtual ~GTKLabel();

			virtual void SetText(Text::CStringNN text);
			virtual OSInt OnNotify(UInt32 code, void *lParam);
			virtual Bool HasTextColor();
			virtual UInt32 GetTextColor();
			virtual void SetTextColor(UInt32 textColor);
		};
	}
}

#endif
