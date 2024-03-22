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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::SiLabDriver *siLabDriver;

			NotNullPtr<UI::GUIGroupBox> grpStream1;
			NotNullPtr<UI::GUIPanel> pnlStream1;
			NotNullPtr<UI::GUILabel> lblStream1;
			NotNullPtr<UI::GUITextBox> txtStream1;
			NotNullPtr<UI::GUICheckBox> chkStreamLog1;
			NotNullPtr<UI::GUIButton> btnStream1;
			NotNullPtr<UI::GUIRealtimeLineChart> rlcStream1;
			NotNullPtr<UI::GUIHSplitter> hspStream;

			NotNullPtr<UI::GUIGroupBox> grpStream2;
			NotNullPtr<UI::GUIPanel> pnlStream2;
			NotNullPtr<UI::GUILabel> lblStream2;
			NotNullPtr<UI::GUITextBox> txtStream2;
			NotNullPtr<UI::GUICheckBox> chkStreamLog2;
			NotNullPtr<UI::GUIButton> btnStream2;
			NotNullPtr<UI::GUIRealtimeLineChart> rlcStream2;

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
			static void __stdcall OnStream1Clicked(AnyType userObj);
			static void __stdcall OnStream2Clicked(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static UInt32 __stdcall Stream1Thread(AnyType userObj);
			static UInt32 __stdcall Stream2Thread(AnyType userObj);

			void StopStream1();
			void StopStream2();
		public:
			AVIRStreamConvForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRStreamConvForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
