#ifndef _SM_SSWR_AVIREAD_AVIRTCPSPDCLIFORM
#define _SM_SSWR_AVIREAD_AVIRTCPSPDCLIFORM
#include "Manage/HiResClock.h"
#include "Net/TCPClient.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRTCPSpdCliForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			UInt64 recvSize;
			UInt64 sendSize;
			Double lastTime;
			UInt64 lastRecvSize;
			UInt64 lastSendSize;
			Manage::HiResClock clk;
			Optional<Net::TCPClient> cli;
			Sync::Mutex cliMut;
			Bool connected;

			Bool procRunning;
			Bool recvRunning;
			Bool toStop;
			Sync::Event mainEvt;
			Sync::Event recvEvt;
			Sync::Event procEvt;

			NN<UI::GUILabel> lblHost;
			NN<UI::GUITextBox> txtHost;
			NN<UI::GUILabel> lblPort;
			NN<UI::GUITextBox> txtPort;
			NN<UI::GUIButton> btnConn;

			NN<UI::GUILabel> lblSendSpeed;
			NN<UI::GUITextBox> txtSendSpeed;
			NN<UI::GUILabel> lblRecvSpeed;
			NN<UI::GUITextBox> txtRecvSpeed;

			static void __stdcall OnConnClick(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static UInt32 __stdcall ProcThread(AnyType userObj);
			static UInt32 __stdcall RecvThread(AnyType userObj);
		public:
			AVIRTCPSpdCliForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRTCPSpdCliForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
