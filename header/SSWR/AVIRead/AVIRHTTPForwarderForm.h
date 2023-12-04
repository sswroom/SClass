#ifndef _SM_SSWR_AVIREAD_AVIRHTTPFORWARDERFORM
#define _SM_SSWR_AVIREAD_AVIRHTTPFORWARDERFORM
#include "Net/WebServer/HTTPForwardHandler.h"
#include "Net/WebServer/WebListener.h"
#include "SSWR/AVIRead/AVIRCore.h"
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
		class AVIRHTTPForwarderForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			Net::WebServer::WebListener *svr;
			Net::WebServer::HTTPForwardHandler *fwdHdlr;
			IO::LogTool *fwdLog;
			Crypto::Cert::X509Cert *sslCert;
			Crypto::Cert::X509File *sslKey;
			Data::ArrayListNN<Crypto::Cert::X509Cert> caCerts;

			UI::GUILabel *lblPort;
			UI::GUITextBox *txtPort;
			UI::GUILabel *lblFwdURL;
			UI::GUITextBox *txtFwdURL;
			UI::GUILabel *lblLog;
			UI::GUICheckBox *chkLog;
			UI::GUICheckBox *chkLogContent;
			UI::GUILabel *lblSSL;
			UI::GUICheckBox *chkSSL;
			UI::GUIButton *btnSSLCert;
			UI::GUILabel *lblSSLCert;
			UI::GUILabel *lblAllowKA;
			UI::GUICheckBox *chkAllowKA;
			UI::GUIButton *btnStart;
			UI::GUIButton *btnStop;

		private:
			static void __stdcall OnStartClick(void *userObj);
			static void __stdcall OnStopClick(void *userObj);
			static void __stdcall OnSSLCertClicked(void *userObj);
			void ClearCACerts();
		public:
			AVIRHTTPForwarderForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRHTTPForwarderForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
