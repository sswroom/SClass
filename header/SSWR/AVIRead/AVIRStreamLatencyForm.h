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
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<IO::SiLabDriver> siLabDriver;

			NN<UI::GUIGroupBox> grpStream;
			NN<UI::GUILabel> lblStream;
			NN<UI::GUITextBox> txtStream;
			NN<UI::GUIButton> btnStream;

			NN<UI::GUIGroupBox> grpControl;
			NN<UI::GUILabel> lblReqInterval;
			NN<UI::GUIComboBox> cboReqInterval;

			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpLatency;
			NN<UI::GUIRealtimeLineChart> rlcLatency;

			NN<UI::GUITabPage> tpLog;
			NN<UI::GUITextBox> txtLog;
			NN<UI::GUIListBox> lbLog;

			NN<UI::GUITabPage> tpStatus;
			NN<UI::GUILabel> lblSentCnt;
			NN<UI::GUITextBox> txtSentCnt;
			NN<UI::GUILabel> lblRecvCnt;
			NN<UI::GUITextBox> txtRecvCnt;

			Optional<IO::Stream> stm;
			NN<IO::LogTool> log;
			NN<UI::ListBoxLogger> logger;
			Bool threadRunning;
			Bool threadToStop;
			Bool remoteClosed;
			IntOS sentCnt;
			IntOS recvCnt;
			IntOS dispSent;
			IntOS dispRecv;
			Int64 lastSentTime;

		private:
			static void __stdcall OnStreamClicked(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static UInt32 __stdcall RecvThread(AnyType userObj);

			void StopStream();

		public:
			AVIRStreamLatencyForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRStreamLatencyForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
