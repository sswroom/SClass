#ifndef _SM_UI_WIN_WINFILEDIALOG
#define _SM_UI_WIN_WINFILEDIALOG
#include "UI/GUIFileDialog.h"

namespace UI
{
	namespace Win
	{
		class WinFileDialog : public UI::GUIFileDialog
		{
		public:
			WinFileDialog(const WChar *compName, const WChar *appName, const WChar *dialogName, Bool isSave);
			virtual ~WinFileDialog();

			virtual Bool ShowDialog(ControlHandle *ownerHandle);
		};
	}
}
#endif
