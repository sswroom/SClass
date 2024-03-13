#ifndef _SM_SSWR_AVIREAD_AVIRTFTPSERVERFORM
#define _SM_SSWR_AVIREAD_AVIRTFTPSERVERFORM
#include "Net/TFTPServer.h"
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
		class AVIRTFTPServerForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Net::TFTPServer *svr;
			IO::LogTool log;
			NotNullPtr<UI::ListBoxLogger> logger;

			NotNullPtr<UI::GUITabControl> tcMain;

			NotNullPtr<UI::GUITabPage> tpControl;
			NotNullPtr<UI::GUILabel> lblPort;
			NotNullPtr<UI::GUITextBox> txtPort;
			NotNullPtr<UI::GUILabel> lblFilePath;
			NotNullPtr<UI::GUITextBox> txtFilePath;
			NotNullPtr<UI::GUIButton> btnStart;

			NotNullPtr<UI::GUITabPage> tpLog;
			NotNullPtr<UI::GUIListBox> lbLog;
			NotNullPtr<UI::GUITextBox> txtLog;

		private:
			static void __stdcall OnStartClick(void *userObj);
			static void __stdcall OnLogSel(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
		public:
			AVIRTFTPServerForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRTFTPServerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
