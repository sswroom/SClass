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
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Net::SocketFactory> sockf;
			Net::ICMPScanner *scanner;

			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUILabel> lblIP;
			NN<UI::GUIComboBox> cboIP;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUIListView> lvIP;

			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
		public:
			AVIRIPScanForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRIPScanForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
