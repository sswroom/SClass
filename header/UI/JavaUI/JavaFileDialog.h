#ifndef _SM_UI_JAVAUI_JAVAFILEDIALOG
#define _SM_UI_JAVAUI_JAVAFILEDIALOG
#include "Handles.h"
#include "Data/ArrayListStringNN.h"
#include "IO/Registry.h"
#include "UI/GUIFileDialog.h"

namespace UI
{
	namespace JavaUI
	{
		class JavaFileDialog : public UI::GUIFileDialog
		{
		public:
			JavaFileDialog(UnsafeArray<const WChar> compName, UnsafeArray<const WChar> appName, UnsafeArray<const WChar> dialogName, Bool isSave);
			virtual ~JavaFileDialog();

			virtual Bool ShowDialog(Optional<ControlHandle> ownerHandle);
		};
	}
}
#endif
