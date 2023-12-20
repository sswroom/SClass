#ifndef _SM_UI_GTK_GTKFOLDERDIALOG
#define _SM_UI_GTK_GTKFOLDERDIALOG
#include "UI/GUIFolderDialog.h"

namespace UI
{
	namespace GTK
	{
		class GTKFolderDialog : public UI::GUIFolderDialog
		{
		public:
			GTKFolderDialog();
			virtual ~GTKFolderDialog();

			virtual Bool ShowDialog(ControlHandle *ownerHandle);
		};
	}
}
#endif
