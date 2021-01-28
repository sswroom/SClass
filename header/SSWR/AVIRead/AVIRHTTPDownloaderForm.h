#ifndef _SM_SSWR_AVIREAD_AVIRHTTPDOWNLOADERFORM
#define _SM_SSWR_AVIREAD_AVIRHTTPDOWNLOADERFORM

#include "Net/HTTPClient.h"
#include "SSWR/AVIRead/AVIRCore.h"
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
			SSWR::AVIRead::AVIRCore *core;

			UI::GUIPanel *pnlRequest;
			UI::GUILabel *lblURL;
			UI::GUITextBox *txtURL;
			UI::GUILabel *lblDownloadDir;
			UI::GUITextBox *txtDownloadDir;
			UI::GUILabel *lblHeaders;
			UI::GUITextBox *txtHeaders;
			UI::GUIButton *btnRequest;
			UI::GUIGroupBox *grpStatus;
			UI::GUILabel *lblCurrSpeed;
			UI::GUITextBox *txtCurrSpeed;
			UI::GUILabel *lblTotalSize;
			UI::GUITextBox *txtTotalSize;
			UI::GUIGroupBox *grpResponse;
			UI::GUIPanel *pnlResponse;
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

			Net::SocketFactory *sockf;
			Bool threadRunning;
			Bool threadToStop;
			Sync::Event *threadEvt;
			const UTF8Char *reqURL;
			const UTF8Char *reqHeader;
			const UTF8Char *downPath;

			Int64 lastT;
			Int64 lastSize;
			Int64 currSize;

			Bool respChanged;
			Bool respHdrChanged;
			Net::SocketUtil::AddressInfo respSvrAddr;
			Double respTimeDNS;
			Double respTimeConn;
			Double respTimeReq;
			Double respTimeResp;
			Double respTimeTotal;
			Data::ArrayList<const UTF8Char*> *respHeaders;

			static void __stdcall OnRequestClicked(void *userObj);
			static UInt32 __stdcall ProcessThread(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			void ClearHeaders();
		public:
			AVIRHTTPDownloaderForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRHTTPDownloaderForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
