#ifndef _SM_UI_GUILABEL
#define _SM_UI_GUILABEL
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUILabel : public GUIControl
	{
	public:
		GUILabel(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent);
		virtual ~GUILabel();

		virtual Text::CStringNN GetObjectClass() const;
		virtual void SetText(Text::CStringNN text) = 0;
		virtual OSInt OnNotify(UInt32 code, void *lParam) = 0;
		virtual Bool HasTextColor() = 0;
		virtual UInt32 GetTextColor() = 0;
		virtual void SetTextColor(UInt32 textColor) = 0;
	};
}

#endif
