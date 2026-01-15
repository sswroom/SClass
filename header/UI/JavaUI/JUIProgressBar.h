#ifndef _SM_UI_JAVAUI_JUIPROGRESSBAR
#define _SM_UI_JAVAUI_JUIPROGRESSBAR
#include "UI/GUIProgressBar.h"

namespace UI
{
	namespace JavaUI
	{
		class JUIProgressBar : public GUIProgressBar
		{
		private:
			UInt64 totalCnt;

		public:
			JUIProgressBar(NN<GUICore> ui, NN<UI::GUIClientControl> parent, UInt64 totalCnt);
			virtual ~JUIProgressBar();

			virtual IntOS OnNotify(UInt32 code, void *lParam);

			virtual void ProgressStart(Text::CStringNN name, UInt64 count);
			virtual void ProgressUpdate(UInt64 currCount, UInt64 newTotalCount);
			virtual void ProgressEnd();
		};
	}
}
#endif
