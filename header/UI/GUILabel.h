#ifndef _SM_UI_GUILABEL
#define _SM_UI_GUILABEL
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUILabel : public GUIControl
	{
	private:
		Bool hasTextColor;
		UInt32 textColor;
	public:
		GUILabel(GUICore *ui, UI::GUIClientControl *parent, Text::CString initText);
		virtual ~GUILabel();

		virtual void SetText(Text::CString text);
		virtual Text::CString GetObjectClass();
		virtual OSInt OnNotify(UInt32 code, void *lParam);
		Bool HasTextColor();
		UInt32 GetTextColor();
		void SetTextColor(UInt32 textColor);
	};
}

#endif
