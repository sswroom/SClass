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
			NN<SSWR::AVIRead::AVIRCore> core;
			IO::Stream *port;
			IO::TVControl *tvCtrl;
			IO::LogTool log;
			NN<UI::ListBoxLogger> logger;
			CommandInfo *cmdInfos;

			NN<UI::GUIPanel> pnlPort;
			NN<UI::GUILabel> lblTVType;
			NN<UI::GUIComboBox> cboTVType;
			NN<UI::GUILabel> lblPort;
			NN<UI::GUIComboBox> cboPort;
			NN<UI::GUICheckBox> chkLogFile;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUITabControl> tcMain;
			NN<UI::GUITabPage> tpControl;
			NN<UI::GUITabPage> tpLog;

			NN<UI::GUIListBox> lbLog;
			NN<UI::GUITextBox> txtLog;

			NN<UI::GUILabel> lblCommand;
			NN<UI::GUIComboBox> cboCommand;
			NN<UI::GUIButton> btnCommand;
			NN<UI::GUITextBox> txtCommand;

			static void __stdcall OnStartClick(AnyType userObj);
			static void __stdcall OnSendCommandClicked(AnyType userObj);
			static void __stdcall OnCmdChanged(AnyType userObj);
		public:
			AVIRTVControlForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRTVControlForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
