#ifndef _SM_UI_WIN_WINPROGRESSBAR
#define _SM_UI_WIN_WINPROGRESSBAR
#include "UI/GUIProgressBar.h"

namespace UI
{
	namespace Win
	{
		class WinProgressBar : public GUIProgressBar
		{
		private:
			UInt64 totalCnt;
		public:
			WinProgressBar(NN<GUICore> ui, NN<UI::GUIClientControl> parent, UInt64 totalCnt);
			virtual ~WinProgressBar();

			virtual OSInt OnNotify(UInt32 code, void *lParam);

			virtual void ProgressStart(Text::CString name, UInt64 count);
			virtual void ProgressUpdate(UInt64 currCount, UInt64 newTotalCount);
			virtual void ProgressEnd();
		};
	}
}
#endif
