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
			NN<SSWR::AVIRead::AVIRCore> core;
			Net::MySQLTCPClient *cli;
			Bool cliConnected;

			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpControl;
			NN<UI::GUILabel> lblHost;
			NN<UI::GUITextBox> txtHost;
			NN<UI::GUILabel> lblPort;
			NN<UI::GUITextBox> txtPort;
			NN<UI::GUILabel> lblUserName;
			NN<UI::GUITextBox> txtUserName;
			NN<UI::GUILabel> lblPassword;
			NN<UI::GUITextBox> txtPassword;
			NN<UI::GUILabel> lblDatabase;
			NN<UI::GUITextBox> txtDatabase;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUILabel> lblStatus;
			NN<UI::GUITextBox> txtStatus;

			NN<UI::GUITabPage> tpInfo;
			NN<UI::GUILabel> lblServerVer;
			NN<UI::GUITextBox> txtServerVer;
			NN<UI::GUILabel> lblConnId;
			NN<UI::GUITextBox> txtConnId;
			NN<UI::GUILabel> lblAuthPluginData;
			NN<UI::GUITextBox> txtAuthPluginData;
			NN<UI::GUILabel> lblServerCap;
			NN<UI::GUITextBox> txtServerCap;
			NN<UI::GUILabel> lblServerCS;
			NN<UI::GUITextBox> txtServerCS;

			NN<UI::GUITabPage> tpQuery;
			NN<UI::GUIPanel> pnlQuery;
			NN<UI::GUIButton> btnQuery;
			NN<UI::GUITextBox> txtQuery;
			NN<UI::GUITextBox> txtQueryStatus;
			NN<UI::GUIVSplitter> vspQuery;
			NN<UI::GUIListView> lvQueryResult;

		private:
			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall OnQueryClicked(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			void UpdateResult(NN<DB::DBReader> r);
		public:
			AVIRMySQLClientForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRMySQLClientForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
