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
			NN<UI::GUIListView> lvService;
			NN<UI::GUIHSplitter> hspService;
			NN<UI::GUIPanel> pnlCtrl;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUIButton> btnStop;
			NN<UI::GUIButton> btnEnable;
			NN<UI::GUIButton> btnDisable;
			NN<UI::GUIButton> btnDelete;
			NN<UI::GUIButton> btnCreate;
			NN<UI::GUIPanel> pnlDetail;
			NN<UI::GUILabel> lblName;
			NN<UI::GUITextBox> txtName;
			NN<UI::GUILabel> lblStatus;
			NN<UI::GUITextBox> txtStatus;
			NN<UI::GUILabel> lblEnabled;
			NN<UI::GUITextBox> txtEnabled;
			NN<UI::GUILabel> lblProcId;
			NN<UI::GUITextBox> txtProcId;
			NN<UI::GUILabel> lblStartTime;
			NN<UI::GUITextBox> txtStartTime;
			NN<UI::GUILabel> lblMemory;
			NN<UI::GUITextBox> txtMemory;

			NN<SSWR::AVIRead::AVIRCore> core;
			IO::ServiceManager svcMgr;

			static void __stdcall OnServiceSelChg(AnyType userObj);
			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall OnStopClicked(AnyType userObj);
			static void __stdcall OnEnableClicked(AnyType userObj);
			static void __stdcall OnDisableClicked(AnyType userObj);
			static void __stdcall OnDeleteClicked(AnyType userObj);
			static void __stdcall OnCreateClicked(AnyType userObj);

			void UpdateSvcList();
		public:
			AVIRServiceForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRServiceForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
