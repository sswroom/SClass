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
		Int64 totalCnt;
	public:
		GUIProgressBar(GUICore *ui, UI::GUIClientControl *parent, Int64 totalCnt);
		virtual ~GUIProgressBar();

		virtual const UTF8Char *GetObjectClass();
		virtual OSInt OnNotify(Int32 code, void *lParam);

		virtual void ProgressStart(const UTF8Char *name, Int64 count);
		virtual void ProgressUpdate(Int64 currCount, Int64 newTotalCount);
		virtual void ProgressEnd();
	};
}
#endif
