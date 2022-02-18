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
				Text::String *name;
				Text::String *value;
			} ParamValue;

			typedef struct
			{
				Text::String *name;
				Text::String *value;
				Text::String *domain;
				Text::String *path;
				Int64 expireTime;
				Bool secure;
			} HTTPCookie;
			
			
		private:
			SSWR::AVIRead::AVIRCore *core;

			UI::GUITabControl *tcMain;

			UI::GUITabPage *tpRequest;
			UI::GUIPanel *pnlRequest;
			UI::GUILabel *lblURL;
			UI::GUITextBox *txtURL;
			UI::GUICheckBox *chkNoShutdown;
			UI::GUILabel *lblMethod;
			UI::GUIComboBox *cboMethod;
			UI::GUICheckBox *chkOSClient;
			UI::GUIButton *btnUserAgent;
			UI::GUILabel *lblUserAgent;
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
			UI::GUILabel *lblRespSize;
			UI::GUITextBox *txtRespSize;
			UI::GUIListView *lvHeaders;
			UI::GUIPanel *pnlControl;
			UI::GUIButton *btnSave;
			UI::GUIButton *btnView;

			UI::GUITabPage *tpCert;
			UI::GUITextBox *txtCert;

			UI::GUITabPage *tpCookie;
			UI::GUIListView *lvCookie;

			Net::SocketFactory *sockf;
			Net::SSLEngine *ssl;
			Bool threadRunning;
			Bool threadToStop;
			Sync::Event *threadEvt;
			Text::String *reqURL;
			const UTF8Char *reqBody;
			UOSInt reqBodyLen;
			Text::String *reqBodyType;
			Text::String *reqUserName;
			Text::String *reqPassword;
			Text::String *reqHeaders;
			Net::WebUtil::RequestMethod reqMeth;
			Bool reqOSClient;
			Bool noShutdown;
			Text::String *userAgent;

			Bool respChanged;
			Net::SocketUtil::AddressInfo respSvrAddr;
			Double respTimeDNS;
			Double respTimeConn;
			Double respTimeReq;
			Double respTimeResp;
			Double respTimeTotal;
			UInt64 respSize;
			Int32 respStatus;
			Text::String *respReqURL;
			Data::ArrayList<Text::String*> *respHeaders;
			Sync::Mutex *respMut;
			Text::String *respContType;
			IO::MemoryStream *respData;
			Text::String *respCert;
			Data::ArrayList<ParamValue*> *params;
			Data::ArrayList<HTTPCookie *> *cookieList;
			Sync::Mutex *cookieMut;
			Data::ArrayList<Text::String*> *fileList;

			static void __stdcall OnUserAgentClicked(void *userObj);
			static void __stdcall OnRequestClicked(void *userObj);
			static void __stdcall OnSaveClicked(void *userObj);
			static void __stdcall OnViewClicked(void *userObj);
			static void __stdcall OnDataStrClicked(void *userObj);
			static void __stdcall OnFileSelectClicked(void *userObj);
			static void __stdcall OnFileClearClicked(void *userObj);
			static UInt32 __stdcall ProcessThread(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			void ClearHeaders();
			void ClearParams();
			void ClearCookie();
			void ClearFiles();
			HTTPCookie *SetCookie(Text::CString cookieStr, Text::CString reqURL);
			UTF8Char *AppendCookie(UTF8Char *sbuff, const UTF8Char *reqURL, UOSInt urlLen);
		public:
			AVIRHTTPClientForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRHTTPClientForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
