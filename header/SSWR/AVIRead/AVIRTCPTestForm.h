#ifndef _SM_SSWR_AVIREAD_AVIRTCPTESTFORM
#define _SM_SSWR_AVIREAD_AVIRTCPTESTFORM

#include "Net/HTTPClient.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "Sync/Mutex.h"
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
			typedef struct
			{
				AVIRTCPTestForm *me;
				Bool threadRunning;
				Bool threadToStop;
				Sync::Event *evt;
			} ThreadStatus;
		private:
			SSWR::AVIRead::AVIRCore *core;
			Net::SocketFactory *sockf;
			ThreadStatus *threadStatus;
			Int32 svrIP;
			Int32 svrPort;
			Sync::Mutex *connMut;
			Int32 connLeftCnt;
			Int32 threadCnt;
			Int32 threadCurrCnt;
			Int32 connCnt;
			Int32 failCnt;

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
			static UInt32 __stdcall ProcessThread(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			void StopThreads();
		public:
			AVIRTCPTestForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRTCPTestForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
