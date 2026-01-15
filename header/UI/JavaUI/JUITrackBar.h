#ifndef _SM_UI_JAVAUI_JUITRACKBAR
#define _SM_UI_JAVAUI_JUITRACKBAR
#include "UI/GUITrackBar.h"

namespace UI
{
	namespace JavaUI
	{
		class JUITrackBar : public GUITrackBar
		{
		public:
			JUITrackBar(NN<GUICore> ui, NN<UI::GUIClientControl> parent, UIntOS minVal, UIntOS maxVal, UIntOS currVal);
			virtual ~JUITrackBar();

			virtual IntOS OnNotify(UInt32 code, void *lParam);
			virtual void SetPos(UIntOS pos);
			virtual void SetRange(UIntOS minVal, UIntOS maxVal);
			virtual UIntOS GetPos();
		};
	}
}
#endif
