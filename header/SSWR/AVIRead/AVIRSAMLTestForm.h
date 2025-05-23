#ifndef _SM_SSWR_AVIREAD_AVIRSAMLTESTFORM
#define _SM_SSWR_AVIREAD_AVIRSAMLTESTFORM
#include "Net/SAMLHandler.h"
#include "Net/WebServer/SAMLService.h"
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
		class AVIRSAMLTestForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			Optional<Net::SAMLHandler> samlHdlr;
			Optional<Net::WebServer::SAMLService> samlSvc;
			Optional<Net::WebServer::WebListener> svr;
			Optional<Crypto::Cert::X509Cert> sslCert;
			Optional<Crypto::Cert::X509File> sslKey;
			Optional<Net::SAMLIdpConfig> samlCfg;
			Data::ArrayListNN<Crypto::Cert::X509Cert> caCerts;
			IO::LogTool log;
			NN<UI::ListBoxLogger> logger;
			Sync::Mutex respMut;
			Optional<Text::String> respNew;

			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpControl;
			NN<UI::GUILabel> lblPort;
			NN<UI::GUITextBox> txtPort;
			NN<UI::GUILabel> lblSSL;
			NN<UI::GUIButton> btnSSLCert;
			NN<UI::GUILabel> lblSSLCert;
			NN<UI::GUILabel> lblHost;
			NN<UI::GUITextBox> txtHost;
			NN<UI::GUILabel> lblSignCert;
			NN<UI::GUITextBox> txtSignCert;
			NN<UI::GUILabel> lblSignKey;
			NN<UI::GUITextBox> txtSignKey;
			NN<UI::GUILabel> lblHashType;
			NN<UI::GUIComboBox> cboHashType;
			NN<UI::GUILabel> lblAuthMethod;
			NN<UI::GUIComboBox> cboAuthMethod;
			NN<UI::GUILabel> lblSSOPath;
			NN<UI::GUITextBox> txtSSOPath;
			NN<UI::GUILabel> lblMetadataPath;
			NN<UI::GUITextBox> txtMetadataPath;
			NN<UI::GUILabel> lblLoginPath;
			NN<UI::GUITextBox> txtLoginPath;
			NN<UI::GUILabel> lblLogoutPath;
			NN<UI::GUITextBox> txtLogoutPath;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUILabel> lblSSOURL;
			NN<UI::GUITextBox> txtSSOURL;
			NN<UI::GUILabel> lblMetadataURL;
			NN<UI::GUITextBox> txtMetadataURL;
			NN<UI::GUILabel> lblLoginURL;
			NN<UI::GUITextBox> txtLoginURL;
			NN<UI::GUILabel> lblLogoutURL;
			NN<UI::GUITextBox> txtLogoutURL;

			NN<UI::GUITabPage> tpIdp;
			NN<UI::GUILabel> lblIdpMetadata;
			NN<UI::GUITextBox> txtIdpMetadata;
			NN<UI::GUIButton> btnIdpMetadata;
			NN<UI::GUILabel> lblIdpServiceName;
			NN<UI::GUITextBox> txtIdpServiceName;
			NN<UI::GUILabel> lblIdpSignOnLocation;
			NN<UI::GUITextBox> txtIdpSignOnLocation;
			NN<UI::GUILabel> lblIdpLogoutLocation;
			NN<UI::GUITextBox> txtIdpLogoutLocation;
			NN<UI::GUILabel> lblIdpEncryptionCert;
			NN<UI::GUITextBox> txtIdpEncryptionCert;
			NN<UI::GUILabel> lblIdpSigningCert;
			NN<UI::GUITextBox> txtIdpSigningCert;

			NN<UI::GUITabPage> tpSAMLResp;
			NN<UI::GUITextBox> txtSAMLResp;

			NN<UI::GUITabPage> tpSAMLRespWF;
			NN<UI::GUITextBox> txtSAMLRespWF;

			NN<UI::GUITabPage> tpSAMLDecrypt;
			NN<UI::GUITextBox> txtSAMLDecrypt;

			NN<UI::GUITabPage> tpLog;
			NN<UI::GUIListBox> lbLog;
			NN<UI::GUITextBox> txtLog;

		private:
			static void __stdcall OnFormFiles(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			static void __stdcall OnLogSel(AnyType userObj);
			static void __stdcall OnSSLCertClicked(AnyType userObj);
			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnIdpMetadataClicked(AnyType userObj);
			static void __stdcall OnHashTypeChanged(AnyType userObj);
			static void __stdcall OnAuthMethodChanged(AnyType userObj);
			static void __stdcall OnSSOResponse(AnyType userObj, NN<Net::SAMLSSOResponse> resp);
			void ClearCACerts();
			Optional<Crypto::Cert::X509Key> CreateSAMLKey();
		public:
			AVIRSAMLTestForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSAMLTestForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
