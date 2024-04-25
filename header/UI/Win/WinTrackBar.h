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
			WinTrackBar(NN<GUICore> ui, NN<UI::GUIClientControl> parent, UOSInt minVal, UOSInt maxVal, UOSInt currVal);
			virtual ~WinTrackBar();

			virtual OSInt OnNotify(UInt32 code, void *lParam);
			virtual void SetPos(UOSInt pos);
			virtual void SetRange(UOSInt minVal, UOSInt maxVal);
			virtual UOSInt GetPos();
		};
	}
}
#endif
