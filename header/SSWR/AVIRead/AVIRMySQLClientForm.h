#ifndef _SM_SSWR_AVIREAD_AVIRMYSQLCLIENTFORM
#define _SM_SSWR_AVIREAD_AVIRMYSQLCLIENTFORM
#include "Net/MySQLTCPClient.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"
#include "UI/GUIVSplitter.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRMySQLClientForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Net::MySQLTCPClient *cli;
			Bool cliConnected;

			UI::GUITabControl *tcMain;

			NotNullPtr<UI::GUITabPage> tpControl;
			NotNullPtr<UI::GUILabel> lblHost;
			NotNullPtr<UI::GUITextBox> txtHost;
			NotNullPtr<UI::GUILabel> lblPort;
			NotNullPtr<UI::GUITextBox> txtPort;
			NotNullPtr<UI::GUILabel> lblUserName;
			NotNullPtr<UI::GUITextBox> txtUserName;
			NotNullPtr<UI::GUILabel> lblPassword;
			NotNullPtr<UI::GUITextBox> txtPassword;
			NotNullPtr<UI::GUILabel> lblDatabase;
			NotNullPtr<UI::GUITextBox> txtDatabase;
			NotNullPtr<UI::GUIButton> btnStart;
			NotNullPtr<UI::GUILabel> lblStatus;
			NotNullPtr<UI::GUITextBox> txtStatus;

			NotNullPtr<UI::GUITabPage> tpInfo;
			NotNullPtr<UI::GUILabel> lblServerVer;
			NotNullPtr<UI::GUITextBox> txtServerVer;
			NotNullPtr<UI::GUILabel> lblConnId;
			NotNullPtr<UI::GUITextBox> txtConnId;
			NotNullPtr<UI::GUILabel> lblAuthPluginData;
			NotNullPtr<UI::GUITextBox> txtAuthPluginData;
			NotNullPtr<UI::GUILabel> lblServerCap;
			NotNullPtr<UI::GUITextBox> txtServerCap;
			NotNullPtr<UI::GUILabel> lblServerCS;
			NotNullPtr<UI::GUITextBox> txtServerCS;

			NotNullPtr<UI::GUITabPage> tpQuery;
			NotNullPtr<UI::GUIPanel> pnlQuery;
			NotNullPtr<UI::GUIButton> btnQuery;
			NotNullPtr<UI::GUITextBox> txtQuery;
			NotNullPtr<UI::GUITextBox> txtQueryStatus;
			NotNullPtr<UI::GUIVSplitter> vspQuery;
			UI::GUIListView *lvQueryResult;

		private:
			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnQueryClicked(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			void UpdateResult(NotNullPtr<DB::DBReader> r);
		public:
			AVIRMySQLClientForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRMySQLClientForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
