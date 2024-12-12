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
				NN<Text::String> name;
				NN<Text::String> value;
			} ParamValue;

			typedef struct
			{
				NN<Text::String> name;
				NN<Text::String> value;
				NN<Text::String> domain;
				Optional<Text::String> path;
				Int64 expireTime;
				Bool secure;
			} HTTPCookie;
			
			
		private:
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpRequest;
			NN<UI::GUIPanel> pnlRequest;
			NN<UI::GUILabel> lblURL;
			NN<UI::GUITextBox> txtURL;
			NN<UI::GUICheckBox> chkNoShutdown;
			NN<UI::GUILabel> lblMethod;
			NN<UI::GUIComboBox> cboMethod;
			NN<UI::GUICheckBox> chkOSClient;
			NN<UI::GUICheckBox> chkAllowComp;
			NN<UI::GUIButton> btnUserAgent;
			NN<UI::GUILabel> lblUserAgent;
			NN<UI::GUIButton> btnClientCert;
			NN<UI::GUILabel> lblClientCert;
			NN<UI::GUILabel> lblUserName;
			NN<UI::GUITextBox> txtUserName;
			NN<UI::GUILabel> lblPassword;
			NN<UI::GUITextBox> txtPassword;
			NN<UI::GUILabel> lblFileUpload;
			NN<UI::GUITextBox> txtFileFormName;
			NN<UI::GUIButton> btnFileSelect;
			NN<UI::GUIButton> btnFileClear;
			NN<UI::GUILabel> lblFileStatus;
			NN<UI::GUILabel> lblDataStr;
			NN<UI::GUITextBox> txtDataStr;
			NN<UI::GUIButton> btnDataStr;
			NN<UI::GUILabel> lblPostFormat;
			NN<UI::GUIComboBox> cboPostFormat;
			NN<UI::GUILabel> lblHeaders;
			NN<UI::GUITextBox> txtHeaders;
			NN<UI::GUIButton> btnRequest;
			NN<UI::GUIListView> lvReqData;

			NN<UI::GUITabPage> tpResponse;
			NN<UI::GUIPanel> pnlResponse;
			NN<UI::GUILabel> lblReqURL;
			NN<UI::GUITextBox> txtReqURL;
			NN<UI::GUILabel> lblSvrIP;
			NN<UI::GUITextBox> txtSvrIP;
			NN<UI::GUILabel> lblStartTime;
			NN<UI::GUITextBox> txtStartTime;
			NN<UI::GUILabel> lblTimeDNS;
			NN<UI::GUITextBox> txtTimeDNS;
			NN<UI::GUILabel> lblTimeConn;
			NN<UI::GUITextBox> txtTimeConn;
			NN<UI::GUILabel> lblTimeSendHdr;
			NN<UI::GUITextBox> txtTimeSendHdr;
			NN<UI::GUILabel> lblTimeResp;
			NN<UI::GUITextBox> txtTimeResp;
			NN<UI::GUILabel> lblTimeTotal;
			NN<UI::GUITextBox> txtTimeTotal;
			NN<UI::GUILabel> lblRespStatus;
			NN<UI::GUITextBox> txtRespStatus;
			NN<UI::GUILabel> lblRespDLSize;
			NN<UI::GUITextBox> txtRespDLSize;
			NN<UI::GUILabel> lblRespULSize;
			NN<UI::GUITextBox> txtRespULSize;
			NN<UI::GUILabel> lblRespTransfSize;
			NN<UI::GUITextBox> txtRespTransfSize;
			NN<UI::GUILabel> lblRespContSize;
			NN<UI::GUITextBox> txtRespContSize;
			NN<UI::GUIListView> lvHeaders;
			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUIButton> btnSave;
			NN<UI::GUIButton> btnView;

			NN<UI::GUITabPage> tpCert;
			NN<UI::GUIPanel> pnlCert;
			NN<UI::GUIButton> btnCert;
			NN<UI::GUITextBox> txtCert;

			NN<UI::GUITabPage> tpCookie;
			NN<UI::GUIListView> lvCookie;

			NN<Net::TCPClientFactory> clif;
			Optional<Net::SSLEngine> ssl;
			Text::String *reqURL;
			UnsafeArrayOpt<const UTF8Char> reqBody;
			UOSInt reqBodyLen;
			Text::String *reqBodyType;
			Text::String *reqUserName;
			Text::String *reqPassword;
			Text::String *reqHeaders;
			Net::WebUtil::RequestMethod reqMeth;
			Bool reqOSClient;
			Bool reqAllowComp;
			Bool noShutdown;
			NN<Text::String> userAgent;
			Optional<Crypto::Cert::X509Cert> cliCert;
			Optional<Crypto::Cert::X509File> cliKey;

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
			Optional<Crypto::Cert::X509File> respCert;
			Data::ArrayListNN<ParamValue> params;
			Data::ArrayListNN<HTTPCookie> cookieList;
			Sync::Mutex cookieMut;
			Data::ArrayListStringNN fileList;
			Sync::Thread procThread;

			static void __stdcall OnUserAgentClicked(AnyType userObj);
			static void __stdcall OnRequestClicked(AnyType userObj);
			static void __stdcall OnSaveClicked(AnyType userObj);
			static void __stdcall OnViewClicked(AnyType userObj);
			static void __stdcall OnDataStrClicked(AnyType userObj);
			static void __stdcall OnFileSelectClicked(AnyType userObj);
			static void __stdcall OnFileClearClicked(AnyType userObj);
			static void __stdcall OnCertClicked(AnyType userObj);
			static void __stdcall OnClientCertClicked(AnyType userObj);
			static void __stdcall ProcessThread(NN<Sync::Thread> thread);
			static void __stdcall OnTimerTick(AnyType userObj);
			void ClearHeaders();
			void ClearParams();
			void ClearCookie();
			void ClearFiles();
			Optional<HTTPCookie> SetCookie(Text::CStringNN cookieStr, Text::CStringNN reqURL);
			UnsafeArrayOpt<UTF8Char> AppendCookie(UnsafeArray<UTF8Char> sbuff, Text::CStringNN reqURL);
		public:
			AVIRHTTPClientForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRHTTPClientForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
