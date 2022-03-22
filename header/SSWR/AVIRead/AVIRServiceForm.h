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
			UI::GUIPanel *pnlDetail;
			UI::GUILabel *lblStatus;
			UI::GUITextBox *txtStatus;
			UI::GUILabel *lblProcId;
			UI::GUITextBox *txtProcId;
			UI::GUILabel *lblStartTime;
			UI::GUITextBox *txtStartTime;
			UI::GUILabel *lblMemory;
			UI::GUITextBox *txtMemory;

			SSWR::AVIRead::AVIRCore *core;
			IO::ServiceManager *svcMgr;

			static void __stdcall OnServiceSelChg(void *userObj);
			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnStopClicked(void *userObj);
		public:
			AVIRServiceForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRServiceForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
