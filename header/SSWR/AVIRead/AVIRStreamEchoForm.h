#ifndef _SM_SSWR_AVIREAD_AVIRSTREAMECHOFORM
#define _SM_SSWR_AVIREAD_AVIRSTREAMECHOFORM
#include "IO/MemoryStream.h"
#include "IO/SiLabDriver.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRStreamEchoForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			IO::SiLabDriver *siLabDriver;

			NN<UI::GUIGroupBox> grpStream;
			NN<UI::GUILabel> lblStream;
			NN<UI::GUITextBox> txtStream;
			NN<UI::GUIButton> btnStream;

			NN<UI::GUIGroupBox> grpStatus;
			NN<UI::GUILabel> lblDataSize;
			NN<UI::GUITextBox> txtDataSize;

			IO::Stream *stm;
			Bool threadRunning;
			Bool threadToStop;
			Bool remoteClosed;
			Bool recvUpdated;
			UInt64 recvCount;

		private:
			static void __stdcall OnStreamClicked(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static UInt32 __stdcall RecvThread(AnyType userObj);

			void StopStream();

		public:
			AVIRStreamEchoForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRStreamEchoForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
