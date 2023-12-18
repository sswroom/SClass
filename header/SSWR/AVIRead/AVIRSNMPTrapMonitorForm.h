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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Net::SNMPTrapMonitor *mon;
			Sync::Mutex packetMut;
			Data::ArrayList<SNMPPacket*> packetList;

			UI::GUIListBox *lbResults;
			NotNullPtr<UI::GUIHSplitter> hspResults;
			NotNullPtr<UI::GUIPanel> pnlResults;
			NotNullPtr<UI::GUIPanel> pnlTrap;
			NotNullPtr<UI::GUILabel> lblCommunity;
			UI::GUITextBox *txtCommunity;
			NotNullPtr<UI::GUILabel> lblEnterpriseOID;
			UI::GUITextBox *txtEnterpriseOID;
			NotNullPtr<UI::GUILabel> lblEnterpriseName;
			UI::GUITextBox *txtEnterpriseName;
			NotNullPtr<UI::GUILabel> lblRemoteIP;
			UI::GUITextBox *txtRemoteIP;
			NotNullPtr<UI::GUILabel> lblRemotePort;
			UI::GUITextBox *txtRemotePort;
			NotNullPtr<UI::GUILabel> lblAgentAddr;
			UI::GUITextBox *txtAgentAddr;
			NotNullPtr<UI::GUILabel> lblGenericTrap;
			UI::GUITextBox *txtGenericTrap;
			NotNullPtr<UI::GUILabel> lblSpecificTrap;
			UI::GUITextBox *txtSpecificTrap;
			NotNullPtr<UI::GUILabel> lblRecvTime;
			UI::GUITextBox *txtRecvTime;
			NotNullPtr<UI::GUILabel> lblTrapTime;
			UI::GUITextBox *txtTrapTime;
			UI::GUIListView *lvResults;

			static void __stdcall OnResultSelChg(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static Bool __stdcall OnSNMPTrapPacket(void *userObj, NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt16 port, NotNullPtr<const Net::SNMPUtil::TrapInfo> trap, NotNullPtr<Data::ArrayList<Net::SNMPUtil::BindingItem*>> itemList);

		public:
			AVIRSNMPTrapMonitorForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSNMPTrapMonitorForm();

			virtual void OnMonitorChanged();

			Bool IsError();
		};
	}
}
#endif
