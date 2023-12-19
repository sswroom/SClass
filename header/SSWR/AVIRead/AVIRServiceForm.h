#ifndef _SM_SSWR_AVIREAD_AVIRSERVICEFORM
#define _SM_SSWR_AVIREAD_AVIRSERVICEFORM
#include "IO/ServiceManager.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRServiceForm : public UI::GUIForm
		{
		private:
			UI::GUIListView *lvService;
			NotNullPtr<UI::GUIHSplitter> hspService;
			NotNullPtr<UI::GUIPanel> pnlCtrl;
			NotNullPtr<UI::GUIButton> btnStart;
			NotNullPtr<UI::GUIButton> btnStop;
			NotNullPtr<UI::GUIButton> btnEnable;
			NotNullPtr<UI::GUIButton> btnDisable;
			NotNullPtr<UI::GUIButton> btnDelete;
			NotNullPtr<UI::GUIButton> btnCreate;
			NotNullPtr<UI::GUIPanel> pnlDetail;
			NotNullPtr<UI::GUILabel> lblName;
			NotNullPtr<UI::GUITextBox> txtName;
			NotNullPtr<UI::GUILabel> lblStatus;
			NotNullPtr<UI::GUITextBox> txtStatus;
			NotNullPtr<UI::GUILabel> lblEnabled;
			NotNullPtr<UI::GUITextBox> txtEnabled;
			NotNullPtr<UI::GUILabel> lblProcId;
			NotNullPtr<UI::GUITextBox> txtProcId;
			NotNullPtr<UI::GUILabel> lblStartTime;
			NotNullPtr<UI::GUITextBox> txtStartTime;
			NotNullPtr<UI::GUILabel> lblMemory;
			NotNullPtr<UI::GUITextBox> txtMemory;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::ServiceManager svcMgr;

			static void __stdcall OnServiceSelChg(void *userObj);
			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnStopClicked(void *userObj);
			static void __stdcall OnEnableClicked(void *userObj);
			static void __stdcall OnDisableClicked(void *userObj);
			static void __stdcall OnDeleteClicked(void *userObj);
			static void __stdcall OnCreateClicked(void *userObj);

			void UpdateSvcList();
		public:
			AVIRServiceForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRServiceForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
