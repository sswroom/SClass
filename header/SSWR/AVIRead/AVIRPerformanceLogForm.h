#ifndef _SM_SSWR_AVIREAD_AVIRPERFORMANCELOGFORM
#define _SM_SSWR_AVIREAD_AVIRPERFORMANCELOGFORM
#include "IO/FileStream.h"
#include "IO/Writer.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUIProgressBar.h"
#include "UI/GUIRealtimeLineChart.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRPerformanceLogForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			IO::Writer *writer;
			IO::FileStream *logStream;
			UInt8 *testBuff;
			Data::Timestamp testTime;

			NN<UI::GUIPanel> pnlCtrl;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUILabel> lblCurrWRate;
			NN<UI::GUITextBox> txtCurrWRate;
			NN<UI::GUIRealtimeLineChart> rlcWRate;

			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			Bool Start();
			void Stop();
			void TestSpeed();
		public:
			AVIRPerformanceLogForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRPerformanceLogForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
