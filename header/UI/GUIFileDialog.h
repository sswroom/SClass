#ifndef _SM_UI_FILEDIALOG
#define _SM_UI_FILEDIALOG
#include "Handles.h"
#include "Data/ArrayListStringNN.h"
#include "IO/FileSelector.h"
#include "IO/Registry.h"
#include "Text/String.h"

namespace UI
{
	class GUIFileDialog : public IO::FileSelector
	{
	protected:
		IO::Registry *reg;
		WChar *dialogName;
		const WChar *lastName;
		Optional<Text::String> fileName;
		Bool isSave;
		Bool allowMulti;
		Data::ArrayListStringNN patterns;
		Data::ArrayListStringNN names;
		Data::ArrayListStringNN fileNames;
		UOSInt filterIndex;

		void ClearFileNames();
	public:
		GUIFileDialog(const WChar *compName, const WChar *appName, const WChar *dialogName, Bool isSave);
		virtual ~GUIFileDialog();

		void AddFilter(Text::CStringNN pattern, Text::CStringNN name);
		UOSInt GetFilterIndex();
		void SetFileName(Text::CString fileName);
		NotNullPtr<Text::String> GetFileName() const;
		UOSInt GetFileNameCount();
		Optional<Text::String> GetFileNames(UOSInt index);
		void SetAllowMultiSel(Bool allowMulti);

		virtual Bool ShowDialog(ControlHandle *ownerHandle) = 0;
	};
}
#endif