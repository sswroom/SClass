#ifndef _SM_UI_WIN_WINFOLDERDIALOG
#define _SM_UI_WIN_WINFOLDERDIALOG
#include "UI/GUIFolderDialog.h"

namespace UI
{
	namespace Win
	{
		class WinFolderDialog : public UI::GUIFolderDialog
		{
		private:
			static Int32 __stdcall BrowseCB(void *hwnd, UInt32 uMsg, IntOS lParam, IntOS lpData);
		public:
			WinFolderDialog();
			virtual ~WinFolderDialog();

			virtual Bool ShowDialog(Optional<ControlHandle> ownerHandle);
		};
	}
}
#endif
