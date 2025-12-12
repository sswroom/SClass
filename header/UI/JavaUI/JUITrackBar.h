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
			JUITrackBar(NN<GUICore> ui, NN<UI::GUIClientControl> parent, UOSInt minVal, UOSInt maxVal, UOSInt currVal);
			virtual ~JUITrackBar();

			virtual OSInt OnNotify(UInt32 code, void *lParam);
			virtual void SetPos(UOSInt pos);
			virtual void SetRange(UOSInt minVal, UOSInt maxVal);
			virtual UOSInt GetPos();
		};
	}
}
#endif
