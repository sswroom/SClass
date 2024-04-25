#ifndef _SM_SSWR_AVIREAD_AVIRNETBIOSSCANNERFORM
#define _SM_SSWR_AVIREAD_AVIRNETBIOSSCANNERFORM
#include "Net/NetBIOSScanner.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"
#include "UI/GUIVSplitter.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRNetBIOSScannerForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Net::NetBIOSScanner *netbios;
			Bool tableUpdated;

			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUILabel> lblTargetAddr;
			NN<UI::GUITextBox> txtTargetAddr;
			NN<UI::GUICheckBox> chkTargetScan;
			NN<UI::GUIButton> btnRequest;
			NN<UI::GUIListView> lvAnswers;
			NN<UI::GUIVSplitter> vspAnswers;
			NN<UI::GUIListView> lvEntries;

			static void __stdcall OnRequestClicked(AnyType userObj);
			static void __stdcall OnAnswerSelChg(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnAnswerUpdated(AnyType userObj, UInt32 sortableIP);

		public:
			AVIRNetBIOSScannerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRNetBIOSScannerForm();

			virtual void OnMonitorChanged();

			Bool IsError();
		};
	}
}
#endif
