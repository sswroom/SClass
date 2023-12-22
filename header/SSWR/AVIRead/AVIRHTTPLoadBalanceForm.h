#ifndef _SM_SSWR_AVIREAD_AVIRHTTPLOADBALANCEFORM
#define _SM_SSWR_AVIREAD_AVIRHTTPLOADBALANCEFORM
#include "Data/ArrayListStrUTF8.h"
#include "Net/WebServer/HTTPForwardHandler.h"
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
		class AVIRHTTPLog;

		class AVIRHTTPLoadBalanceForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			Net::WebServer::WebListener *svr;
			IO::LogTool *log;
			Net::WebServer::HTTPForwardHandler *fwdHdlr;
			UI::ListBoxLogger *logger;
			Net::WebServer::WebListener::SERVER_STATUS lastStatus;
			AVIRHTTPLog *reqLog;
			UOSInt lastAccessIndex;
			Crypto::Cert::X509Cert *sslCert;
			Crypto::Cert::X509File *sslKey;
			Data::ArrayListNN<Crypto::Cert::X509Cert> caCerts;

			NotNullPtr<UI::GUITabControl> tcMain;
			NotNullPtr<UI::GUITabPage> tpControl;
			NotNullPtr<UI::GUITabPage> tpStatus;
			NotNullPtr<UI::GUITabPage> tpAccess;
			NotNullPtr<UI::GUITabPage> tpLog;

			NotNullPtr<UI::GUIGroupBox> grpParam;
			NotNullPtr<UI::GUILabel> lblPort;
			NotNullPtr<UI::GUITextBox> txtPort;
			NotNullPtr<UI::GUILabel> lblLogDir;
			NotNullPtr<UI::GUITextBox> txtLogDir;
			NotNullPtr<UI::GUILabel> lblSSL;
			NotNullPtr<UI::GUICheckBox> chkSSL;
			NotNullPtr<UI::GUIButton> btnSSLCert;
			NotNullPtr<UI::GUILabel> lblSSLCert;
			NotNullPtr<UI::GUILabel> lblAllowProxy;
			NotNullPtr<UI::GUICheckBox> chkAllowProxy;
			NotNullPtr<UI::GUILabel> lblSkipLog;
			NotNullPtr<UI::GUICheckBox> chkSkipLog;
			NotNullPtr<UI::GUILabel> lblAllowKA;
			NotNullPtr<UI::GUICheckBox> chkAllowKA;
			NotNullPtr<UI::GUILabel> lblFwdURL;
			NotNullPtr<UI::GUITextBox> txtFwdURL;
			NotNullPtr<UI::GUILabel> lblFwdType;
			NotNullPtr<UI::GUIComboBox> cboFwdType;
			NotNullPtr<UI::GUIButton> btnStart;
			NotNullPtr<UI::GUIButton> btnStop;

			NotNullPtr<UI::GUILabel> lblConnCurr;
			NotNullPtr<UI::GUITextBox> txtConnCurr;
			NotNullPtr<UI::GUILabel> lblConnTotal;
			NotNullPtr<UI::GUITextBox> txtConnTotal;
			NotNullPtr<UI::GUILabel> lblDataRateR;
			NotNullPtr<UI::GUITextBox> txtDataRateR;
			NotNullPtr<UI::GUILabel> lblDataRateW;
			NotNullPtr<UI::GUITextBox> txtDataRateW;
			NotNullPtr<UI::GUILabel> lblDataTotalR;
			NotNullPtr<UI::GUITextBox> txtDataTotalR;
			NotNullPtr<UI::GUILabel> lblDataTotalW;
			NotNullPtr<UI::GUITextBox> txtDataTotalW;
			NotNullPtr<UI::GUILabel> lblReqRate;
			NotNullPtr<UI::GUITextBox> txtReqRate;
			NotNullPtr<UI::GUILabel> lblReqTotal;
			NotNullPtr<UI::GUITextBox> txtReqTotal;

			NotNullPtr<UI::GUIListBox> lbAccess;
			NotNullPtr<UI::GUIHSplitter> hspAccess;
			NotNullPtr<UI::GUITextBox> txtAccess;

			NotNullPtr<UI::GUIListBox> lbLog;
			NotNullPtr<UI::GUITextBox> txtLog;

		private:
			static void __stdcall OnStartClick(void *userObj);
			static void __stdcall OnStopClick(void *userObj);
			static void __stdcall OnLogSel(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnAccessSelChg(void *userObj);
			static void __stdcall OnSSLCertClicked(void *userObj);
			void ClearCACerts();
		public:
			AVIRHTTPLoadBalanceForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRHTTPLoadBalanceForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
