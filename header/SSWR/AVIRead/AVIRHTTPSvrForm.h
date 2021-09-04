#ifndef _SM_SSWR_AVIREAD_AVIRHTTPSVRFORM
#define _SM_SSWR_AVIREAD_AVIRHTTPSVRFORM
#include "Data/ArrayListStrUTF8.h"
#include "Net/WebServer/HTTPDirectoryHandler.h"
#include "Net/WebServer/WebListener.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "Sync/MutexUsage.h"
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
		class AVIRHTTPLog : public Net::WebServer::IReqLogger
		{
		public:
			typedef struct
			{
				Int64 reqTime;
				Net::SocketUtil::AddressInfo cliAddr;
				UInt16 cliPort;
				const UTF8Char *reqURI;
				Data::ArrayListStrUTF8 *headerName;
				Data::ArrayList<const UTF8Char *> *headerVal;
			} LogEntry;
		private:
			UOSInt logCnt;
			LogEntry *entries;
			UOSInt currEnt;
			Sync::Mutex *entMut;

		public:
			AVIRHTTPLog(UOSInt logCnt);
			virtual ~AVIRHTTPLog();

			virtual void LogRequest(Net::WebServer::IWebRequest *req);
			UOSInt GetNextIndex();
			void Use(Sync::MutexUsage *mutUsage);
			void GetEntries(Data::ArrayList<LogEntry*> *logs, Data::ArrayList<UOSInt> *logIndex);
			LogEntry *GetEntry(UOSInt index);
		};

		class AVIRHTTPSvrForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;
			Net::SSLEngine *ssl;
			Net::WebServer::WebListener *svr;
			IO::LogTool *log;
			Net::WebServer::HTTPDirectoryHandler *dirHdlr;
			UI::ListBoxLogger *logger;
			Net::WebServer::WebListener::SERVER_STATUS lastStatus;
			AVIRHTTPLog *reqLog;
			UOSInt lastAccessIndex;
			Crypto::Cert::X509File *sslCert;
			Crypto::Cert::X509File *sslKey;

			UI::GUITabControl *tcMain;
			UI::GUITabPage *tpControl;
			UI::GUITabPage *tpStatus;
			UI::GUITabPage *tpAccess;
			UI::GUITabPage *tpLog;

			UI::GUIGroupBox *grpParam;
			UI::GUILabel *lblPort;
			UI::GUITextBox *txtPort;
			UI::GUILabel *lblDocDir;
			UI::GUITextBox *txtDocDir;
			UI::GUILabel *lblLogDir;
			UI::GUITextBox *txtLogDir;
			UI::GUILabel *lblSSL;
			UI::GUICheckBox *chkSSL;
			UI::GUIButton *btnSSLCert;
			UI::GUILabel *lblSSLCert;
			UI::GUILabel *lblAllowBrowse;
			UI::GUICheckBox *chkAllowBrowse;
			UI::GUILabel *lblAllowProxy;
			UI::GUICheckBox *chkAllowProxy;
			UI::GUILabel *lblCacheFile;
			UI::GUICheckBox *chkCacheFile;
			UI::GUILabel *lblPackageFile;
			UI::GUICheckBox *chkPackageFile;
			UI::GUILabel *lblSkipLog;
			UI::GUICheckBox *chkSkipLog;
			UI::GUILabel *lblAllowKA;
			UI::GUICheckBox *chkAllowKA;
			UI::GUILabel *lblCrossOrigin;
			UI::GUICheckBox *chkCrossOrigin;
			UI::GUILabel *lblDownloadCnt;
			UI::GUICheckBox *chkDownloadCnt;
			UI::GUIButton *btnStart;
			UI::GUIButton *btnStop;

			UI::GUILabel *lblConnCurr;
			UI::GUITextBox *txtConnCurr;
			UI::GUILabel *lblConnTotal;
			UI::GUITextBox *txtConnTotal;
			UI::GUILabel *lblDataRateR;
			UI::GUITextBox *txtDataRateR;
			UI::GUILabel *lblDataRateW;
			UI::GUITextBox *txtDataRateW;
			UI::GUILabel *lblDataTotalR;
			UI::GUITextBox *txtDataTotalR;
			UI::GUILabel *lblDataTotalW;
			UI::GUITextBox *txtDataTotalW;
			UI::GUILabel *lblReqRate;
			UI::GUITextBox *txtReqRate;
			UI::GUILabel *lblReqTotal;
			UI::GUITextBox *txtReqTotal;

			UI::GUIListBox *lbAccess;
			UI::GUIHSplitter *hspAccess;
			UI::GUITextBox *txtAccess;

			UI::GUIListBox *lbLog;
			UI::GUITextBox *txtLog;

		private:
			static void __stdcall OnStartClick(void *userObj);
			static void __stdcall OnStopClick(void *userObj);
			static void __stdcall OnLogSel(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnAccessSelChg(void *userObj);
			static void __stdcall OnSSLCertClicked(void *userObj);
		public:
			AVIRHTTPSvrForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRHTTPSvrForm();

			virtual void OnMonitorChanged();

			void SetPort(Int32 port);
			void SetDocPath(const UTF8Char *docPath);
			void SetLogPath(const UTF8Char *logPath);
			void SetBrowseDir(Bool browseDir);
			void SetAllowProxy(Bool allowProxy);
			void StartListen();
		};
	}
}
#endif
