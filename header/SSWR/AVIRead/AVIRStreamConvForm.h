#ifndef _SM_SSWR_AVIREAD_AVIRSTREAMCONVFORM
#define _SM_SSWR_AVIREAD_AVIRSTREAMCONVFORM
#include "IO/FileStream.h"
#include "IO/SiLabDriver.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUIRealtimeLineChart.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRStreamConvForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;
			IO::SiLabDriver *siLabDriver;

			UI::GUIGroupBox *grpStream1;
			UI::GUIPanel *pnlStream1;
			UI::GUILabel *lblStream1;
			UI::GUITextBox *txtStream1;
			UI::GUICheckBox *chkStreamLog1;
			UI::GUIButton *btnStream1;
			UI::GUIRealtimeLineChart *rlcStream1;
			UI::GUIHSplitter *hspStream;

			UI::GUIGroupBox *grpStream2;
			UI::GUIPanel *pnlStream2;
			UI::GUILabel *lblStream2;
			UI::GUITextBox *txtStream2;
			UI::GUICheckBox *chkStreamLog2;
			UI::GUIButton *btnStream2;
			UI::GUIRealtimeLineChart *rlcStream2;

			Sync::Mutex mut1;
			IO::Stream *stm1;
			IO::FileStream *stmLog1;
			Bool thread1Running;
			Bool thread1ToStop;
			Bool remoteClosed1;
			UInt64 stm1DataSize;
			UInt64 stm1LastSize;

			Sync::Mutex mut2;
			IO::Stream *stm2;
			IO::FileStream *stmLog2;
			Bool thread2Running;
			Bool thread2ToStop;
			Bool remoteClosed2;
			UInt64 stm2DataSize;
			UInt64 stm2LastSize;
		private:
			static void __stdcall OnStream1Clicked(void *userObj);
			static void __stdcall OnStream2Clicked(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static UInt32 __stdcall Stream1Thread(void *userObj);
			static UInt32 __stdcall Stream2Thread(void *userObj);

			void StopStream1();
			void StopStream2();
		public:
			AVIRStreamConvForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRStreamConvForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
