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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			Net::WebServer::SAMLHandler *samlHdlr;
			Net::WebServer::WebListener *svr;
			Crypto::Cert::X509Cert *sslCert;
			Crypto::Cert::X509File *sslKey;
			Data::ArrayListNN<Crypto::Cert::X509Cert> caCerts;
			IO::LogTool log;
			NotNullPtr<UI::ListBoxLogger> logger;
			Sync::Mutex respMut;
			Text::String *respNew;

			UI::GUITabControl *tcMain;

			NotNullPtr<UI::GUITabPage> tpControl;
			NotNullPtr<UI::GUILabel> lblPort;
			NotNullPtr<UI::GUITextBox> txtPort;
			NotNullPtr<UI::GUILabel> lblSSL;
			NotNullPtr<UI::GUIButton> btnSSLCert;
			NotNullPtr<UI::GUILabel> lblSSLCert;
			NotNullPtr<UI::GUILabel> lblHost;
			NotNullPtr<UI::GUITextBox> txtHost;
			NotNullPtr<UI::GUILabel> lblSignCert;
			NotNullPtr<UI::GUITextBox> txtSignCert;
			NotNullPtr<UI::GUILabel> lblSignKey;
			NotNullPtr<UI::GUITextBox> txtSignKey;
			NotNullPtr<UI::GUILabel> lblSSOPath;
			NotNullPtr<UI::GUITextBox> txtSSOPath;
			NotNullPtr<UI::GUILabel> lblMetadataPath;
			NotNullPtr<UI::GUITextBox> txtMetadataPath;
			NotNullPtr<UI::GUILabel> lblLogoutPath;
			NotNullPtr<UI::GUITextBox> txtLogoutPath;
			NotNullPtr<UI::GUIButton> btnStart;
			NotNullPtr<UI::GUILabel> lblSSOURL;
			NotNullPtr<UI::GUITextBox> txtSSOURL;
			NotNullPtr<UI::GUILabel> lblMetadataURL;
			NotNullPtr<UI::GUITextBox> txtMetadataURL;
			NotNullPtr<UI::GUILabel> lblLogoutURL;
			NotNullPtr<UI::GUITextBox> txtLogoutURL;

			NotNullPtr<UI::GUITabPage> tpSAMLResp;
			NotNullPtr<UI::GUITextBox> txtSAMLResp;

			NotNullPtr<UI::GUITabPage> tpSAMLRespWF;
			NotNullPtr<UI::GUITextBox> txtSAMLRespWF;

			NotNullPtr<UI::GUITabPage> tpSAMLDecrypt;
			NotNullPtr<UI::GUITextBox> txtSAMLDecrypt;

			NotNullPtr<UI::GUITabPage> tpLog;
			NotNullPtr<UI::GUIListBox> lbLog;
			NotNullPtr<UI::GUITextBox> txtLog;

		private:
			static void __stdcall OnFormFiles(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles);
			static void __stdcall OnLogSel(void *userObj);
			static void __stdcall OnSSLCertClicked(void *userObj);
			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnSAMLResponse(void *userObj, Text::CString msg);
			static Bool __stdcall OnLoginRequest(void *userObj, NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, const Net::WebServer::SAMLMessage *msg);
			void ClearCACerts();
		public:
			AVIRSAMLTestForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSAMLTestForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
