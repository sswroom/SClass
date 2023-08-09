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
			UI::GUILabel *lblName;
			UI::GUITextBox *txtName;
			UI::GUILabel *lblDesc;
			UI::GUITextBox *txtDesc;
			UI::GUILabel *lblState;
			UI::GUIComboBox *cboState;
			UI::GUILabel *lblCmdLine;
			UI::GUITextBox *txtCmdLine;
			UI::GUIButton *btnCreate;
			UI::GUIButton *btnCancel;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::ServiceManager svcMgr;

			static void __stdcall OnCreateClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
		public:
			AVIRServiceCreateForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRServiceCreateForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
