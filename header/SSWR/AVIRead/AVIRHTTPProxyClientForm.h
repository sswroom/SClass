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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			NotNullPtr<UI::GUIPanel> pnlRequest;
			NotNullPtr<UI::GUILabel> lblURL;
			NotNullPtr<UI::GUITextBox> txtURL;
			NotNullPtr<UI::GUILabel> lblProxySvr;
			NotNullPtr<UI::GUITextBox> txtProxySvr;
			NotNullPtr<UI::GUILabel> lblProxyPort;
			NotNullPtr<UI::GUITextBox> txtProxyPort;
			NotNullPtr<UI::GUIButton> btnRequest;
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
			Data::ArrayListStringNN respHeaders;

			static void __stdcall OnRequestClicked(void *userObj);
			static UInt32 __stdcall ProcessThread(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			void ClearHeaders();
		public:
			AVIRHTTPProxyClientForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRHTTPProxyClientForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
