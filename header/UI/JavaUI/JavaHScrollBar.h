#ifndef _SM_UI_JAVAUI_JAVAHSCROLLBAR
#define _SM_UI_JAVAUI_JAVAHSCROLLBAR
#include "UI/GUIHScrollBar.h"

namespace UI
{
	namespace JavaUI
	{
		class JavaHScrollBar : public GUIHScrollBar
		{
		public:
			JavaHScrollBar(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Double width);
			virtual ~JavaHScrollBar();

			virtual void InitScrollBar(UOSInt minVal, UOSInt maxVal, UOSInt currVal, UOSInt largeChg);
			virtual void SetPos(UOSInt pos);
			virtual UOSInt GetPos();
			virtual void SetArea(Double left, Double top, Double right, Double bottom, Bool updateScn);
			virtual void SetAreaP(OSInt left, OSInt top, OSInt right, OSInt bottom, Bool updateScn);
			virtual OSInt OnNotify(UInt32 code, void *lParam);
			virtual void UpdatePos(Bool redraw);
		};
	}
}
#endif
