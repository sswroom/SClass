#ifndef _SM_UI_FOLDERDIALOG
#define _SM_UI_FOLDERDIALOG
#include "Handles.h"
#include "IO/Registry.h"
#include "Text/String.h"

namespace UI
{
	class FolderDialog
	{
	private:
		IO::Registry *reg;
		Text::String *dirName;
		const UTF8Char *message;

		static Int32 __stdcall BrowseCB(void *hwnd, UInt32 uMsg, OSInt lParam, OSInt lpData);
	public:
		FolderDialog(const WChar *compName, const WChar *appName, const WChar *dialogName);
		~FolderDialog();

		void SetFolder(Text::CString dirName);
		NotNullPtr<Text::String> GetFolder() const;
		void SetMessage(const UTF8Char *message);

		Bool ShowDialog(ControlHandle *ownerHandle);
	};
};
#endif
