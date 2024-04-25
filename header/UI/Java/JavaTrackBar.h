#ifndef _SM_UI_JAVA_JAVATRACKBAR
#define _SM_UI_JAVA_JAVATRACKBAR
#include "UI/GUITrackBar.h"

namespace UI
{
	namespace Java
	{
		class JavaTrackBar : public GUITrackBar
		{
		public:
			JavaTrackBar(NN<GUICore> ui, NN<UI::GUIClientControl> parent, UOSInt minVal, UOSInt maxVal, UOSInt currVal);
			virtual ~JavaTrackBar();

			virtual OSInt OnNotify(UInt32 code, void *lParam);
			virtual void SetPos(UOSInt pos);
			virtual void SetRange(UOSInt minVal, UOSInt maxVal);
			virtual UOSInt GetPos();
		};
	}
}
#endif
