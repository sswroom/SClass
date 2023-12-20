#ifndef _SM_UI_JAVA_JAVAFOLDERDIALOG
#define _SM_UI_JAVA_JAVAFOLDERDIALOG
#include "UI/GUIFolderDialog.h"

namespace UI
{
	namespace Java
	{
		class JavaFolderDialog : public UI::GUIFolderDialog
		{
		public:
			JavaFolderDialog();
			virtual ~JavaFolderDialog();

			virtual Bool ShowDialog(ControlHandle *ownerHandle);
		};
	}
}
#endif
