#ifndef _SM_UI_GUIPROGRESSBAR
#define _SM_UI_GUIPROGRESSBAR
#include "UI/GUIClientControl.h"
#include "IO/IProgressHandler.h"

namespace UI
{
	class GUIProgressBar : public GUIControl, public IO::IProgressHandler
	{
	private:
		void *clsData;
		UInt64 totalCnt;
	public:
		GUIProgressBar(GUICore *ui, UI::GUIClientControl *parent, UInt64 totalCnt);
		virtual ~GUIProgressBar();

		virtual const UTF8Char *GetObjectClass();
		virtual OSInt OnNotify(Int32 code, void *lParam);

		virtual void ProgressStart(const UTF8Char *name, UInt64 count);
		virtual void ProgressUpdate(UInt64 currCount, UInt64 newTotalCount);
		virtual void ProgressEnd();
	};
}
#endif
