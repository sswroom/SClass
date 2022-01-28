#ifndef _SM_UI_FILEDIALOG
#define _SM_UI_FILEDIALOG
#include "Handles.h"
#include "Data/ArrayListString.h"
#include "Data/ArrayListStrUTF8.h"
#include "IO/IFileSelector.h"
#include "IO/Registry.h"

namespace UI
{
	class FileDialog : public IO::IFileSelector
	{
	private:
		IO::Registry *reg;
		WChar *dialogName;
		const WChar *lastName;
		Text::String *fileName;
		Bool isSave;
		Bool allowMulti;
		Data::ArrayListString *patterns;
		Data::ArrayListString *names;
		Data::ArrayListStrUTF8 *fileNames;
		UOSInt filterIndex;

		void ClearFileNames();
	public:
		FileDialog(const WChar *compName, const WChar *appName, const WChar *dialogName, Bool isSave);
		virtual ~FileDialog();

		virtual void AddFilter(const UTF8Char *pattern, const UTF8Char *name);
		UOSInt GetFilterIndex();

		void SetFileName(const UTF8Char *fileName);
		Text::String *GetFileName();

		UOSInt GetFileNameCount();
		const UTF8Char *GetFileNames(UOSInt index);

		void SetAllowMultiSel(Bool allowMulti);

		Bool ShowDialog(ControlHandle *ownerHandle);
	};
}
#endif
