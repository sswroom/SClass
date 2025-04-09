#ifndef _SM_SSWR_AVIREAD_AVIRHTTPPROXYCLIENTFORM
#define _SM_SSWR_AVIREAD_AVIRHTTPPROXYCLIENTFORM
#include "AnyType.h"
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
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUIPanel> pnlRequest;
			NN<UI::GUILabel> lblURL;
			NN<UI::GUITextBox> txtURL;
			NN<UI::GUILabel> lblProxySvr;
			NN<UI::GUITextBox> txtProxySvr;
			NN<UI::GUILabel> lblProxyPort;
			NN<UI::GUITextBox> txtProxyPort;
			NN<UI::GUILabel> lblProxyUser;
			NN<UI::GUITextBox> txtProxyUser;
			NN<UI::GUILabel> lblProxyPwd;
			NN<UI::GUITextBox> txtProxyPwd;
			NN<UI::GUIButton> btnRequest;
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
			NN<UI::GUILabel> lblStatus;
			NN<UI::GUITextBox> txtStatus;
			NN<UI::GUILabel> lblDownSize;
			NN<UI::GUITextBox> txtDownSize;
			NN<UI::GUIListView> lvHeaders;

			NN<Net::SocketFactory> sockf;
			Bool threadRunning;
			Bool threadToStop;
			NN<Sync::Event> threadEvt;
			Optional<Text::String> reqURL;
			UInt32 proxyIP;
			UInt16 proxyPort;
			Optional<Text::String> reqUser;
			Optional<Text::String> reqPwd;

			Bool respChanged;
			Net::SocketUtil::AddressInfo respSvrAddr;
			Double respTimeDNS;
			Double respTimeConn;
			Double respTimeReq;
			Double respTimeResp;
			Double respTimeTotal;
			Net::WebStatus::StatusCode respStatus;
			UInt64 respDownSize;
			Data::ArrayListStringNN respHeaders;

			static void __stdcall OnRequestClicked(AnyType userObj);
			static UInt32 __stdcall ProcessThread(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			void ClearHeaders();
		public:
			AVIRHTTPProxyClientForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRHTTPProxyClientForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
