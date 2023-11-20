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

			UI::GUITabControl *tcMain;

			NotNullPtr<UI::GUITabPage> tpControl;
			NotNullPtr<UI::GUIGroupBox> grpParam;
			UI::GUILabel *lblPort;
			UI::GUITextBox *txtPort;
			UI::GUILabel *lblLogDir;
			UI::GUITextBox *txtLogDir;
			UI::GUILabel *lblAllowProxy;
			UI::GUICheckBox *chkAllowProxy;
			UI::GUILabel *lblSkipLog;
			UI::GUICheckBox *chkSkipLog;
			UI::GUILabel *lblAllowKA;
			UI::GUICheckBox *chkAllowKA;
			UI::GUILabel *lblDatabase;
			UI::GUITextBox *txtDatabase;
			UI::GUIButton *btnDatabaseMySQL;
			UI::GUIButton *btnDatabaseODBCDSN;
			UI::GUIButton *btnStart;
			UI::GUIButton *btnStop;

			NotNullPtr<UI::GUITabPage> tpTable;
			UI::GUIListView *lvTable;

			NotNullPtr<UI::GUITabPage> tpLog;
			NotNullPtr<UI::GUIListBox> lbLog;
			UI::GUITextBox *txtLog;

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
