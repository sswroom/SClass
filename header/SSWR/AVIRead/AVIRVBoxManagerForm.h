#ifndef _SM_SSWR_AVIREAD_AVIRVBOXMANAGERFORM
#define _SM_SSWR_AVIREAD_AVIRVBOXMANAGERFORM
#include "IO/VBoxManager.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRVBoxManagerForm : public UI::GUIForm
		{
		private:
			IO::VBoxManager vbox;
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<UI::GUIPanel> pnlVersion;
			NN<UI::GUILabel> lblVersion;
			NN<UI::GUITextBox> txtVersion;
			NN<UI::GUIListBox> lbVMS;
			NN<UI::GUIHSplitter> hspVM;
			NN<UI::GUITabControl> tcVM;
			NN<UI::GUITabPage> tpControl;
			NN<UI::GUILabel> lblState;
			NN<UI::GUITextBox> txtState;
			NN<UI::GUILabel> lblStateSince;
			NN<UI::GUITextBox> txtStateSince;

			static void __stdcall OnVMSSelChg(AnyType userObj);
			void UpdateVMInfo();
		public:
			AVIRVBoxManagerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRVBoxManagerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
