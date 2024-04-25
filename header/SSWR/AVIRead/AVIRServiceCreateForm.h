#ifndef _SM_SSWR_AVIREAD_AVIRSERVICECREATEFORM
#define _SM_SSWR_AVIREAD_AVIRSERVICECREATEFORM
#include "IO/ServiceManager.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRServiceCreateForm : public UI::GUIForm
		{
		private:
			NN<UI::GUILabel> lblName;
			NN<UI::GUITextBox> txtName;
			NN<UI::GUILabel> lblDesc;
			NN<UI::GUITextBox> txtDesc;
			NN<UI::GUILabel> lblState;
			NN<UI::GUIComboBox> cboState;
			NN<UI::GUILabel> lblCmdLine;
			NN<UI::GUITextBox> txtCmdLine;
			NN<UI::GUIButton> btnCreate;
			NN<UI::GUIButton> btnCancel;

			NN<SSWR::AVIRead::AVIRCore> core;
			IO::ServiceManager svcMgr;

			static void __stdcall OnCreateClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
		public:
			AVIRServiceCreateForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRServiceCreateForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
