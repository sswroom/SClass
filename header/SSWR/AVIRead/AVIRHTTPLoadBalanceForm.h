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

			UI::GUITabControl *tcMain;
			NotNullPtr<UI::GUITabPage> tpControl;
			NotNullPtr<UI::GUITabPage> tpStatus;
			NotNullPtr<UI::GUITabPage> tpAccess;
			NotNullPtr<UI::GUITabPage> tpLog;

			NotNullPtr<UI::GUIGroupBox> grpParam;
			UI::GUILabel *lblPort;
			UI::GUITextBox *txtPort;
			UI::GUILabel *lblLogDir;
			UI::GUITextBox *txtLogDir;
			UI::GUILabel *lblSSL;
			UI::GUICheckBox *chkSSL;
			NotNullPtr<UI::GUIButton> btnSSLCert;
			UI::GUILabel *lblSSLCert;
			UI::GUILabel *lblAllowProxy;
			UI::GUICheckBox *chkAllowProxy;
			UI::GUILabel *lblSkipLog;
			UI::GUICheckBox *chkSkipLog;
			UI::GUILabel *lblAllowKA;
			UI::GUICheckBox *chkAllowKA;
			UI::GUILabel *lblFwdURL;
			UI::GUITextBox *txtFwdURL;
			UI::GUILabel *lblFwdType;
			NotNullPtr<UI::GUIComboBox> cboFwdType;
			NotNullPtr<UI::GUIButton> btnStart;
			NotNullPtr<UI::GUIButton> btnStop;

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
			NotNullPtr<UI::GUIHSplitter> hspAccess;
			UI::GUITextBox *txtAccess;

			NotNullPtr<UI::GUIListBox> lbLog;
			UI::GUITextBox *txtLog;

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
