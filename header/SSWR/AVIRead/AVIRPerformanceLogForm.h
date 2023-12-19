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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::Writer *writer;
			IO::FileStream *logStream;
			UInt8 *testBuff;
			Data::Timestamp testTime;

			NotNullPtr<UI::GUIPanel> pnlCtrl;
			NotNullPtr<UI::GUIButton> btnStart;
			NotNullPtr<UI::GUILabel> lblCurrWRate;
			NotNullPtr<UI::GUITextBox> txtCurrWRate;
			UI::GUIRealtimeLineChart *rlcWRate;

			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			Bool Start();
			void Stop();
			void TestSpeed();
		public:
			AVIRPerformanceLogForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRPerformanceLogForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
