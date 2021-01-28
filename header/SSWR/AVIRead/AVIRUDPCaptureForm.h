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
				Int32 buffSize;
				UInt8 *buff;
				Net::SocketUtil::AddressInfo addr;
				UInt16 port;
			} PacketInfo;
		private:
			UI::GUIPanel *pnlControl;
			UI::GUILabel *lblPort;
			UI::GUITextBox *txtPort;
			UI::GUICheckBox *chkReuseAddr;
			UI::GUIButton *btnStart;
			UI::GUITabControl *tcMain;

			UI::GUITabPage *tpData;
			UI::GUIListBox *lbData;
			UI::GUIHSplitter *hspData;
			UI::GUITextBox *txtData;

			UI::GUITabPage *tpLog;
			UI::GUITextBox *txtLog;
			UI::GUIListBox *lbLog;

			UI::GUITabPage *tpPorts;
			UI::GUIListView *lvPorts;

			UI::GUITabPage *tpMulticast;
			UI::GUIPanel *pnlMulticastCommon;
			UI::GUILabel *lblMulticastCommon;
			UI::GUIListBox *lbMulticastCommon;
			UI::GUIHSplitter *hspMulticast;
			UI::GUIPanel *pnlMulticast;
			UI::GUIPanel *pnlMulticastCtrl;
			UI::GUILabel *lblMulticastCurr;
			UI::GUITextBox *txtMulticastCurr;
			UI::GUIButton *btnMulticastAdd;
			UI::GUIListBox *lbMulticastCurr;

			SSWR::AVIRead::AVIRCore *core;
			IO::LogTool *log;
			UI::ListBoxLogger *logger;
			Net::UDPServer *svr;

			Bool packetsChg;
			OSInt packetCurr;
			PacketInfo *packets;
			Sync::Mutex *packetMut;

			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnLogSelChg(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnDataSelChg(void *userObj);
			static void __stdcall OnPortsDblClk(void *userObj, OSInt index);
			static void __stdcall OnUDPPacket(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData);
			static void __stdcall OnMulticastClicked(void *userObj);
			static void __stdcall OnMulticastDoubleClk(void *userObj);
		public:
			AVIRUDPCaptureForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRUDPCaptureForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
