#ifndef _SM_SSWR_AVIREAD_AVIRSELENIUMIDEFORM
#define _SM_SSWR_AVIREAD_AVIRSELENIUMIDEFORM
#include "IO/SeleniumIDE.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSeleniumIDEForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<IO::SeleniumIDE> side;

			NN<UI::GUIPanel> pnlSIDE;
			NN<UI::GUILabel> lblSIDEName;
			NN<UI::GUITextBox> txtSIDEName;
			NN<UI::GUILabel> lblSIDEVersion;
			NN<UI::GUITextBox> txtSIDEVersion;
			NN<UI::GUILabel> lblSIDEURL;
			NN<UI::GUITextBox> txtSIDEURL;
			NN<UI::GUIListBox> lbTest;
			NN<UI::GUIPanel> pnlTestCtrl;
			NN<UI::GUIButton> btnTestRun;
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

			static void __stdcall OnCommandSelChg(AnyType userObj);
			static void __stdcall OnTestSelChg(AnyType userObj);
			static void __stdcall OnTestRunClicked(AnyType userObj);
			void DisplayTest();
		public:
			AVIRSeleniumIDEForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<IO::SeleniumIDE> side);
			virtual ~AVIRSeleniumIDEForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
