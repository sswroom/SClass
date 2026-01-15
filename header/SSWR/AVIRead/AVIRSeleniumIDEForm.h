#ifndef _SM_SSWR_AVIREAD_AVIRSELENIUMIDEFORM
#define _SM_SSWR_AVIREAD_AVIRSELENIUMIDEFORM
#include "IO/SeleniumIDE.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSeleniumIDEForm : public UI::GUIForm
		{
		private:
			struct RunStepStatus
			{
				Data::Timestamp ts;
				UIntOS index;
				Data::Duration dur;
			};

		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<IO::SeleniumIDE> side;
			Data::ArrayListNN<RunStepStatus> statusList;

			NN<UI::GUIPanel> pnlSIDE;
			NN<UI::GUILabel> lblSIDEName;
			NN<UI::GUITextBox> txtSIDEName;
			NN<UI::GUILabel> lblSIDEVersion;
			NN<UI::GUITextBox> txtSIDEVersion;
			NN<UI::GUILabel> lblSIDEURL;
			NN<UI::GUITextBox> txtSIDEURL;
			NN<UI::GUIListBox> lbTest;
			NN<UI::GUIPanel> pnlTestCtrl;
			NN<UI::GUILabel> lblTestPort;
			NN<UI::GUITextBox> txtTestPort;
			NN<UI::GUILabel> lblTestBrowser;
			NN<UI::GUIComboBox> cboTestBrowser;
			NN<UI::GUICheckBox> chkTestMobile;
			NN<UI::GUIComboBox> cboTestMobile;
			NN<UI::GUICheckBox> chkTestUserDataDir;
			NN<UI::GUITextBox> txtTestUserDataDir;
			NN<UI::GUIButton> btnTestRun;
			NN<UI::GUICheckBox> chkTestHeadless;
			NN<UI::GUICheckBox> chkTestDisableGPU;
			NN<UI::GUICheckBox> chkTestNoSandbox;
			NN<UI::GUICheckBox> chkTestNoPause;
			NN<UI::GUITabControl> tcTest;
			NN<UI::GUITabPage> tpCommand;
			NN<UI::GUIPanel> pnlCommand;
			NN<UI::GUIListView> lvCommand;
			NN<UI::GUILabel> lblCommand;
			NN<UI::GUITextBox> txtCommand;
			NN<UI::GUILabel> lblCommandTarget;
			NN<UI::GUITextBox> txtCommandTarget;
			NN<UI::GUILabel> lblCommandValue;
			NN<UI::GUITextBox> txtCommandValue;
			NN<UI::GUILabel> lblCommandComment;
			NN<UI::GUITextBox> txtCommandComment;
			NN<UI::GUITabPage> tpRunLog;
			NN<UI::GUIListView> lvRunLog;
			NN<UI::GUITabPage> tpTimeout;
			NN<UI::GUILabel> lblTimeoutScript;
			NN<UI::GUITextBox> txtTimeoutScript;
			NN<UI::GUILabel> lblTimeoutPageLoad;
			NN<UI::GUITextBox> txtTimeoutPageLoad;
			NN<UI::GUILabel> lblTimeoutImplicit;
			NN<UI::GUITextBox> txtTimeoutImplicit;
			NN<UI::GUILabel> lblTimeoutCommand;
			NN<UI::GUITextBox> txtTimeoutCommand;

			static void __stdcall OnCommandSelChg(AnyType userObj);
			static void __stdcall OnTestSelChg(AnyType userObj);
			static void __stdcall OnTestRunClicked(AnyType userObj);
			static void __stdcall OnStepStatus(AnyType userObj, UIntOS index, Data::Duration dur);
			void DisplayTest();
			void DisplayStatus();
		public:
			AVIRSeleniumIDEForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<IO::SeleniumIDE> side);
			virtual ~AVIRSeleniumIDEForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
