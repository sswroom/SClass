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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<UI::GUIPanel> pnlVersion;
			NotNullPtr<UI::GUILabel> lblVersion;
			NotNullPtr<UI::GUITextBox> txtVersion;
			NotNullPtr<UI::GUIListBox> lbVMS;
			NotNullPtr<UI::GUIHSplitter> hspVM;
			NotNullPtr<UI::GUITabControl> tcVM;
			NotNullPtr<UI::GUITabPage> tpControl;
			NotNullPtr<UI::GUILabel> lblState;
			NotNullPtr<UI::GUITextBox> txtState;
			NotNullPtr<UI::GUILabel> lblStateSince;
			NotNullPtr<UI::GUITextBox> txtStateSince;

			static void __stdcall OnVMSSelChg(AnyType userObj);
			void UpdateVMInfo();
		public:
			AVIRVBoxManagerForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRVBoxManagerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
