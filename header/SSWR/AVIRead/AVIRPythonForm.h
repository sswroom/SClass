#ifndef _SM_SSWR_AVIREAD_AVIRPYTHONFORM
#define _SM_SSWR_AVIREAD_AVIRPYTHONFORM
#include "Python/PythonCore.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRPythonForm : public UI::GUIForm
		{
		private:
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpInfo;
			NN<UI::GUILabel> lblVersion;
			NN<UI::GUITextBox> txtVersion;
			NN<UI::GUILabel> lblPlatform;
			NN<UI::GUITextBox> txtPlatform;
			NN<UI::GUILabel> lblCompiler;
			NN<UI::GUITextBox> txtCompiler;
			NN<UI::GUILabel> lblBuildInfo;
			NN<UI::GUITextBox> txtBuildInfo;
			NN<UI::GUILabel> lblCopyright;
			NN<UI::GUITextBox> txtCopyright;

			NN<UI::GUITabPage> tpModule;
			NN<UI::GUIPanel> pnlModule;
			NN<UI::GUILabel> lblModuleName;
			NN<UI::GUITextBox> txtModuleName;
			NN<UI::GUIButton> btnModule;
			NN<UI::GUILabel> lblModuleFile;
			NN<UI::GUITextBox> txtModuleFile;
			NN<UI::GUIListView> lvModule;

			NN<SSWR::AVIRead::AVIRCore> core;
			Python::PythonCore pyCore;
			Optional<Python::PythonModule> pyModule;

			static void __stdcall OnModuleClicked(AnyType userObj);
		public:
			AVIRPythonForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRPythonForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
