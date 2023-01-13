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
			SSWR::AVIRead::AVIRCore *core;
			Net::SSLEngine *ssl;
			Net::WebServer::WebListener *svr;
			Net::WebServer::HTTPForwardHandler *fwdHdlr;
			Crypto::Cert::X509Cert *sslCert;
			Crypto::Cert::X509File *sslKey;

			UI::GUILabel *lblPort;
			UI::GUITextBox *txtPort;
			UI::GUILabel *lblFwdURL;
			UI::GUITextBox *txtFwdURL;
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
		public:
			AVIRHTTPForwarderForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRHTTPForwarderForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
