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
			NotNullPtr<UI::GUILabel> lblName;
			NotNullPtr<UI::GUITextBox> txtName;
			NotNullPtr<UI::GUILabel> lblDesc;
			NotNullPtr<UI::GUITextBox> txtDesc;
			NotNullPtr<UI::GUILabel> lblState;
			NotNullPtr<UI::GUIComboBox> cboState;
			NotNullPtr<UI::GUILabel> lblCmdLine;
			NotNullPtr<UI::GUITextBox> txtCmdLine;
			NotNullPtr<UI::GUIButton> btnCreate;
			NotNullPtr<UI::GUIButton> btnCancel;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::ServiceManager svcMgr;

			static void __stdcall OnCreateClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
		public:
			AVIRServiceCreateForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRServiceCreateForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
