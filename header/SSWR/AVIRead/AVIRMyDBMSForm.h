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
			NN<SSWR::AVIRead::AVIRCore> core;
			Net::MySQLServer *svr;
			IO::LogTool *log;
			NN<UI::ListBoxLogger> logger;

			NN<UI::GUIPanel> pnlCtrl;
			NN<UI::GUILabel> lblPort;
			NN<UI::GUITextBox> txtPort;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpUser;
			NN<UI::GUIListBox> lbUser;
			NN<UI::GUIHSplitter> hspUser;
			NN<UI::GUIPanel> pnlUser;
			NN<UI::GUILabel> lblUserName;
			NN<UI::GUITextBox> txtUserName;
			NN<UI::GUILabel> lblPassword;
			NN<UI::GUITextBox> txtPassword;
			NN<UI::GUIButton> btnUserAdd;

			NN<UI::GUITabPage> tpLog;
			NN<UI::GUIListBox> lbLog;
			NN<UI::GUITextBox> txtLog;

		private:
			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnUserAddClicked(void *userObj);
			static void __stdcall OnLogSel(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
		public:
			AVIRMyDBMSForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRMyDBMSForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
