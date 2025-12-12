#ifndef _SM_UI_JAVAUI_JUIHSCROLLBAR
#define _SM_UI_JAVAUI_JUIHSCROLLBAR
#include "UI/GUIHScrollBar.h"

namespace UI
{
	namespace JavaUI
	{
		class JUIHScrollBar : public GUIHScrollBar
		{
		public:
			JUIHScrollBar(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Double width);
			virtual ~JUIHScrollBar();

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
