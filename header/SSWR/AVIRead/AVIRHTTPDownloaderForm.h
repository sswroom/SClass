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
			NotNullPtr<UI::GUILabel> lblURL;
			UI::GUITextBox *txtURL;
			NotNullPtr<UI::GUILabel> lblDownloadDir;
			UI::GUITextBox *txtDownloadDir;
			NotNullPtr<UI::GUILabel> lblHeaders;
			UI::GUITextBox *txtHeaders;
			NotNullPtr<UI::GUIButton> btnRequest;
			NotNullPtr<UI::GUIGroupBox> grpStatus;
			NotNullPtr<UI::GUILabel> lblCurrSpeed;
			UI::GUITextBox *txtCurrSpeed;
			NotNullPtr<UI::GUILabel> lblTotalSize;
			UI::GUITextBox *txtTotalSize;
			NotNullPtr<UI::GUIGroupBox> grpResponse;
			NotNullPtr<UI::GUIPanel> pnlResponse;
			NotNullPtr<UI::GUILabel> lblSvrIP;
			UI::GUITextBox *txtSvrIP;
			NotNullPtr<UI::GUILabel> lblTimeDNS;
			UI::GUITextBox *txtTimeDNS;
			NotNullPtr<UI::GUILabel> lblTimeConn;
			UI::GUITextBox *txtTimeConn;
			NotNullPtr<UI::GUILabel> lblTimeSendHdr;
			UI::GUITextBox *txtTimeSendHdr;
			NotNullPtr<UI::GUILabel> lblTimeResp;
			UI::GUITextBox *txtTimeResp;
			NotNullPtr<UI::GUILabel> lblTimeTotal;
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
