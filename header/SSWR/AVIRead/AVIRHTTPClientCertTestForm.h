#ifndef _SM_SSWR_AVIREAD_AVIRHTTPCLIENTCERTTESTFORM
#define _SM_SSWR_AVIREAD_AVIRHTTPCLIENTCERTTESTFORM
#include "Net/WebServer/WebListener.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRHTTPClientCertTestForm : public UI::GUIForm, public Net::WebServer::IWebHandler
		{
		private:
			SSWR::AVIRead::AVIRCore *core;
			Net::SSLEngine *ssl;
			Net::WebServer::WebListener *svr;
			IO::LogTool *log;
			Crypto::Cert::X509Cert *sslCert;
			Crypto::Cert::X509File *sslKey;

			UI::GUIGroupBox *grpParam;
			UI::GUILabel *lblPort;
			UI::GUITextBox *txtPort;
			UI::GUILabel *lblSSL;
			UI::GUIButton *btnSSLCert;
			UI::GUILabel *lblSSLCert;
			UI::GUILabel *lblClientCA;
			UI::GUITextBox *txtClientCA;
			UI::GUIButton *btnStart;
			UI::GUIButton *btnStop;

		private:
			static void __stdcall OnStartClick(void *userObj);
			static void __stdcall OnStopClick(void *userObj);
			static void __stdcall OnSSLCertClicked(void *userObj);
		public:
			AVIRHTTPClientCertTestForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRHTTPClientCertTestForm();

			virtual void OnMonitorChanged();

			virtual void WebRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);
			virtual void Release();
		};
	}
}
#endif
