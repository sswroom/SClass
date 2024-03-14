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

			NotNullPtr<UI::GUILabel> lblPort;
			NotNullPtr<UI::GUITextBox> txtPort;
			NotNullPtr<UI::GUILabel> lblFwdURL;
			NotNullPtr<UI::GUITextBox> txtFwdURL;
			NotNullPtr<UI::GUILabel> lblLog;
			NotNullPtr<UI::GUICheckBox> chkLog;
			NotNullPtr<UI::GUICheckBox> chkLogContent;
			NotNullPtr<UI::GUILabel> lblSSL;
			NotNullPtr<UI::GUICheckBox> chkSSL;
			NotNullPtr<UI::GUIButton> btnSSLCert;
			NotNullPtr<UI::GUILabel> lblSSLCert;
			NotNullPtr<UI::GUILabel> lblAllowKA;
			NotNullPtr<UI::GUICheckBox> chkAllowKA;
			NotNullPtr<UI::GUIButton> btnStart;
			NotNullPtr<UI::GUIButton> btnStop;

		private:
			static void __stdcall OnStartClick(AnyType userObj);
			static void __stdcall OnStopClick(AnyType userObj);
			static void __stdcall OnSSLCertClicked(AnyType userObj);
			void ClearCACerts();
		public:
			AVIRHTTPForwarderForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRHTTPForwarderForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
