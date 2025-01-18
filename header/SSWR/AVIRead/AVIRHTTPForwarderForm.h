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
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			Optional<Net::WebServer::WebListener> svr;
			Optional<Net::WebServer::HTTPForwardHandler> fwdHdlr;
			Optional<IO::LogTool> fwdLog;
			Optional<Crypto::Cert::X509Cert> sslCert;
			Optional<Crypto::Cert::X509File> sslKey;
			Data::ArrayListNN<Crypto::Cert::X509Cert> caCerts;

			NN<UI::GUILabel> lblPort;
			NN<UI::GUITextBox> txtPort;
			NN<UI::GUILabel> lblFwdURL;
			NN<UI::GUITextBox> txtFwdURL;
			NN<UI::GUILabel> lblLog;
			NN<UI::GUICheckBox> chkLog;
			NN<UI::GUICheckBox> chkLogContent;
			NN<UI::GUILabel> lblSSL;
			NN<UI::GUICheckBox> chkSSL;
			NN<UI::GUIButton> btnSSLCert;
			NN<UI::GUILabel> lblSSLCert;
			NN<UI::GUILabel> lblAllowKA;
			NN<UI::GUICheckBox> chkAllowKA;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUIButton> btnStop;

		private:
			static void __stdcall OnStartClick(AnyType userObj);
			static void __stdcall OnStopClick(AnyType userObj);
			static void __stdcall OnSSLCertClicked(AnyType userObj);
			void ClearCACerts();
		public:
			AVIRHTTPForwarderForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRHTTPForwarderForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
