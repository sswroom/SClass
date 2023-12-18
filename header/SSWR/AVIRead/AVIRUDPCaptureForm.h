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
			NotNullPtr<UI::GUIPanel> pnlControl;
			NotNullPtr<UI::GUILabel> lblPort;
			UI::GUITextBox *txtPort;
			UI::GUICheckBox *chkReuseAddr;
			NotNullPtr<UI::GUIButton> btnStart;
			UI::GUITabControl *tcMain;

			NotNullPtr<UI::GUITabPage> tpData;
			UI::GUIListBox *lbData;
			NotNullPtr<UI::GUIHSplitter> hspData;
			UI::GUITextBox *txtData;

			NotNullPtr<UI::GUITabPage> tpLog;
			UI::GUITextBox *txtLog;
			NotNullPtr<UI::GUIListBox> lbLog;

			NotNullPtr<UI::GUITabPage> tpPorts;
			UI::GUIListView *lvPorts;

			NotNullPtr<UI::GUITabPage> tpMulticast;
			NotNullPtr<UI::GUIPanel> pnlMulticastCommon;
			NotNullPtr<UI::GUILabel> lblMulticastCommon;
			UI::GUIListBox *lbMulticastCommon;
			NotNullPtr<UI::GUIHSplitter> hspMulticast;
			NotNullPtr<UI::GUIPanel> pnlMulticast;
			NotNullPtr<UI::GUIPanel> pnlMulticastCtrl;
			NotNullPtr<UI::GUILabel> lblMulticastCurr;
			UI::GUITextBox *txtMulticastCurr;
			NotNullPtr<UI::GUIButton> btnMulticastAdd;
			UI::GUIListBox *lbMulticastCurr;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::LogTool log;
			NotNullPtr<UI::ListBoxLogger> logger;
			Net::UDPServer *svr;

			Bool packetsChg;
			OSInt packetCurr;
			PacketInfo *packets;
			Sync::Mutex packetMut;

			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnLogSelChg(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnDataSelChg(void *userObj);
			static void __stdcall OnPortsDblClk(void *userObj, UOSInt index);
			static void __stdcall OnUDPPacket(NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData);
			static void __stdcall OnMulticastClicked(void *userObj);
			static void __stdcall OnMulticastDoubleClk(void *userObj);
		public:
			AVIRUDPCaptureForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRUDPCaptureForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
