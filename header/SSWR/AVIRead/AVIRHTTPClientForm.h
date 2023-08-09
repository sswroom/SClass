#ifndef _SM_SSWR_AVIREAD_AVIRHTTPCLIENTFORM
#define _SM_SSWR_AVIREAD_AVIRHTTPCLIENTFORM
#include "Data/ArrayList.h"
#include "IO/MemoryStream.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRHTTPClientForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				NotNullPtr<Text::String> name;
				NotNullPtr<Text::String> value;
			} ParamValue;

			typedef struct
			{
				NotNullPtr<Text::String> name;
				NotNullPtr<Text::String> value;
				NotNullPtr<Text::String> domain;
				Text::String *path;
				Int64 expireTime;
				Bool secure;
			} HTTPCookie;
			
			
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			UI::GUITabControl *tcMain;

			UI::GUITabPage *tpRequest;
			UI::GUIPanel *pnlRequest;
			UI::GUILabel *lblURL;
			UI::GUITextBox *txtURL;
			UI::GUICheckBox *chkNoShutdown;
			UI::GUILabel *lblMethod;
			UI::GUIComboBox *cboMethod;
			UI::GUICheckBox *chkOSClient;
			UI::GUICheckBox *chkAllowComp;
			UI::GUIButton *btnUserAgent;
			UI::GUILabel *lblUserAgent;
			UI::GUIButton *btnClientCert;
			UI::GUILabel *lblClientCert;
			UI::GUILabel *lblUserName;
			UI::GUITextBox *txtUserName;
			UI::GUILabel *lblPassword;
			UI::GUITextBox *txtPassword;
			UI::GUILabel *lblFileUpload;
			UI::GUITextBox *txtFileFormName;
			UI::GUIButton *btnFileSelect;
			UI::GUIButton *btnFileClear;
			UI::GUILabel *lblFileStatus;
			UI::GUILabel *lblDataStr;
			UI::GUITextBox *txtDataStr;
			UI::GUIButton *btnDataStr;
			UI::GUILabel *lblPostFormat;
			UI::GUIComboBox *cboPostFormat;
			UI::GUILabel *lblHeaders;
			UI::GUITextBox *txtHeaders;
			UI::GUIButton *btnRequest;
			UI::GUIListView *lvReqData;

			UI::GUITabPage *tpResponse;
			UI::GUIPanel *pnlResponse;
			UI::GUILabel *lblReqURL;
			UI::GUITextBox *txtReqURL;
			UI::GUILabel *lblSvrIP;
			UI::GUITextBox *txtSvrIP;
			UI::GUILabel *lblTimeDNS;
			UI::GUITextBox *txtTimeDNS;
			UI::GUILabel *lblTimeConn;
			UI::GUITextBox *txtTimeConn;
			UI::GUILabel *lblTimeSendHdr;
			UI::GUITextBox *txtTimeSendHdr;
			UI::GUILabel *lblTimeResp;
			UI::GUITextBox *txtTimeResp;
			UI::GUILabel *lblTimeTotal;
			UI::GUITextBox *txtTimeTotal;
			UI::GUILabel *lblRespStatus;
			UI::GUITextBox *txtRespStatus;
			UI::GUILabel *lblRespDLSize;
			UI::GUITextBox *txtRespDLSize;
			UI::GUILabel *lblRespULSize;
			UI::GUITextBox *txtRespULSize;
			UI::GUILabel *lblRespTransfSize;
			UI::GUITextBox *txtRespTransfSize;
			UI::GUILabel *lblRespContSize;
			UI::GUITextBox *txtRespContSize;
			UI::GUIListView *lvHeaders;
			UI::GUIPanel *pnlControl;
			UI::GUIButton *btnSave;
			UI::GUIButton *btnView;

			UI::GUITabPage *tpCert;
			UI::GUIPanel *pnlCert;
			UI::GUIButton *btnCert;
			UI::GUITextBox *txtCert;

			UI::GUITabPage *tpCookie;
			UI::GUIListView *lvCookie;

			NotNullPtr<Net::SocketFactory> sockf;
			Net::SSLEngine *ssl;
			Bool threadRunning;
			Bool threadToStop;
			Sync::Event threadEvt;
			Text::String *reqURL;
			const UTF8Char *reqBody;
			UOSInt reqBodyLen;
			Text::String *reqBodyType;
			Text::String *reqUserName;
			Text::String *reqPassword;
			Text::String *reqHeaders;
			Net::WebUtil::RequestMethod reqMeth;
			Bool reqOSClient;
			Bool reqAllowComp;
			Bool noShutdown;
			NotNullPtr<Text::String> userAgent;
			Crypto::Cert::X509Cert *cliCert;
			Crypto::Cert::X509File *cliKey;

			Bool respChanged;
			Net::SocketUtil::AddressInfo respSvrAddr;
			Double respTimeDNS;
			Double respTimeConn;
			Double respTimeReq;
			Double respTimeResp;
			Double respTimeTotal;
			UInt64 respTransfSize;
			UInt64 respDLSize;
			UInt64 respULSize;
			Int32 respStatus;
			Text::String *respReqURL;
			Data::ArrayListNN<Text::String> respHeaders;
			Sync::Mutex respMut;
			Text::String *respContType;
			IO::MemoryStream *respData;
			Text::String *respCertText;
			Crypto::Cert::X509File *respCert;
			Data::ArrayList<ParamValue*> params;
			Data::ArrayList<HTTPCookie *> cookieList;
			Sync::Mutex cookieMut;
			Data::ArrayListNN<Text::String> fileList;

			static void __stdcall OnUserAgentClicked(void *userObj);
			static void __stdcall OnRequestClicked(void *userObj);
			static void __stdcall OnSaveClicked(void *userObj);
			static void __stdcall OnViewClicked(void *userObj);
			static void __stdcall OnDataStrClicked(void *userObj);
			static void __stdcall OnFileSelectClicked(void *userObj);
			static void __stdcall OnFileClearClicked(void *userObj);
			static void __stdcall OnCertClicked(void *userObj);
			static void __stdcall OnClientCertClicked(void *userObj);
			static UInt32 __stdcall ProcessThread(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			void ClearHeaders();
			void ClearParams();
			void ClearCookie();
			void ClearFiles();
			HTTPCookie *SetCookie(Text::CString cookieStr, Text::CString reqURL);
			UTF8Char *AppendCookie(UTF8Char *sbuff, Text::CString reqURL);
		public:
			AVIRHTTPClientForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRHTTPClientForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
