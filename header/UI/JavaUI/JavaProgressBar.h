#ifndef _SM_UI_JAVAUI_JAVAPROGRESSBAR
#define _SM_UI_JAVAUI_JAVAPROGRESSBAR
#include "UI/GUIProgressBar.h"

namespace UI
{
	namespace JavaUI
	{
		class JavaProgressBar : public GUIProgressBar
		{
		private:
			UInt64 totalCnt;

		public:
			JavaProgressBar(NN<GUICore> ui, NN<UI::GUIClientControl> parent, UInt64 totalCnt);
			virtual ~JavaProgressBar();

			virtual OSInt OnNotify(UInt32 code, void *lParam);

			virtual void ProgressStart(Text::CStringNN name, UInt64 count);
			virtual void ProgressUpdate(UInt64 currCount, UInt64 newTotalCount);
			virtual void ProgressEnd();
		};
	}
}
#endif
