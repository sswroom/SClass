#ifndef _SM_UI_FILEDIALOG
#define _SM_UI_FILEDIALOG
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
		const UTF8Char *fileName;
		Bool isSave;
		Bool allowMulti;
		Data::ArrayListStrUTF8 *patterns;
		Data::ArrayListStrUTF8 *names;
		Data::ArrayListStrUTF8 *fileNames;
		OSInt filterIndex;

		void ClearFileNames();
	public:
		FileDialog(const WChar *compName, const WChar *appName, const WChar *dialogName, Bool isSave);
		virtual ~FileDialog();

		virtual void AddFilter(const UTF8Char *pattern, const UTF8Char *name);
		OSInt GetFilterIndex();

		void SetFileName(const UTF8Char *fileName);
		const UTF8Char *GetFileName();

		OSInt GetFileNameCount();
		const UTF8Char *GetFileNames(OSInt index);

		void SetAllowMultiSel(Bool allowMulti);

		Bool ShowDialog(void *ownerHandle);
	};
}
#endif
