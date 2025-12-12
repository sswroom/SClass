#ifndef _SM_UI_JAVAUI_JUIFILEDIALOG
#define _SM_UI_JAVAUI_JUIFILEDIALOG
#include "Handles.h"
#include "Data/ArrayListStringNN.h"
#include "IO/Registry.h"
#include "UI/GUIFileDialog.h"

namespace UI
{
	namespace JavaUI
	{
		class JUIFileDialog : public UI::GUIFileDialog
		{
		public:
			JUIFileDialog(UnsafeArray<const WChar> compName, UnsafeArray<const WChar> appName, UnsafeArray<const WChar> dialogName, Bool isSave);
			virtual ~JUIFileDialog();

			virtual Bool ShowDialog(Optional<ControlHandle> ownerHandle);
		};
	}
}
#endif
