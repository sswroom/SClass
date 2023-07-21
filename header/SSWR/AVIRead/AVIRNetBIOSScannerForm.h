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
			SSWR::AVIRead::AVIRCore *core;
			Net::NetBIOSScanner *netbios;
			Bool tableUpdated;

			UI::GUIPanel *pnlControl;
			UI::GUILabel *lblTargetAddr;
			UI::GUITextBox *txtTargetAddr;
			UI::GUICheckBox *chkTargetScan;
			UI::GUIButton *btnRequest;
			UI::GUIListView *lvAnswers;
			UI::GUIVSplitter *vspAnswers;
			UI::GUIListView *lvEntries;

			static void __stdcall OnRequestClicked(void *userObj);
			static void __stdcall OnAnswerSelChg(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnAnswerUpdated(void *userObj, UInt32 sortableIP);

		public:
			AVIRNetBIOSScannerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRNetBIOSScannerForm();

			virtual void OnMonitorChanged();

			Bool IsError();
		};
	}
}
#endif
