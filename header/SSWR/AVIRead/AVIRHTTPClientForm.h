#ifndef _SM_SSWR_AVIREAD_AVIRHTTPCLIENTFORM
#define _SM_SSWR_AVIREAD_AVIRHTTPCLIENTFORM
#include "Data/ArrayList.h"
#include "IO/MemoryStream.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "Sync/Thread.h"
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

			NotNullPtr<UI::GUITabPage> tpRequest;
			NotNullPtr<UI::GUIPanel> pnlRequest;
			NotNullPtr<UI::GUILabel> lblURL;
			NotNullPtr<UI::GUITextBox> txtURL;
			UI::GUICheckBox *chkNoShutdown;
			NotNullPtr<UI::GUILabel> lblMethod;
			NotNullPtr<UI::GUIComboBox> cboMethod;
			UI::GUICheckBox *chkOSClient;
			UI::GUICheckBox *chkAllowComp;
			NotNullPtr<UI::GUIButton> btnUserAgent;
			NotNullPtr<UI::GUILabel> lblUserAgent;
			NotNullPtr<UI::GUIButton> btnClientCert;
			NotNullPtr<UI::GUILabel> lblClientCert;
			NotNullPtr<UI::GUILabel> lblUserName;
			NotNullPtr<UI::GUITextBox> txtUserName;
			NotNullPtr<UI::GUILabel> lblPassword;
			NotNullPtr<UI::GUITextBox> txtPassword;
			NotNullPtr<UI::GUILabel> lblFileUpload;
			NotNullPtr<UI::GUITextBox> txtFileFormName;
			NotNullPtr<UI::GUIButton> btnFileSelect;
			NotNullPtr<UI::GUIButton> btnFileClear;
			NotNullPtr<UI::GUILabel> lblFileStatus;
			NotNullPtr<UI::GUILabel> lblDataStr;
			NotNullPtr<UI::GUITextBox> txtDataStr;
			NotNullPtr<UI::GUIButton> btnDataStr;
			NotNullPtr<UI::GUILabel> lblPostFormat;
			NotNullPtr<UI::GUIComboBox> cboPostFormat;
			NotNullPtr<UI::GUILabel> lblHeaders;
			NotNullPtr<UI::GUITextBox> txtHeaders;
			NotNullPtr<UI::GUIButton> btnRequest;
			UI::GUIListView *lvReqData;

			NotNullPtr<UI::GUITabPage> tpResponse;
			NotNullPtr<UI::GUIPanel> pnlResponse;
			NotNullPtr<UI::GUILabel> lblReqURL;
			NotNullPtr<UI::GUITextBox> txtReqURL;
			NotNullPtr<UI::GUILabel> lblSvrIP;
			NotNullPtr<UI::GUITextBox> txtSvrIP;
			NotNullPtr<UI::GUILabel> lblStartTime;
			NotNullPtr<UI::GUITextBox> txtStartTime;
			NotNullPtr<UI::GUILabel> lblTimeDNS;
			NotNullPtr<UI::GUITextBox> txtTimeDNS;
			NotNullPtr<UI::GUILabel> lblTimeConn;
			NotNullPtr<UI::GUITextBox> txtTimeConn;
			NotNullPtr<UI::GUILabel> lblTimeSendHdr;
			NotNullPtr<UI::GUITextBox> txtTimeSendHdr;
			NotNullPtr<UI::GUILabel> lblTimeResp;
			NotNullPtr<UI::GUITextBox> txtTimeResp;
			NotNullPtr<UI::GUILabel> lblTimeTotal;
			NotNullPtr<UI::GUITextBox> txtTimeTotal;
			NotNullPtr<UI::GUILabel> lblRespStatus;
			NotNullPtr<UI::GUITextBox> txtRespStatus;
			NotNullPtr<UI::GUILabel> lblRespDLSize;
			NotNullPtr<UI::GUITextBox> txtRespDLSize;
			NotNullPtr<UI::GUILabel> lblRespULSize;
			NotNullPtr<UI::GUITextBox> txtRespULSize;
			NotNullPtr<UI::GUILabel> lblRespTransfSize;
			NotNullPtr<UI::GUITextBox> txtRespTransfSize;
			NotNullPtr<UI::GUILabel> lblRespContSize;
			NotNullPtr<UI::GUITextBox> txtRespContSize;
			UI::GUIListView *lvHeaders;
			NotNullPtr<UI::GUIPanel> pnlControl;
			NotNullPtr<UI::GUIButton> btnSave;
			NotNullPtr<UI::GUIButton> btnView;

			NotNullPtr<UI::GUITabPage> tpCert;
			NotNullPtr<UI::GUIPanel> pnlCert;
			NotNullPtr<UI::GUIButton> btnCert;
			NotNullPtr<UI::GUITextBox> txtCert;

			NotNullPtr<UI::GUITabPage> tpCookie;
			UI::GUIListView *lvCookie;

			NotNullPtr<Net::SocketFactory> sockf;
			Optional<Net::SSLEngine> ssl;
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
			Data::Timestamp respTimeStart;
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
			Data::ArrayListStringNN respHeaders;
			Sync::Mutex respMut;
			Text::String *respContType;
			IO::MemoryStream *respData;
			Text::String *respCertText;
			Crypto::Cert::X509File *respCert;
			Data::ArrayList<ParamValue*> params;
			Data::ArrayList<HTTPCookie *> cookieList;
			Sync::Mutex cookieMut;
			Data::ArrayListStringNN fileList;
			Sync::Thread procThread;

			static void __stdcall OnUserAgentClicked(void *userObj);
			static void __stdcall OnRequestClicked(void *userObj);
			static void __stdcall OnSaveClicked(void *userObj);
			static void __stdcall OnViewClicked(void *userObj);
			static void __stdcall OnDataStrClicked(void *userObj);
			static void __stdcall OnFileSelectClicked(void *userObj);
			static void __stdcall OnFileClearClicked(void *userObj);
			static void __stdcall OnCertClicked(void *userObj);
			static void __stdcall OnClientCertClicked(void *userObj);
			static void __stdcall ProcessThread(NotNullPtr<Sync::Thread> thread);
			static void __stdcall OnTimerTick(void *userObj);
			void ClearHeaders();
			void ClearParams();
			void ClearCookie();
			void ClearFiles();
			HTTPCookie *SetCookie(Text::CStringNN cookieStr, Text::CStringNN reqURL);
			UTF8Char *AppendCookie(UTF8Char *sbuff, Text::CStringNN reqURL);
		public:
			AVIRHTTPClientForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRHTTPClientForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
