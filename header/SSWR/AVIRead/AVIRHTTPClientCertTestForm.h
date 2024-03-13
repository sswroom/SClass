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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			Net::WebServer::WebListener *svr;
			IO::LogTool *log;
			Crypto::Cert::X509Cert *sslCert;
			Crypto::Cert::X509File *sslKey;
			Data::ArrayListNN<Crypto::Cert::X509Cert> caCerts;

			NotNullPtr<UI::GUIGroupBox> grpParam;
			NotNullPtr<UI::GUILabel> lblPort;
			NotNullPtr<UI::GUITextBox> txtPort;
			NotNullPtr<UI::GUILabel> lblSSL;
			NotNullPtr<UI::GUIButton> btnSSLCert;
			NotNullPtr<UI::GUILabel> lblSSLCert;
			NotNullPtr<UI::GUILabel> lblClientCA;
			NotNullPtr<UI::GUITextBox> txtClientCA;
			NotNullPtr<UI::GUIButton> btnStart;
			NotNullPtr<UI::GUIButton> btnStop;

		private:
			static void __stdcall OnStartClick(void *userObj);
			static void __stdcall OnStopClick(void *userObj);
			static void __stdcall OnSSLCertClicked(void *userObj);
			void ClearCACerts();
		public:
			AVIRHTTPClientCertTestForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRHTTPClientCertTestForm();

			virtual void OnMonitorChanged();

			virtual void WebRequest(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp);
			virtual void Release();
		};
	}
}
#endif
