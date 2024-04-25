#ifndef _SM_SSWR_AVIREAD_AVIRUDPCAPTUREFORM
#define _SM_SSWR_AVIREAD_AVIRUDPCAPTUREFORM
#include "Net/UDPServer.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "Sync/Mutex.h"
#include "UI/ListBoxLogger.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRUDPCaptureForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				Int64 recvTime;
				UInt32 buffSize;
				UInt8 *buff;
				Net::SocketUtil::AddressInfo addr;
				UInt16 port;
			} PacketInfo;
		private:
			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUILabel> lblPort;
			NN<UI::GUITextBox> txtPort;
			NN<UI::GUICheckBox> chkReuseAddr;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpData;
			NN<UI::GUIListBox> lbData;
			NN<UI::GUIHSplitter> hspData;
			NN<UI::GUITextBox> txtData;

			NN<UI::GUITabPage> tpLog;
			NN<UI::GUITextBox> txtLog;
			NN<UI::GUIListBox> lbLog;

			NN<UI::GUITabPage> tpPorts;
			NN<UI::GUIListView> lvPorts;

			NN<UI::GUITabPage> tpMulticast;
			NN<UI::GUIPanel> pnlMulticastCommon;
			NN<UI::GUILabel> lblMulticastCommon;
			NN<UI::GUIListBox> lbMulticastCommon;
			NN<UI::GUIHSplitter> hspMulticast;
			NN<UI::GUIPanel> pnlMulticast;
			NN<UI::GUIPanel> pnlMulticastCtrl;
			NN<UI::GUILabel> lblMulticastCurr;
			NN<UI::GUITextBox> txtMulticastCurr;
			NN<UI::GUIButton> btnMulticastAdd;
			NN<UI::GUIListBox> lbMulticastCurr;

			NN<SSWR::AVIRead::AVIRCore> core;
			IO::LogTool log;
			NN<UI::ListBoxLogger> logger;
			Net::UDPServer *svr;

			Bool packetsChg;
			OSInt packetCurr;
			PacketInfo *packets;
			Sync::Mutex packetMut;

			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall OnLogSelChg(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnDataSelChg(AnyType userObj);
			static void __stdcall OnPortsDblClk(AnyType userObj, UOSInt index);
			static void __stdcall OnUDPPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, AnyType userData);
			static void __stdcall OnMulticastClicked(AnyType userObj);
			static void __stdcall OnMulticastDoubleClk(AnyType userObj);
		public:
			AVIRUDPCaptureForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRUDPCaptureForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
