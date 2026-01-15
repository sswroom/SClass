#ifndef _SM_UI_GTK_GTKPROGRESSBAR
#define _SM_UI_GTK_GTKPROGRESSBAR
#include "UI/GUIProgressBar.h"
#include <gtk/gtk.h>

namespace UI
{
	namespace GTK
	{
		class GTKProgressBar : public GUIProgressBar
		{
		private:
			GtkProgressBar *bar;
			UInt64 currCnt;
			UInt64 totalCnt;
			UIntOS timerId;
			Bool cntUpdated;

			static Int32 SignalTick(void *userObj);
		public:
			GTKProgressBar(NN<GUICore> ui, NN<UI::GUIClientControl> parent, UInt64 totalCnt);
			virtual ~GTKProgressBar();

			virtual IntOS OnNotify(UInt32 code, void *lParam);

			virtual void ProgressStart(Text::CStringNN name, UInt64 count);
			virtual void ProgressUpdate(UInt64 currCount, UInt64 newTotalCount);
			virtual void ProgressEnd();
		};
	}
}
#endif
