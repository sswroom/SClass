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
		class AVIRHTTPClientCertTestForm : public UI::GUIForm, public Net::WebServer::WebHandler
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			Optional<Net::WebServer::WebListener> svr;
			Optional<Crypto::Cert::X509Cert> sslCert;
			Optional<Crypto::Cert::X509File> sslKey;
			Data::ArrayListNN<Crypto::Cert::X509Cert> caCerts;

			NN<UI::GUIGroupBox> grpParam;
			NN<UI::GUILabel> lblPort;
			NN<UI::GUITextBox> txtPort;
			NN<UI::GUILabel> lblSSL;
			NN<UI::GUIButton> btnSSLCert;
			NN<UI::GUILabel> lblSSLCert;
			NN<UI::GUILabel> lblClientCA;
			NN<UI::GUITextBox> txtClientCA;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUIButton> btnStop;

		private:
			static void __stdcall OnStartClick(AnyType userObj);
			static void __stdcall OnStopClick(AnyType userObj);
			static void __stdcall OnSSLCertClicked(AnyType userObj);
			void ClearCACerts();
		public:
			AVIRHTTPClientCertTestForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRHTTPClientCertTestForm();

			virtual void OnMonitorChanged();

			virtual void DoWebRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
			virtual void Release();
		};
	}
}
#endif
