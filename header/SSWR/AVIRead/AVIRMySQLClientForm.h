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
			SSWR::AVIRead::AVIRCore *core;
			Net::MySQLTCPClient *cli;
			Bool cliConnected;

			UI::GUITabControl *tcMain;

			UI::GUITabPage *tpControl;
			UI::GUILabel *lblHost;
			UI::GUITextBox *txtHost;
			UI::GUILabel *lblPort;
			UI::GUITextBox *txtPort;
			UI::GUILabel *lblUserName;
			UI::GUITextBox *txtUserName;
			UI::GUILabel *lblPassword;
			UI::GUITextBox *txtPassword;
			UI::GUILabel *lblDatabase;
			UI::GUITextBox *txtDatabase;
			UI::GUIButton *btnStart;

			UI::GUITabPage *tpInfo;
			UI::GUILabel *lblServerVer;
			UI::GUITextBox *txtServerVer;
			UI::GUILabel *lblConnId;
			UI::GUITextBox *txtConnId;
			UI::GUILabel *lblAuthPluginData;
			UI::GUITextBox *txtAuthPluginData;
			UI::GUILabel *lblServerCap;
			UI::GUITextBox *txtServerCap;
			UI::GUILabel *lblServerCS;
			UI::GUITextBox *txtServerCS;

			UI::GUITabPage *tpQuery;
			UI::GUIPanel *pnlQuery;
			UI::GUIButton *btnQuery;
			UI::GUITextBox *txtQuery;
			UI::GUITextBox *txtQueryStatus;
			UI::GUIVSplitter *vspQuery;
			UI::GUIListView *lvQueryResult;

		private:
			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnQueryClicked(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			void UpdateResult(DB::DBReader *r);
		public:
			AVIRMySQLClientForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRMySQLClientForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
