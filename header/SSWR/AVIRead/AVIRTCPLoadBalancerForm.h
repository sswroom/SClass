#ifndef _SM_SSWR_AVIREAD_AVIRTCPLOADBALANCERFORM
#define _SM_SSWR_AVIREAD_AVIRTCPLOADBALANCERFORM
#include "Net/TCPClientMgr.h"
#include "Net/TCPServer.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRTCPLoadBalancerForm : public UI::GUIForm
		{
		private:
			struct TCPSession
			{
				Data::Timestamp lastDataTime;
				Data::Timestamp sessCreatedTime;
				NN<Net::TCPClient> sourceCli;
				NN<Net::TCPClient> targetCli;
				NN<AVIRTCPLoadBalancerForm> me;
				UInt64 sourceDataSize;
				UInt64 targetDataSize;
				Bool displayed;
				Sync::Mutex mut;
				Bool sourceClosed;
				Bool targetClosed;
				Bool notUsed;
			};
			
			struct TCPTarget
			{
				Net::SocketUtil::AddressInfo targetAddr;
				UInt16 targetPort;
			};
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Net::SocketFactory> sockf;
			Optional<Net::TCPServer> sourceSvr;
			NN<Net::TCPClientMgr> sourceCliMgr;
			NN<Net::TCPClientMgr> targetCliMgr;
			Sync::Mutex sessMut;
			Data::ArrayListNN<TCPSession> sessList;
			UIntOS nextTarget;
			UIntOS dispNextTarget;
			Data::ArrayListNN<TCPTarget> targetList;
			IO::LogTool log;
			Data::Duration sessTimeout;

			NN<UI::GUIPanel> pnlListener;
			NN<UI::GUILabel> lblListenerPort;
			NN<UI::GUITextBox> txtListenerPort;
			NN<UI::GUILabel> lblTimeout;
			NN<UI::GUITextBox> txtTimeout;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUITabControl> tcMain;
			
			NN<UI::GUITabPage> tpTarget;
			NN<UI::GUIPanel> pnlTarget;
			NN<UI::GUIListView> lvTarget;
			NN<UI::GUILabel> lblTargetIP;
			NN<UI::GUITextBox> txtTargetIP;
			NN<UI::GUILabel> lblTargetPort;
			NN<UI::GUITextBox> txtTargetPort;
			NN<UI::GUIButton> btnTargetAdd;
			NN<UI::GUIButton> btnTargetDel;

			NN<UI::GUITabPage> tpSession;
			NN<UI::GUIListView> lvSession;

			static void __stdcall OnClientConnect(NN<Socket> s, AnyType userObj);
			static void __stdcall OnSourceEvent(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, Net::TCPClientMgr::TCPEventType evtType);
			static void __stdcall OnSourceData(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, const Data::ByteArrayR &buff);
			static void __stdcall OnSourceTimeout(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData);
			static void __stdcall OnTargetEvent(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, Net::TCPClientMgr::TCPEventType evtType);
			static void __stdcall OnTargetData(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, const Data::ByteArrayR &buff);
			static void __stdcall OnTargetTimeout(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData);
			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall OnTargetAddClicked(AnyType userObj);
			static void __stdcall OnTargetDelClicked(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall EndSession(NN<TCPSession> sess);
			static void __stdcall FreeSession(NN<TCPSession> sess);
		public:
			AVIRTCPLoadBalancerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRTCPLoadBalancerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
