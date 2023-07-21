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
			SSWR::AVIRead::AVIRCore *core;
			IO::Stream *port;
			IO::TVControl *tvCtrl;
			IO::LogTool *log;
			UI::ListBoxLogger *logger;
			CommandInfo *cmdInfos;

			UI::GUIPanel *pnlPort;
			UI::GUILabel *lblTVType;
			UI::GUIComboBox *cboTVType;
			UI::GUILabel *lblPort;
			UI::GUIComboBox *cboPort;
			UI::GUICheckBox *chkLogFile;
			UI::GUIButton *btnStart;
			UI::GUITabControl *tcMain;
			UI::GUITabPage *tpControl;
			UI::GUITabPage *tpLog;

			UI::GUIListBox *lbLog;
			UI::GUITextBox *txtLog;

			UI::GUILabel *lblCommand;
			UI::GUIComboBox *cboCommand;
			UI::GUIButton *btnCommand;
			UI::GUITextBox *txtCommand;

			static void __stdcall OnStartClick(void *userObj);
			static void __stdcall OnSendCommandClicked(void *userObj);
			static void __stdcall OnCmdChanged(void *userObj);
		public:
			AVIRTVControlForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRTVControlForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
