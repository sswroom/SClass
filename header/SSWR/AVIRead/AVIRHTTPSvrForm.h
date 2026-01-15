#ifndef _SM_SSWR_AVIREAD_AVIRHTTPSVRFORM
#define _SM_SSWR_AVIREAD_AVIRHTTPSVRFORM
#include "Data/ArrayListStringNN.h"
#include "Data/ArrayListStrUTF8.h"
#include "Net/WebServer/HTTPDirectoryHandler.h"
#include "Net/WebServer/WebListener.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "Sync/MutexUsage.h"
#include "UI/ListBoxLogger.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
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
		class AVIRHTTPLog : public Net::WebServer::WebRequestLogger
		{
		public:
			typedef struct
			{
				Int64 reqTime;
				Net::SocketUtil::AddressInfo cliAddr;
				UInt16 cliPort;
				Optional<Text::String> reqURI;
				NN<Data::ArrayListStringNN> headerName;
				NN<Data::ArrayListStringNN> headerVal;
			} LogEntry;
		private:
			UIntOS logCnt;
			UnsafeArray<LogEntry> entries;
			UIntOS currEnt;
			Sync::Mutex entMut;

		public:
			AVIRHTTPLog(UIntOS logCnt);
			virtual ~AVIRHTTPLog();

			virtual void LogRequest(NN<Net::WebServer::WebRequest> req);
			UIntOS GetNextIndex();
			void Use(NN<Sync::MutexUsage> mutUsage);
			void GetEntries(NN<Data::ArrayListNN<LogEntry>> logs, NN<Data::ArrayListNative<UIntOS>> logIndex);
			NN<LogEntry> GetEntry(UIntOS index);
		};

		class AVIRHTTPSvrForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			Optional<Net::WebServer::WebListener> svr;
			Optional<IO::LogTool> log;
			Optional<Net::WebServer::HTTPDirectoryHandler> dirHdlr;
			Optional<UI::ListBoxLogger> logger;
			Net::WebServer::WebListener::SERVER_STATUS lastStatus;
			NN<AVIRHTTPLog> reqLog;
			UIntOS lastAccessIndex;
			Optional<Crypto::Cert::X509Cert> sslCert;
			Optional<Crypto::Cert::X509File> sslKey;
			Data::ArrayListNN<Crypto::Cert::X509Cert> caCerts;

			NN<UI::GUITabControl> tcMain;
			NN<UI::GUITabPage> tpControl;
			NN<UI::GUITabPage> tpStatus;
			NN<UI::GUITabPage> tpAccess;
			NN<UI::GUITabPage> tpLog;

			NN<UI::GUIGroupBox> grpParam;
			NN<UI::GUILabel> lblPort;
			NN<UI::GUITextBox> txtPort;
			NN<UI::GUILabel> lblDocDir;
			NN<UI::GUITextBox> txtDocDir;
			NN<UI::GUILabel> lblLogDir;
			NN<UI::GUITextBox> txtLogDir;
			NN<UI::GUILabel> lblSSL;
			NN<UI::GUICheckBox> chkSSL;
			NN<UI::GUIButton> btnSSLCert;
			NN<UI::GUILabel> lblSSLCert;
			NN<UI::GUILabel> lblWorkerCnt;
			NN<UI::GUITextBox> txtWorkerCnt;
			NN<UI::GUILabel> lblAllowBrowse;
			NN<UI::GUICheckBox> chkAllowBrowse;
			NN<UI::GUILabel> lblAllowProxy;
			NN<UI::GUICheckBox> chkAllowProxy;
			NN<UI::GUILabel> lblCacheFile;
			NN<UI::GUICheckBox> chkCacheFile;
			NN<UI::GUILabel> lblSPKPackageFile;
			NN<UI::GUICheckBox> chkSPKPackageFile;
			NN<UI::GUILabel> lblZIPPackageFile;
			NN<UI::GUICheckBox> chkZIPPackageFile;
			NN<UI::GUILabel> lblSkipLog;
			NN<UI::GUICheckBox> chkSkipLog;
			NN<UI::GUILabel> lblKeepAlive;
			NN<UI::GUIComboBox> cboKeepAlive;
			NN<UI::GUILabel> lblCrossOrigin;
			NN<UI::GUICheckBox> chkCrossOrigin;
			NN<UI::GUILabel> lblDownloadCnt;
			NN<UI::GUICheckBox> chkDownloadCnt;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUIButton> btnStop;

			NN<UI::GUILabel> lblConnCurr;
			NN<UI::GUITextBox> txtConnCurr;
			NN<UI::GUILabel> lblConnTotal;
			NN<UI::GUITextBox> txtConnTotal;
			NN<UI::GUILabel> lblDataRateR;
			NN<UI::GUITextBox> txtDataRateR;
			NN<UI::GUILabel> lblDataRateW;
			NN<UI::GUITextBox> txtDataRateW;
			NN<UI::GUILabel> lblDataTotalR;
			NN<UI::GUITextBox> txtDataTotalR;
			NN<UI::GUILabel> lblDataTotalW;
			NN<UI::GUITextBox> txtDataTotalW;
			NN<UI::GUILabel> lblReqRate;
			NN<UI::GUITextBox> txtReqRate;
			NN<UI::GUILabel> lblReqTotal;
			NN<UI::GUITextBox> txtReqTotal;

			NN<UI::GUIListBox> lbAccess;
			NN<UI::GUIHSplitter> hspAccess;
			NN<UI::GUITextBox> txtAccess;

			NN<UI::GUIListBox> lbLog;
			NN<UI::GUITextBox> txtLog;

		private:
			static void __stdcall OnStartClick(AnyType userObj);
			static void __stdcall OnStopClick(AnyType userObj);
			static void __stdcall OnLogSel(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnAccessSelChg(AnyType userObj);
			static void __stdcall OnSSLCertClicked(AnyType userObj);
			void ClearCACerts();
		public:
			AVIRHTTPSvrForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRHTTPSvrForm();

			virtual void OnMonitorChanged();

			void SetPort(Int32 port);
			void SetDocPath(Text::CStringNN docPath);
			void SetLogPath(Text::CStringNN logPath);
			void SetBrowseDir(Bool browseDir);
			void SetAllowProxy(Bool allowProxy);
			void StartListen();
		};
	}
}
#endif
