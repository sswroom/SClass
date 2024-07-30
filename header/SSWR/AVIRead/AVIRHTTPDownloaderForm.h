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
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUIPanel> pnlRequest;
			NN<UI::GUILabel> lblURL;
			NN<UI::GUITextBox> txtURL;
			NN<UI::GUILabel> lblDownloadDir;
			NN<UI::GUITextBox> txtDownloadDir;
			NN<UI::GUILabel> lblHeaders;
			NN<UI::GUITextBox> txtHeaders;
			NN<UI::GUIButton> btnRequest;
			NN<UI::GUIGroupBox> grpStatus;
			NN<UI::GUILabel> lblCurrSpeed;
			NN<UI::GUITextBox> txtCurrSpeed;
			NN<UI::GUILabel> lblTotalSize;
			NN<UI::GUITextBox> txtTotalSize;
			NN<UI::GUIGroupBox> grpResponse;
			NN<UI::GUIPanel> pnlResponse;
			NN<UI::GUILabel> lblSvrIP;
			NN<UI::GUITextBox> txtSvrIP;
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
			NN<UI::GUIListView> lvHeaders;

			NN<Net::TCPClientFactory> clif;
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
			static UInt32 __stdcall ProcessThread(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			void ClearHeaders();
		public:
			AVIRHTTPDownloaderForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRHTTPDownloaderForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
