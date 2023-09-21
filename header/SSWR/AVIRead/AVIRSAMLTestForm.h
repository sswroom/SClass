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
			Net::SSLEngine *ssl;
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

			UI::GUITabPage *tpControl;
			UI::GUILabel *lblPort;
			UI::GUITextBox *txtPort;
			UI::GUILabel *lblSSL;
			UI::GUIButton *btnSSLCert;
			UI::GUILabel *lblSSLCert;
			UI::GUILabel *lblHost;
			UI::GUITextBox *txtHost;
			UI::GUILabel *lblSignCert;
			UI::GUITextBox *txtSignCert;
			UI::GUILabel *lblSignKey;
			UI::GUITextBox *txtSignKey;
			UI::GUILabel *lblSSOPath;
			UI::GUITextBox *txtSSOPath;
			UI::GUILabel *lblMetadataPath;
			UI::GUITextBox *txtMetadataPath;
			UI::GUILabel *lblLogoutPath;
			UI::GUITextBox *txtLogoutPath;
			UI::GUIButton *btnStart;
			UI::GUILabel *lblSSOURL;
			UI::GUITextBox *txtSSOURL;
			UI::GUILabel *lblMetadataURL;
			UI::GUITextBox *txtMetadataURL;
			UI::GUILabel *lblLogoutURL;
			UI::GUITextBox *txtLogoutURL;

			UI::GUITabPage *tpSAMLResp;
			UI::GUITextBox *txtSAMLResp;

			UI::GUITabPage *tpSAMLRespWF;
			UI::GUITextBox *txtSAMLRespWF;

			UI::GUITabPage *tpSAMLDecrypt;
			UI::GUITextBox *txtSAMLDecrypt;

			UI::GUITabPage *tpLog;
			NotNullPtr<UI::GUIListBox> lbLog;
			UI::GUITextBox *txtLog;

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
