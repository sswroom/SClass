#ifndef _SM_UI_GUIPROGRESSBAR
#define _SM_UI_GUIPROGRESSBAR
#include "UI/GUIClientControl.h"
#include "IO/ProgressHandler.h"

namespace UI
{
	class GUIProgressBar : public GUIControl, public IO::ProgressHandler
	{
	private:
		void *clsData;
		UInt64 totalCnt;
	public:
		GUIProgressBar(NotNullPtr<GUICore> ui, UI::GUIClientControl *parent, UInt64 totalCnt);
		virtual ~GUIProgressBar();

		virtual Text::CStringNN GetObjectClass() const;
		virtual OSInt OnNotify(UInt32 code, void *lParam);

		virtual void ProgressStart(Text::CString name, UInt64 count);
		virtual void ProgressUpdate(UInt64 currCount, UInt64 newTotalCount);
		virtual void ProgressEnd();
	};
}
#endif
