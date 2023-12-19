#ifndef _SM_SSWR_AVIREAD_AVIRCLIPBOARDVIEWERFORM
#define _SM_SSWR_AVIREAD_AVIRCLIPBOARDVIEWERFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/Clipboard.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUIListBox.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRClipboardViewerForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			UI::Clipboard *clipboard;

			UI::GUIListBox *lbType;
			NotNullPtr<UI::GUIHSplitter> hspMain;
			NotNullPtr<UI::GUITextBox> txtMain;

			static void __stdcall OnTypeSelChg(void *userObj);
		public:
			AVIRClipboardViewerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRClipboardViewerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
