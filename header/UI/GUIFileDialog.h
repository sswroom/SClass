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
		Optional<IO::Registry> reg;
		UnsafeArray<WChar> dialogName;
		UnsafeArrayOpt<const WChar> lastName;
		Optional<Text::String> fileName;
		Bool isSave;
		Bool allowMulti;
		Data::ArrayListStringNN patterns;
		Data::ArrayListStringNN names;
		Data::ArrayListStringNN fileNames;
		UIntOS filterIndex;

		void ClearFileNames();
	public:
		GUIFileDialog(UnsafeArray<const WChar> compName, UnsafeArray<const WChar> appName, UnsafeArray<const WChar> dialogName, Bool isSave);
		virtual ~GUIFileDialog();

		void AddFilter(Text::CStringNN pattern, Text::CStringNN name);
		UIntOS GetFilterIndex();
		void SetFileName(Text::CString fileName);
		NN<Text::String> GetFileName() const;
		UIntOS GetFileNameCount();
		Optional<Text::String> GetFileNames(UIntOS index);
		void SetAllowMultiSel(Bool allowMulti);

		virtual Bool ShowDialog(Optional<ControlHandle> ownerHandle) = 0;
	};
}
#endif
