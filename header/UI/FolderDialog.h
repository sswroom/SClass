#ifndef _SM_UI_FOLDERDIALOG
#define _SM_UI_FOLDERDIALOG
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

		void SetFolder(const UTF8Char *dirName);
		Text::String *GetFolder();
		void SetMessage(const UTF8Char *message);

		Bool ShowDialog(void *ownerHandle);
	};
};
#endif
