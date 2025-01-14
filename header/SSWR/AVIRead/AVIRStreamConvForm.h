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
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<IO::SiLabDriver> siLabDriver;

			NN<UI::GUIGroupBox> grpStream1;
			NN<UI::GUIPanel> pnlStream1;
			NN<UI::GUILabel> lblStream1;
			NN<UI::GUITextBox> txtStream1;
			NN<UI::GUICheckBox> chkStreamLog1;
			NN<UI::GUIButton> btnStream1;
			NN<UI::GUIRealtimeLineChart> rlcStream1;
			NN<UI::GUIHSplitter> hspStream;

			NN<UI::GUIGroupBox> grpStream2;
			NN<UI::GUIPanel> pnlStream2;
			NN<UI::GUILabel> lblStream2;
			NN<UI::GUITextBox> txtStream2;
			NN<UI::GUICheckBox> chkStreamLog2;
			NN<UI::GUIButton> btnStream2;
			NN<UI::GUIRealtimeLineChart> rlcStream2;

			Sync::Mutex mut1;
			Optional<IO::Stream> stm1;
			Optional<IO::FileStream> stmLog1;
			Bool thread1Running;
			Bool thread1ToStop;
			Bool remoteClosed1;
			UInt64 stm1DataSize;
			UInt64 stm1LastSize;

			Sync::Mutex mut2;
			Optional<IO::Stream> stm2;
			Optional<IO::FileStream> stmLog2;
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
			AVIRStreamConvForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRStreamConvForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
