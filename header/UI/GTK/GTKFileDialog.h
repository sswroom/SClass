#ifndef _SM_UI_WIN_WINFILEDIALOG
#define _SM_UI_WIN_WINFILEDIALOG
#include "UI/GUIFileDialog.h"

namespace UI
{
	namespace GTK
	{
		class GTKFileDialog : public UI::GUIFileDialog
		{
		public:
			GTKFileDialog(UnsafeArray<const WChar> compName, UnsafeArray<const WChar> appName, UnsafeArray<const WChar> dialogName, Bool isSave);
			virtual ~GTKFileDialog();

			virtual Bool ShowDialog(Optional<ControlHandle> ownerHandle);
		};
	}
}
#endif
