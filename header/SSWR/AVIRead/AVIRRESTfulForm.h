#ifndef _SM_SSWR_AVIREAD_AVIRRESTFULFORM
#define _SM_SSWR_AVIREAD_AVIRRESTFULFORM
#include "Data/ArrayListStrUTF8.h"
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
			SSWR::AVIRead::AVIRCore *core;
			Net::WebServer::WebListener *svr;
			Net::WebServer::RESTfulHandler *restHdlr;
			IO::LogTool *log;
			UI::ListBoxLogger *logger;
			DB::DBConn *dbConn;

			UI::GUITabControl *tcMain;

			UI::GUITabPage *tpControl;
			UI::GUIGroupBox *grpParam;
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

			UI::GUITabPage *tpTable;
			UI::GUIListView *lvTable;

			UI::GUITabPage *tpLog;
			UI::GUIListBox *lbLog;
			UI::GUITextBox *txtLog;

		private:
			static void __stdcall OnDatabaseMySQLClicked(void *userObj);
			static void __stdcall OnDatabaseODBCDSNClicked(void *userObj);
			static void __stdcall OnStartClick(void *userObj);
			static void __stdcall OnStopClick(void *userObj);
			static void __stdcall OnLogSel(void *userObj);
		public:
			AVIRRESTfulForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRRESTfulForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
