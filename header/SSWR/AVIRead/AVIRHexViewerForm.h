#ifndef _SM_SSWR_AVIREAD_AVIRHEXVIEWERFORM
#define _SM_SSWR_AVIREAD_AVIRHEXVIEWERFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUIHexFileView.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRHexViewerForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;

			UI::GUIHexFileView *hexView;

			static void __stdcall OnFilesDrop(void *userObj, const UTF8Char **files, UOSInt nFiles);
		public:
			AVIRHexViewerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRHexViewerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
