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
			NotNullPtr<UI::GUITextBox> txtURL;
			NotNullPtr<UI::GUILabel> lblDownloadDir;
			NotNullPtr<UI::GUITextBox> txtDownloadDir;
			NotNullPtr<UI::GUILabel> lblHeaders;
			NotNullPtr<UI::GUITextBox> txtHeaders;
			NotNullPtr<UI::GUIButton> btnRequest;
			NotNullPtr<UI::GUIGroupBox> grpStatus;
			NotNullPtr<UI::GUILabel> lblCurrSpeed;
			NotNullPtr<UI::GUITextBox> txtCurrSpeed;
			NotNullPtr<UI::GUILabel> lblTotalSize;
			NotNullPtr<UI::GUITextBox> txtTotalSize;
			NotNullPtr<UI::GUIGroupBox> grpResponse;
			NotNullPtr<UI::GUIPanel> pnlResponse;
			NotNullPtr<UI::GUILabel> lblSvrIP;
			NotNullPtr<UI::GUITextBox> txtSvrIP;
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
			NotNullPtr<UI::GUIListView> lvHeaders;

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

			static void __stdcall OnRequestClicked(AnyType userObj);
			static UInt32 __stdcall ProcessThread(void *userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			void ClearHeaders();
		public:
			AVIRHTTPDownloaderForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRHTTPDownloaderForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
