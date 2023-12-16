#ifndef _SM_SSWR_AVIREAD_AVIRHTTPDOWNLOADERFORM
#define _SM_SSWR_AVIREAD_AVIRHTTPDOWNLOADERFORM

#include "Net/HTTPClient.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "Text/String.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRHTTPDownloaderForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			NotNullPtr<UI::GUIPanel> pnlRequest;
			UI::GUILabel *lblURL;
			UI::GUITextBox *txtURL;
			UI::GUILabel *lblDownloadDir;
			UI::GUITextBox *txtDownloadDir;
			UI::GUILabel *lblHeaders;
			UI::GUITextBox *txtHeaders;
			UI::GUIButton *btnRequest;
			NotNullPtr<UI::GUIGroupBox> grpStatus;
			UI::GUILabel *lblCurrSpeed;
			UI::GUITextBox *txtCurrSpeed;
			UI::GUILabel *lblTotalSize;
			UI::GUITextBox *txtTotalSize;
			NotNullPtr<UI::GUIGroupBox> grpResponse;
			NotNullPtr<UI::GUIPanel> pnlResponse;
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
			UI::GUIListView *lvHeaders;

			NotNullPtr<Net::SocketFactory> sockf;
			Optional<Net::SSLEngine> ssl;
			Bool threadRunning;
			Bool threadToStop;
			Sync::Event threadEvt;
			Text::String *reqURL;
			Text::String *reqHeader;
			Text::String *downPath;

			Int64 lastT;
			UInt64 lastSize;
			UInt64 currSize;

			Bool respChanged;
			Bool respHdrChanged;
			Net::SocketUtil::AddressInfo respSvrAddr;
			Double respTimeDNS;
			Double respTimeConn;
			Double respTimeReq;
			Double respTimeResp;
			Double respTimeTotal;
			Data::ArrayListStringNN respHeaders;

			static void __stdcall OnRequestClicked(void *userObj);
			static UInt32 __stdcall ProcessThread(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			void ClearHeaders();
		public:
			AVIRHTTPDownloaderForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRHTTPDownloaderForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
