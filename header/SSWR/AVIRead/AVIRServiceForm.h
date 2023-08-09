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
			UI::GUIHSplitter *hspService;
			UI::GUIPanel *pnlCtrl;
			UI::GUIButton *btnStart;
			UI::GUIButton *btnStop;
			UI::GUIButton *btnEnable;
			UI::GUIButton *btnDisable;
			UI::GUIButton *btnDelete;
			UI::GUIButton *btnCreate;
			UI::GUIPanel *pnlDetail;
			UI::GUILabel *lblName;
			UI::GUITextBox *txtName;
			UI::GUILabel *lblStatus;
			UI::GUITextBox *txtStatus;
			UI::GUILabel *lblEnabled;
			UI::GUITextBox *txtEnabled;
			UI::GUILabel *lblProcId;
			UI::GUITextBox *txtProcId;
			UI::GUILabel *lblStartTime;
			UI::GUITextBox *txtStartTime;
			UI::GUILabel *lblMemory;
			UI::GUITextBox *txtMemory;

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
