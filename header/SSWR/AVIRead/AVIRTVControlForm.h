#ifndef _SM_SSWR_AVIREAD_AVIRTVCONTROLFORM
#define _SM_SSWR_AVIREAD_AVIRTVCONTROLFORM
#include "IO/TVControl.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/ListBoxLogger.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRTVControlForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				IO::TVControl::CommandType cmdType;
				IO::TVControl::CommandFormat cmdFmt;
			} CommandInfo;
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::Stream *port;
			IO::TVControl *tvCtrl;
			IO::LogTool log;
			NotNullPtr<UI::ListBoxLogger> logger;
			CommandInfo *cmdInfos;

			NotNullPtr<UI::GUIPanel> pnlPort;
			NotNullPtr<UI::GUILabel> lblTVType;
			NotNullPtr<UI::GUIComboBox> cboTVType;
			NotNullPtr<UI::GUILabel> lblPort;
			NotNullPtr<UI::GUIComboBox> cboPort;
			NotNullPtr<UI::GUICheckBox> chkLogFile;
			NotNullPtr<UI::GUIButton> btnStart;
			UI::GUITabControl *tcMain;
			NotNullPtr<UI::GUITabPage> tpControl;
			NotNullPtr<UI::GUITabPage> tpLog;

			NotNullPtr<UI::GUIListBox> lbLog;
			NotNullPtr<UI::GUITextBox> txtLog;

			NotNullPtr<UI::GUILabel> lblCommand;
			NotNullPtr<UI::GUIComboBox> cboCommand;
			NotNullPtr<UI::GUIButton> btnCommand;
			NotNullPtr<UI::GUITextBox> txtCommand;

			static void __stdcall OnStartClick(void *userObj);
			static void __stdcall OnSendCommandClicked(void *userObj);
			static void __stdcall OnCmdChanged(void *userObj);
		public:
			AVIRTVControlForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRTVControlForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
