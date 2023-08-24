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

			UI::GUIPanel *pnlRequest;
			UI::GUILabel *lblIP;
			UI::GUITextBox *txtIP;
			UI::GUILabel *lblPort;
			UI::GUITextBox *txtPort;
			UI::GUILabel *lblConcurrCnt;
			UI::GUITextBox *txtConcurrCnt;
			UI::GUILabel *lblTotalConnCnt;
			UI::GUITextBox *txtTotalConnCnt;
			UI::GUIButton *btnStart;
			UI::GUIGroupBox *grpStatus;
			UI::GUILabel *lblConnLeftCnt;
			UI::GUITextBox *txtConnLeftCnt;
			UI::GUILabel *lblThreadCnt;
			UI::GUITextBox *txtThreadCnt;
			UI::GUILabel *lblSuccCnt;
			UI::GUITextBox *txtSuccCnt;
			UI::GUILabel *lblFailCnt;
			UI::GUITextBox *txtFailCnt;

			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall ProcessThread(NotNullPtr<Sync::Thread> thread);
			static void __stdcall OnTimerTick(void *userObj);
			void StopThreads();
		public:
			AVIRTCPTestForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRTCPTestForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
