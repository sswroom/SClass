#ifndef _SM_SSWR_AVIREAD_AVIRPYTHONFORM
#define _SM_SSWR_AVIREAD_AVIRPYTHONFORM
#include "Python/PythonCore.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
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

			NN<SSWR::AVIRead::AVIRCore> core;
			Python::PythonCore pyCore;
		public:
			AVIRPythonForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRPythonForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
