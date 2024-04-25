#ifndef _SM_SSWR_AVIREAD_AVIRTCPTESTFORM
#define _SM_SSWR_AVIREAD_AVIRTCPTESTFORM

#include "Net/HTTPClient.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "Sync/Mutex.h"
#include "Sync/Thread.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRTCPTestForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Net::SocketFactory> sockf;
			Sync::Thread **threads;
			UInt32 svrIP;
			UInt16 svrPort;
			Sync::Mutex connMut;
			UInt32 connLeftCnt;
			UInt32 threadCnt;
			UInt32 threadCurrCnt;
			UInt32 connCnt;
			UInt32 failCnt;

			NN<UI::GUIPanel> pnlRequest;
			NN<UI::GUILabel> lblIP;
			NN<UI::GUITextBox> txtIP;
			NN<UI::GUILabel> lblPort;
			NN<UI::GUITextBox> txtPort;
			NN<UI::GUILabel> lblConcurrCnt;
			NN<UI::GUITextBox> txtConcurrCnt;
			NN<UI::GUILabel> lblTotalConnCnt;
			NN<UI::GUITextBox> txtTotalConnCnt;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUIGroupBox> grpStatus;
			NN<UI::GUILabel> lblConnLeftCnt;
			NN<UI::GUITextBox> txtConnLeftCnt;
			NN<UI::GUILabel> lblThreadCnt;
			NN<UI::GUITextBox> txtThreadCnt;
			NN<UI::GUILabel> lblSuccCnt;
			NN<UI::GUITextBox> txtSuccCnt;
			NN<UI::GUILabel> lblFailCnt;
			NN<UI::GUITextBox> txtFailCnt;

			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall ProcessThread(NN<Sync::Thread> thread);
			static void __stdcall OnTimerTick(AnyType userObj);
			void StopThreads();
		public:
			AVIRTCPTestForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRTCPTestForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
