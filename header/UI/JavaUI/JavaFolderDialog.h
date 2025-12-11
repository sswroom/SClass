#ifndef _SM_UI_JAVAUI_JAVAFOLDERDIALOG
#define _SM_UI_JAVAUI_JAVAFOLDERDIALOG
#include "UI/GUIFolderDialog.h"

namespace UI
{
	namespace JavaUI
	{
		class JavaFolderDialog : public UI::GUIFolderDialog
		{
		public:
			JavaFolderDialog();
			virtual ~JavaFolderDialog();

			virtual Bool ShowDialog(Optional<ControlHandle> ownerHandle);
		};
	}
}
#endif
