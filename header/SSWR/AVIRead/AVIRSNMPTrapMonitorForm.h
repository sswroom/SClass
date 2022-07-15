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
			SSWR::AVIRead::AVIRCore *core;
			Net::SNMPTrapMonitor *mon;
			Sync::Mutex packetMut;
			Data::ArrayList<SNMPPacket*> packetList;

			UI::GUIListBox *lbResults;
			UI::GUIHSplitter *hspResults;
			UI::GUIPanel *pnlResults;
			UI::GUIPanel *pnlTrap;
			UI::GUILabel *lblCommunity;
			UI::GUITextBox *txtCommunity;
			UI::GUILabel *lblEnterpriseOID;
			UI::GUITextBox *txtEnterpriseOID;
			UI::GUILabel *lblEnterpriseName;
			UI::GUITextBox *txtEnterpriseName;
			UI::GUILabel *lblRemoteIP;
			UI::GUITextBox *txtRemoteIP;
			UI::GUILabel *lblRemotePort;
			UI::GUITextBox *txtRemotePort;
			UI::GUILabel *lblAgentAddr;
			UI::GUITextBox *txtAgentAddr;
			UI::GUILabel *lblGenericTrap;
			UI::GUITextBox *txtGenericTrap;
			UI::GUILabel *lblSpecificTrap;
			UI::GUITextBox *txtSpecificTrap;
			UI::GUILabel *lblRecvTime;
			UI::GUITextBox *txtRecvTime;
			UI::GUILabel *lblTrapTime;
			UI::GUITextBox *txtTrapTime;
			UI::GUIListView *lvResults;

			static void __stdcall OnResultSelChg(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static Bool __stdcall OnSNMPTrapPacket(void *userObj, const Net::SocketUtil::AddressInfo *addr, UInt16 port, const Net::SNMPUtil::TrapInfo *trap, Data::ArrayList<Net::SNMPUtil::BindingItem*> *itemList);

		public:
			AVIRSNMPTrapMonitorForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRSNMPTrapMonitorForm();

			virtual void OnMonitorChanged();

			Bool IsError();
		};
	}
}
#endif
