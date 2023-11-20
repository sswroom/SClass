#ifndef _SM_SSWR_AVIREAD_AVIRNTPSERVERFORM
#define _SM_SSWR_AVIREAD_AVIRNTPSERVERFORM
#include "Net/NTPServer.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/ListBoxLogger.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRNTPServerForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Net::NTPServer *svr;
			IO::LogTool log;
			NotNullPtr<UI::ListBoxLogger> logger;

			UI::GUITabControl *tcMain;

			NotNullPtr<UI::GUITabPage> tpControl;
			UI::GUILabel *lblPort;
			UI::GUITextBox *txtPort;
			UI::GUILabel *lblTimeServer;
			UI::GUITextBox *txtTimeServer;
			UI::GUIButton *btnStart;

			NotNullPtr<UI::GUITabPage> tpLog;
			NotNullPtr<UI::GUIListBox> lbLog;
			UI::GUITextBox *txtLog;

		private:
			static void __stdcall OnStartClick(void *userObj);
			static void __stdcall OnLogSel(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
		public:
			AVIRNTPServerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRNTPServerForm();

			virtual void OnMonitorChanged();
		};
	};
}
#endif
