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
			SSWR::AVIRead::AVIRCore *core;
			UI::GUIPanel *pnlVersion;
			UI::GUILabel *lblVersion;
			UI::GUITextBox *txtVersion;
			UI::GUIListBox *lbVMS;
			UI::GUIHSplitter *hspVM;
			UI::GUITabControl *tcVM;
			UI::GUITabPage *tpControl;
			UI::GUILabel *lblState;
			UI::GUITextBox *txtState;
			UI::GUILabel *lblStateSince;
			UI::GUITextBox *txtStateSince;

			static void __stdcall OnVMSSelChg(void *userObj);
			void UpdateVMInfo();
		public:
			AVIRVBoxManagerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRVBoxManagerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
