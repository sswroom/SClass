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
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<UI::Clipboard> clipboard;

			NN<UI::GUIListBox> lbType;
			NN<UI::GUIHSplitter> hspMain;
			NN<UI::GUITextBox> txtMain;

			static void __stdcall OnTypeSelChg(AnyType userObj);
		public:
			AVIRClipboardViewerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRClipboardViewerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
