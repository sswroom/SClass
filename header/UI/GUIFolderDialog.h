#ifndef _SM_UI_FOLDERDIALOG
#define _SM_UI_FOLDERDIALOG
#include "Handles.h"
#include "Text/String.h"

namespace UI
{
	class GUIFolderDialog
	{
	protected:
		Optional<Text::String> dirName;
		Optional<Text::String> message;
	public:
		GUIFolderDialog();
		virtual ~GUIFolderDialog();

		void SetFolder(Text::CString dirName);
		NN<Text::String> GetFolder() const;
		void SetMessage(Text::CStringNN message);

		virtual Bool ShowDialog(Optional<ControlHandle> ownerHandle) = 0;
	};
}
#endif
