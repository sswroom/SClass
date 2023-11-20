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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::SiLabDriver *siLabDriver;

			NotNullPtr<UI::GUIGroupBox> grpStream;
			UI::GUILabel *lblStream;
			UI::GUITextBox *txtStream;
			UI::GUIButton *btnStream;

			NotNullPtr<UI::GUIGroupBox> grpStatus;
			UI::GUILabel *lblDataSize;
			UI::GUITextBox *txtDataSize;

			IO::Stream *stm;
			Bool threadRunning;
			Bool threadToStop;
			Bool remoteClosed;
			Bool recvUpdated;
			UInt64 recvCount;

		private:
			static void __stdcall OnStreamClicked(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static UInt32 __stdcall RecvThread(void *userObj);

			void StopStream();

		public:
			AVIRStreamEchoForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRStreamEchoForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
