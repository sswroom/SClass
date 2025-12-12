#ifndef _SM_UI_JAVAUI_JUIFOLDERDIALOG
#define _SM_UI_JAVAUI_JUIFOLDERDIALOG
#include "UI/GUIFolderDialog.h"

namespace UI
{
	namespace JavaUI
	{
		class JUIFolderDialog : public UI::GUIFolderDialog
		{
		public:
			JUIFolderDialog();
			virtual ~JUIFolderDialog();

			virtual Bool ShowDialog(Optional<ControlHandle> ownerHandle);
		};
	}
}
#endif
