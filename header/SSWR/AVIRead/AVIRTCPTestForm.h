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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<Net::SocketFactory> sockf;
			Sync::Thread **threads;
			UInt32 svrIP;
			UInt16 svrPort;
			Sync::Mutex connMut;
			UInt32 connLeftCnt;
			UInt32 threadCnt;
			UInt32 threadCurrCnt;
			UInt32 connCnt;
			UInt32 failCnt;

			NotNullPtr<UI::GUIPanel> pnlRequest;
			NotNullPtr<UI::GUILabel> lblIP;
			NotNullPtr<UI::GUITextBox> txtIP;
			NotNullPtr<UI::GUILabel> lblPort;
			NotNullPtr<UI::GUITextBox> txtPort;
			NotNullPtr<UI::GUILabel> lblConcurrCnt;
			NotNullPtr<UI::GUITextBox> txtConcurrCnt;
			NotNullPtr<UI::GUILabel> lblTotalConnCnt;
			NotNullPtr<UI::GUITextBox> txtTotalConnCnt;
			NotNullPtr<UI::GUIButton> btnStart;
			NotNullPtr<UI::GUIGroupBox> grpStatus;
			NotNullPtr<UI::GUILabel> lblConnLeftCnt;
			NotNullPtr<UI::GUITextBox> txtConnLeftCnt;
			NotNullPtr<UI::GUILabel> lblThreadCnt;
			NotNullPtr<UI::GUITextBox> txtThreadCnt;
			NotNullPtr<UI::GUILabel> lblSuccCnt;
			NotNullPtr<UI::GUITextBox> txtSuccCnt;
			NotNullPtr<UI::GUILabel> lblFailCnt;
			NotNullPtr<UI::GUITextBox> txtFailCnt;

			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall ProcessThread(NotNullPtr<Sync::Thread> thread);
			static void __stdcall OnTimerTick(void *userObj);
			void StopThreads();
		public:
			AVIRTCPTestForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRTCPTestForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
