#ifndef _SM_SSWR_AVIREAD_AVIRHTTPPROXYCLIENTFORM
#define _SM_SSWR_AVIREAD_AVIRHTTPPROXYCLIENTFORM
#include "Data/ArrayListNN.h"
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
		class AVIRHTTPProxyClientForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;

			UI::GUIPanel *pnlRequest;
			UI::GUILabel *lblURL;
			UI::GUITextBox *txtURL;
			UI::GUILabel *lblProxySvr;
			UI::GUITextBox *txtProxySvr;
			UI::GUILabel *lblProxyPort;
			UI::GUITextBox *txtProxyPort;
			UI::GUIButton *btnRequest;
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

			NotNullPtr<Net::SocketFactory> sockf;
			Bool threadRunning;
			Bool threadToStop;
			Sync::Event *threadEvt;
			Text::String *reqURL;
			UInt32 proxyIP;
			UInt16 proxyPort;

			Bool respChanged;
			Net::SocketUtil::AddressInfo respSvrAddr;
			Double respTimeDNS;
			Double respTimeConn;
			Double respTimeReq;
			Double respTimeResp;
			Double respTimeTotal;
			Data::ArrayListNN<Text::String> respHeaders;

			static void __stdcall OnRequestClicked(void *userObj);
			static UInt32 __stdcall ProcessThread(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			void ClearHeaders();
		public:
			AVIRHTTPProxyClientForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRHTTPProxyClientForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
