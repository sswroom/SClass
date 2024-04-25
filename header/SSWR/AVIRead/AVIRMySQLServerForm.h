#ifndef _SM_SSWR_AVIREAD_AVIRMYSQLSERVERFORM
#define _SM_SSWR_AVIREAD_AVIRMYSQLSERVERFORM
#include "Net/MySQLServer.h"
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
		class AVIRMySQLServerForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Net::MySQLServer *svr;
			DB::DBMS *dbms;
			IO::LogTool log;
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
			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall OnUserAddClicked(AnyType userObj);
			static void __stdcall OnLogSel(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
		public:
			AVIRMySQLServerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRMySQLServerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
