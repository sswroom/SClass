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
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::NTPServer> svr;
			IO::LogTool log;
			NN<UI::ListBoxLogger> logger;

			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpControl;
			NN<UI::GUILabel> lblPort;
			NN<UI::GUITextBox> txtPort;
			NN<UI::GUILabel> lblTimeServer;
			NN<UI::GUITextBox> txtTimeServer;
			NN<UI::GUIButton> btnStart;

			NN<UI::GUITabPage> tpLog;
			NN<UI::GUIListBox> lbLog;
			NN<UI::GUITextBox> txtLog;

		private:
			static void __stdcall OnStartClick(AnyType userObj);
			static void __stdcall OnLogSel(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
		public:
			AVIRNTPServerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRNTPServerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
