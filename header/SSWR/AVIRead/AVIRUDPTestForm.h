#ifndef _SM_SSWR_AVIREAD_AVIRUDPTESTFORM
#define _SM_SSWR_AVIREAD_AVIRUDPTESTFORM

#include "Manage/HiResClock.h"
#include "Net/UDPServer.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRUDPTestForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				AVIRUDPTestForm *me;
				UOSInt reqCnt;
				Net::SocketUtil::AddressInfo destAddr;
				UInt16 destPort;
				Int32 status; //0 = not running, 1 = idle, 2 = procesisng
				Int32 taskType; //0 = idle, 1 = sending, 2 = toExit;
				UInt64 sentSuccCnt;
				UInt64 sentFailCnt;
				Sync::Event *evt;
			} ThreadStatus;
		private:
			SSWR::AVIRead::AVIRCore *core;
			Net::SocketFactory *sockf;
			Net::UDPServer *udp;
			Bool autoReply;
			Sync::Mutex *mut;
			UInt64 recvCnt;
			UInt64 recvSize;
			UInt64 lastRecvCnt;
			UInt64 lastRecvSize;
			UInt64 lastSentSuccCnt;
			UInt64 lastSentFailCnt;
			Data::DateTime *lastTime;

			UOSInt threadCnt;
			ThreadStatus *threads;
			Sync::Event *mainEvt;

			UI::GUIGroupBox *grpServer;
			UI::GUILabel *lblServerPort;
			UI::GUITextBox *txtServerPort;
			UI::GUIButton *btnStart;
			UI::GUICheckBox *chkAutoReply;
			UI::GUIGroupBox *grpDest;
			UI::GUILabel *lblDestHost;
			UI::GUITextBox *txtDestHost;
			UI::GUILabel *lblDestPort;
			UI::GUITextBox *txtDestPort;
			UI::GUILabel *lblDestCount;
			UI::GUITextBox *txtDestCount;
			UI::GUIButton *btnSend;
			UI::GUIGroupBox *grpStatus;
			UI::GUILabel *lblRecvCnt;
			UI::GUITextBox *txtRecvCnt;
			UI::GUITextBox *txtRecvCntRate;
			UI::GUILabel *lblRecvSize;
			UI::GUITextBox *txtRecvSize;
			UI::GUITextBox *txtRecvSizeRate;
			UI::GUILabel *lblSentSuccCnt;
			UI::GUITextBox *txtSentSuccCnt;
			UI::GUITextBox *txtSentSuccCntRate;
			UI::GUILabel *lblSentFailCnt;
			UI::GUITextBox *txtSentFailCnt;
			UI::GUITextBox *txtSentFailCntRate;

			static void __stdcall OnUDPPacket(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData);
			static void __stdcall OnAutoReplyChanged(void *userObj, Bool newVal);
			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnSendClicked(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);

			static UInt32 __stdcall ProcThread(void *userObj);
		public:
			AVIRUDPTestForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRUDPTestForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
