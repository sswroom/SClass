#ifndef _SM_SSWR_AVIREAD_AVIRCLIPBOARDVIEWERFORM
#define _SM_SSWR_AVIREAD_AVIRCLIPBOARDVIEWERFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUIListBox.h"
#include "UI/GUITextBox.h"
#include "Win32/Clipboard.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRClipboardViewerForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;
			Win32::Clipboard *clipboard;

			UI::GUIListBox *lbType;
			UI::GUIHSplitter *hspMain;
			UI::GUITextBox *txtMain;

			static void __stdcall OnTypeSelChg(void *userObj);
		public:
			AVIRClipboardViewerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRClipboardViewerForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
