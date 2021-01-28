#ifndef _SM_UI_GUILABEL
#define _SM_UI_GUILABEL
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUILabel : public GUIControl
	{
	private:
		Bool hasTextColor;
		Int32 textColor;
	public:
		GUILabel(GUICore *ui, UI::GUIClientControl *parent, const UTF8Char *initText);
		virtual ~GUILabel();

		virtual void SetText(const UTF8Char *text);
		virtual const UTF8Char *GetObjectClass();
		virtual OSInt OnNotify(Int32 code, void *lParam);
		Bool HasTextColor();
		Int32 GetTextColor();
		void SetTextColor(Int32 textColor);
	};
};

#endif
