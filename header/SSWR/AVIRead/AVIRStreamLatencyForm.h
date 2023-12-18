#ifndef _SM_SSWR_AVIREAD_AVIRSTREAMLATENCYFORM
#define _SM_SSWR_AVIREAD_AVIRSTREAMLATENCYFORM
#include "IO/MemoryStream.h"
#include "IO/SiLabDriver.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIRealtimeLineChart.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"
#include "UI/ListBoxLogger.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRStreamLatencyForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::SiLabDriver *siLabDriver;

			NotNullPtr<UI::GUIGroupBox> grpStream;
			UI::GUILabel *lblStream;
			UI::GUITextBox *txtStream;
			NotNullPtr<UI::GUIButton> btnStream;

			NotNullPtr<UI::GUIGroupBox> grpControl;
			UI::GUILabel *lblReqInterval;
			UI::GUIComboBox *cboReqInterval;

			UI::GUITabControl *tcMain;

			NotNullPtr<UI::GUITabPage> tpLatency;
			UI::GUIRealtimeLineChart *rlcLatency;

			NotNullPtr<UI::GUITabPage> tpLog;
			UI::GUITextBox *txtLog;
			NotNullPtr<UI::GUIListBox> lbLog;

			NotNullPtr<UI::GUITabPage> tpStatus;
			UI::GUILabel *lblSentCnt;
			UI::GUITextBox *txtSentCnt;
			UI::GUILabel *lblRecvCnt;
			UI::GUITextBox *txtRecvCnt;

			IO::Stream *stm;
			IO::LogTool *log;
			NotNullPtr<UI::ListBoxLogger> logger;
			Bool threadRunning;
			Bool threadToStop;
			Bool remoteClosed;
			OSInt sentCnt;
			OSInt recvCnt;
			OSInt dispSent;
			OSInt dispRecv;
			Int64 lastSentTime;

		private:
			static void __stdcall OnStreamClicked(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static UInt32 __stdcall RecvThread(void *userObj);

			void StopStream();

		public:
			AVIRStreamLatencyForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRStreamLatencyForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
