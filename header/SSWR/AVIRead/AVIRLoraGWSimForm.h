#ifndef _SM_SSWR_AVIREAD_AVIRLORAGWSIMFORM
#define _SM_SSWR_AVIREAD_AVIRLORAGWSIMFORM
#include "Net/UDPServer.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/ListBoxLogger.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIPanel.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRLoraGWSimForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;
			Net::SocketFactory *sockf;

			Net::UDPServer *udp;
			IO::LogTool log;
			Net::SocketUtil::AddressInfo svrAddr;
			UInt16 svrPort;
			UInt8 gatewayEUI[8];
			UInt16 tokenNext;
			Sync::Mutex tokenMut;

			UI::GUIPanel *pnlControl;
			UI::GUILabel *lblServerIP;
			UI::GUITextBox *txtServerIP;
			UI::GUILabel *lblServerPort;
			UI::GUITextBox *txtServerPort;
			UI::GUILabel *lblGatewayEUI;
			UI::GUITextBox *txtGatewayEUI;
			UI::GUIButton *btnStart;
			UI::GUIPanel *pnlDevice;
			UI::GUIButton *btnTest;

		private:
			static void __stdcall OnStartClick(void *userObj);
			static void __stdcall OnTestClick(void *userObj);
			static void __stdcall OnUDPPacket(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData);
			static void __stdcall OnTimerTick(void *userObj);

			static UOSInt GenUpPayload(UInt8 *buff, Bool needConfirm, UInt32 devAddr, UInt32 fCnt, UInt8 fPort, const UInt8 *nwkSKey, const UInt8 *appSKey, const UInt8 *payload, UOSInt payloadLen);
			static void GenRxpkJSON(Text::StringBuilderUTF8 *sb, UInt32 freq, UInt32 chan, UInt32 rfch, UInt32 codrk, Int32 rssi, Int32 lsnr, const UInt8 *data, UOSInt dataSize);
			UInt16 NextToken();
			Bool SendPushData(const UInt8 *data, UOSInt dataLeng);
			Bool SendPullData();
		public:
			AVIRLoraGWSimForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRLoraGWSimForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
