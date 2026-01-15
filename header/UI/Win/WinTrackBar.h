#ifndef _SM_UI_WIN_WINTRACKBAR
#define _SM_UI_WIN_WINTRACKBAR
#include "UI/GUITrackBar.h"

namespace UI
{
	namespace Win
	{
		class WinTrackBar : public GUITrackBar
		{
		public:
			WinTrackBar(NN<GUICore> ui, NN<UI::GUIClientControl> parent, UIntOS minVal, UIntOS maxVal, UIntOS currVal);
			virtual ~WinTrackBar();

			virtual IntOS OnNotify(UInt32 code, void *lParam);
			virtual void SetPos(UIntOS pos);
			virtual void SetRange(UIntOS minVal, UIntOS maxVal);
			virtual UIntOS GetPos();
		};
	}
}
#endif
