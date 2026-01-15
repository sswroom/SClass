#ifndef _SM_SSWR_AVIREAD_AVIRUDPLOADBALANCERFORM
#define _SM_SSWR_AVIREAD_AVIRUDPLOADBALANCERFORM
#include "Net/UDPServer.h"
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
		class AVIRUDPLoadBalancerForm : public UI::GUIForm
		{
		private:
			struct UDPSession
			{
				Net::SocketUtil::AddressInfo sourceAddr;
				UInt16 sourcePort;
				Net::SocketUtil::AddressInfo targetAddr;
				UInt16 targetPort;
				Data::Timestamp lastDataTime;
				Data::Timestamp sessCreatedTime;
				NN<Net::UDPServer> targetUDP;
				NN<AVIRUDPLoadBalancerForm> me;
				Bool displayed;
			};
			
			struct UDPTarget
			{
				Net::SocketUtil::AddressInfo targetAddr;
				UInt16 targetPort;
			};
		private:
			Bool stopping;
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Net::SocketFactory> sockf;
			Optional<Net::UDPServer> sourceUDP;
			Sync::Mutex sessMut;
			Data::ArrayListNN<UDPSession> sessList;
			UIntOS nextTarget;
			UIntOS dispNextTarget;
			Data::ArrayListNN<UDPTarget> targetList;
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

			static void __stdcall OnSourceUDPPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData);
			static void __stdcall OnTargetUDPPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData);
			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall OnTargetAddClicked(AnyType userObj);
			static void __stdcall OnTargetDelClicked(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall FreeSession(NN<UDPSession> sess);
		public:
			AVIRUDPLoadBalancerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRUDPLoadBalancerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
