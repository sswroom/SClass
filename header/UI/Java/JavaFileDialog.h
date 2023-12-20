#ifndef _SM_UI_JAVA_JAVAFILEDIALOG
#define _SM_UI_JAVA_JAVAFILEDIALOG
#include "Handles.h"
#include "Data/ArrayListStringNN.h"
#include "IO/Registry.h"
#include "UI/GUIFileDialog.h"

namespace UI
{
	namespace Java
	{
		class JavaFileDialog : public UI::GUIFileDialog
		{
		private:
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
			JavaFileDialog(const WChar *compName, const WChar *appName, const WChar *dialogName, Bool isSave);
			virtual ~JavaFileDialog();

			virtual void AddFilter(Text::CStringNN pattern, Text::CStringNN name);
			virtual UOSInt GetFilterIndex();
			virtual void SetFileName(Text::CString fileName);
			virtual NotNullPtr<Text::String> GetFileName() const;
			virtual UOSInt GetFileNameCount();
			virtual Optional<Text::String> GetFileNames(UOSInt index);
			virtual void SetAllowMultiSel(Bool allowMulti);

			virtual Bool ShowDialog(ControlHandle *ownerHandle);
		};
	}
}
#endif
