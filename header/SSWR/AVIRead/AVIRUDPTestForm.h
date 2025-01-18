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
				NN<AVIRUDPTestForm> me;
				UOSInt reqCnt;
				Net::SocketUtil::AddressInfo destAddr;
				UInt16 destPort;
				Int32 status; //0 = not running, 1 = idle, 2 = procesisng
				Int32 taskType; //0 = idle, 1 = sending, 2 = toExit;
				UInt64 sentSuccCnt;
				UInt64 sentFailCnt;
				NN<Sync::Event> evt;
			} ThreadStatus;
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Net::SocketFactory> sockf;
			Optional<Net::UDPServer> udp;
			Bool autoReply;
			Sync::Mutex mut;
			UInt64 recvCnt;
			UInt64 recvSize;
			UInt64 lastRecvCnt;
			UInt64 lastRecvSize;
			UInt64 lastSentSuccCnt;
			UInt64 lastSentFailCnt;
			Data::DateTime lastTime;

			UOSInt threadCnt;
			UnsafeArray<ThreadStatus> threads;
			Sync::Event mainEvt;

			NN<UI::GUIGroupBox> grpServer;
			NN<UI::GUILabel> lblServerPort;
			NN<UI::GUITextBox> txtServerPort;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUICheckBox> chkAutoReply;
			NN<UI::GUIGroupBox> grpDest;
			NN<UI::GUILabel> lblDestHost;
			NN<UI::GUITextBox> txtDestHost;
			NN<UI::GUILabel> lblDestPort;
			NN<UI::GUITextBox> txtDestPort;
			NN<UI::GUILabel> lblDestCount;
			NN<UI::GUITextBox> txtDestCount;
			NN<UI::GUIButton> btnSend;
			NN<UI::GUIGroupBox> grpStatus;
			NN<UI::GUILabel> lblRecvCnt;
			NN<UI::GUITextBox> txtRecvCnt;
			NN<UI::GUITextBox> txtRecvCntRate;
			NN<UI::GUILabel> lblRecvSize;
			NN<UI::GUITextBox> txtRecvSize;
			NN<UI::GUITextBox> txtRecvSizeRate;
			NN<UI::GUILabel> lblSentSuccCnt;
			NN<UI::GUITextBox> txtSentSuccCnt;
			NN<UI::GUITextBox> txtSentSuccCntRate;
			NN<UI::GUILabel> lblSentFailCnt;
			NN<UI::GUITextBox> txtSentFailCnt;
			NN<UI::GUITextBox> txtSentFailCntRate;

			static void __stdcall OnUDPPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData);
			static void __stdcall OnAutoReplyChanged(AnyType userObj, Bool newVal);
			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall OnSendClicked(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);

			static UInt32 __stdcall ProcThread(AnyType userObj);
		public:
			AVIRUDPTestForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRUDPTestForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
