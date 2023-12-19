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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Net::WebServer::WebListener *svr;
			Net::WebServer::RESTfulHandler *restHdlr;
			IO::LogTool log;
			UI::ListBoxLogger *logger;
			DB::DBConn *dbConn;
			DB::DBTool *db;
			DB::DBCache *dbCache;
			DB::DBModel *dbModel;

			NotNullPtr<UI::GUITabControl> tcMain;

			NotNullPtr<UI::GUITabPage> tpControl;
			NotNullPtr<UI::GUIGroupBox> grpParam;
			NotNullPtr<UI::GUILabel> lblPort;
			NotNullPtr<UI::GUITextBox> txtPort;
			NotNullPtr<UI::GUILabel> lblLogDir;
			NotNullPtr<UI::GUITextBox> txtLogDir;
			NotNullPtr<UI::GUILabel> lblAllowProxy;
			NotNullPtr<UI::GUICheckBox> chkAllowProxy;
			NotNullPtr<UI::GUILabel> lblSkipLog;
			NotNullPtr<UI::GUICheckBox> chkSkipLog;
			NotNullPtr<UI::GUILabel> lblAllowKA;
			NotNullPtr<UI::GUICheckBox> chkAllowKA;
			NotNullPtr<UI::GUILabel> lblDatabase;
			NotNullPtr<UI::GUITextBox> txtDatabase;
			NotNullPtr<UI::GUIButton> btnDatabaseMySQL;
			NotNullPtr<UI::GUIButton> btnDatabaseODBCDSN;
			NotNullPtr<UI::GUIButton> btnStart;
			NotNullPtr<UI::GUIButton> btnStop;

			NotNullPtr<UI::GUITabPage> tpTable;
			UI::GUIListView *lvTable;

			NotNullPtr<UI::GUITabPage> tpLog;
			NotNullPtr<UI::GUIListBox> lbLog;
			NotNullPtr<UI::GUITextBox> txtLog;

		private:
			static void __stdcall OnDatabaseMySQLClicked(void *userObj);
			static void __stdcall OnDatabaseODBCDSNClicked(void *userObj);
			static void __stdcall OnStartClick(void *userObj);
			static void __stdcall OnStopClick(void *userObj);
			static void __stdcall OnLogSel(void *userObj);
			
			void InitDB();
		public:
			AVIRRESTfulForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRRESTfulForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
