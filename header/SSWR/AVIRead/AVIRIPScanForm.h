#ifndef _SM_SSWR_AVIREAD_AVIRIPSCANFORM
#define _SM_SSWR_AVIREAD_AVIRIPSCANFORM
#include "Net/ICMPScanner.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRIPScanForm : public UI::GUIForm
		{

		private:
			SSWR::AVIRead::AVIRCore *core;
			NotNullPtr<Net::SocketFactory> sockf;
			Net::ICMPScanner *scanner;

			UI::GUIPanel *pnlControl;
			UI::GUILabel *lblIP;
			UI::GUIComboBox *cboIP;
			UI::GUIButton *btnStart;
			UI::GUIListView *lvIP;

			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
		public:
			AVIRIPScanForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRIPScanForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
