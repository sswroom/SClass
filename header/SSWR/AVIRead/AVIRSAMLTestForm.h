#ifndef _SM_SSWR_AVIREAD_AVIRSAMLTESTFORM
#define _SM_SSWR_AVIREAD_AVIRSAMLTESTFORM
#include "Net/WebServer/SAMLHandler.h"
#include "Net/WebServer/WebListener.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "Sync/MutexUsage.h"
#include "UI/ListBoxLogger.h"
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
		class AVIRSAMLTestForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			Net::WebServer::SAMLHandler *samlHdlr;
			Net::WebServer::WebListener *svr;
			Optional<Crypto::Cert::X509Cert> sslCert;
			Optional<Crypto::Cert::X509File> sslKey;
			Data::ArrayListNN<Crypto::Cert::X509Cert> caCerts;
			IO::LogTool log;
			NN<UI::ListBoxLogger> logger;
			Sync::Mutex respMut;
			Text::String *respNew;

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
			NN<UI::GUILabel> lblSSOPath;
			NN<UI::GUITextBox> txtSSOPath;
			NN<UI::GUILabel> lblMetadataPath;
			NN<UI::GUITextBox> txtMetadataPath;
			NN<UI::GUILabel> lblLogoutPath;
			NN<UI::GUITextBox> txtLogoutPath;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUILabel> lblSSOURL;
			NN<UI::GUITextBox> txtSSOURL;
			NN<UI::GUILabel> lblMetadataURL;
			NN<UI::GUITextBox> txtMetadataURL;
			NN<UI::GUILabel> lblLogoutURL;
			NN<UI::GUITextBox> txtLogoutURL;

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
			static void __stdcall OnSAMLResponse(AnyType userObj, Text::CStringNN msg);
			static Bool __stdcall OnLoginRequest(AnyType userObj, NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, const Net::WebServer::SAMLMessage *msg);
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
