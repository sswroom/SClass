#ifndef _SM_SSWR_AVIREAD_AVIRSNMPTRAPMONITORFORM
#define _SM_SSWR_AVIREAD_AVIRSNMPTRAPMONITORFORM
#include "Net/SNMPTrapMonitor.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSNMPTrapMonitorForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				Net::SocketUtil::AddressInfo addr;
				UInt16 port;
				Int64 t;
				Net::SNMPUtil::TrapInfo trap;
				Data::ArrayList<Net::SNMPUtil::BindingItem*> *itemList;
			} SNMPPacket;
			
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Net::SNMPTrapMonitor *mon;
			Sync::Mutex packetMut;
			Data::ArrayList<SNMPPacket*> packetList;

			NN<UI::GUIListBox> lbResults;
			NN<UI::GUIHSplitter> hspResults;
			NN<UI::GUIPanel> pnlResults;
			NN<UI::GUIPanel> pnlTrap;
			NN<UI::GUILabel> lblCommunity;
			NN<UI::GUITextBox> txtCommunity;
			NN<UI::GUILabel> lblEnterpriseOID;
			NN<UI::GUITextBox> txtEnterpriseOID;
			NN<UI::GUILabel> lblEnterpriseName;
			NN<UI::GUITextBox> txtEnterpriseName;
			NN<UI::GUILabel> lblRemoteIP;
			NN<UI::GUITextBox> txtRemoteIP;
			NN<UI::GUILabel> lblRemotePort;
			NN<UI::GUITextBox> txtRemotePort;
			NN<UI::GUILabel> lblAgentAddr;
			NN<UI::GUITextBox> txtAgentAddr;
			NN<UI::GUILabel> lblGenericTrap;
			NN<UI::GUITextBox> txtGenericTrap;
			NN<UI::GUILabel> lblSpecificTrap;
			NN<UI::GUITextBox> txtSpecificTrap;
			NN<UI::GUILabel> lblRecvTime;
			NN<UI::GUITextBox> txtRecvTime;
			NN<UI::GUILabel> lblTrapTime;
			NN<UI::GUITextBox> txtTrapTime;
			NN<UI::GUIListView> lvResults;

			static void __stdcall OnResultSelChg(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static Bool __stdcall OnSNMPTrapPacket(AnyType userObj, NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, NN<const Net::SNMPUtil::TrapInfo> trap, NN<Data::ArrayList<Net::SNMPUtil::BindingItem*>> itemList);

		public:
			AVIRSNMPTrapMonitorForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSNMPTrapMonitorForm();

			virtual void OnMonitorChanged();

			Bool IsError();
		};
	}
}
#endif
