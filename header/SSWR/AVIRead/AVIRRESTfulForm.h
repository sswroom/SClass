#ifndef _SM_SSWR_AVIREAD_AVIRRESTFULFORM
#define _SM_SSWR_AVIREAD_AVIRRESTFULFORM
#include "Data/ArrayListStrUTF8.h"
#include "DB/DBCache.h"
#include "Net/WebServer/RESTfulHandler.h"
#include "Net/WebServer/WebListener.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/ListBoxLogger.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRRESTfulForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::WebServer::WebListener> svr;
			Optional<Net::WebServer::RESTfulHandler> restHdlr;
			IO::LogTool log;
			Optional<UI::ListBoxLogger> logger;
			Optional<DB::DBConn> dbConn;
			Optional<DB::DBTool> db;
			Optional<DB::DBCache> dbCache;
			Optional<DB::DBModel> dbModel;

			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpControl;
			NN<UI::GUIGroupBox> grpParam;
			NN<UI::GUILabel> lblPort;
			NN<UI::GUITextBox> txtPort;
			NN<UI::GUILabel> lblLogDir;
			NN<UI::GUITextBox> txtLogDir;
			NN<UI::GUILabel> lblAllowProxy;
			NN<UI::GUICheckBox> chkAllowProxy;
			NN<UI::GUILabel> lblSkipLog;
			NN<UI::GUICheckBox> chkSkipLog;
			NN<UI::GUILabel> lblAllowKA;
			NN<UI::GUICheckBox> chkAllowKA;
			NN<UI::GUILabel> lblDatabase;
			NN<UI::GUITextBox> txtDatabase;
			NN<UI::GUIButton> btnDatabaseMySQL;
			NN<UI::GUIButton> btnDatabaseODBCDSN;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUIButton> btnStop;

			NN<UI::GUITabPage> tpTable;
			NN<UI::GUIListView> lvTable;

			NN<UI::GUITabPage> tpLog;
			NN<UI::GUIListBox> lbLog;
			NN<UI::GUITextBox> txtLog;

		private:
			static void __stdcall OnDatabaseMySQLClicked(AnyType userObj);
			static void __stdcall OnDatabaseODBCDSNClicked(AnyType userObj);
			static void __stdcall OnStartClick(AnyType userObj);
			static void __stdcall OnStopClick(AnyType userObj);
			static void __stdcall OnLogSel(AnyType userObj);
			
			void InitDB();
		public:
			AVIRRESTfulForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRRESTfulForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
