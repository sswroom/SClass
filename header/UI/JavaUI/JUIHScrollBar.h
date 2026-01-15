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

			virtual void InitScrollBar(UIntOS minVal, UIntOS maxVal, UIntOS currVal, UIntOS largeChg);
			virtual void SetPos(UIntOS pos);
			virtual UIntOS GetPos();
			virtual void SetArea(Double left, Double top, Double right, Double bottom, Bool updateScn);
			virtual void SetAreaP(IntOS left, IntOS top, IntOS right, IntOS bottom, Bool updateScn);
			virtual IntOS OnNotify(UInt32 code, void *lParam);
			virtual void UpdatePos(Bool redraw);
		};
	}
}
#endif
