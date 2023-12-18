#ifndef _SM_SSWR_AVIREAD_AVIRMYDBMSFORM
#define _SM_SSWR_AVIREAD_AVIRMYDBMSFORM
#include "Net/MySQLServer.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"
#include "UI/ListBoxLogger.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRMyDBMSForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Net::MySQLServer *svr;
			IO::LogTool *log;
			NotNullPtr<UI::ListBoxLogger> logger;

			NotNullPtr<UI::GUIPanel> pnlCtrl;
			NotNullPtr<UI::GUILabel> lblPort;
			UI::GUITextBox *txtPort;
			NotNullPtr<UI::GUIButton> btnStart;
			UI::GUITabControl *tcMain;

			NotNullPtr<UI::GUITabPage> tpUser;
			UI::GUIListBox *lbUser;
			NotNullPtr<UI::GUIHSplitter> hspUser;
			NotNullPtr<UI::GUIPanel> pnlUser;
			NotNullPtr<UI::GUILabel> lblUserName;
			UI::GUITextBox *txtUserName;
			NotNullPtr<UI::GUILabel> lblPassword;
			UI::GUITextBox *txtPassword;
			NotNullPtr<UI::GUIButton> btnUserAdd;

			NotNullPtr<UI::GUITabPage> tpLog;
			NotNullPtr<UI::GUIListBox> lbLog;
			UI::GUITextBox *txtLog;

		private:
			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnUserAddClicked(void *userObj);
			static void __stdcall OnLogSel(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
		public:
			AVIRMyDBMSForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRMyDBMSForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
