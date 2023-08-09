#ifndef _SM_SSWR_AVIREAD_AVIRIOPINTESTFORM
#define _SM_SSWR_AVIREAD_AVIRIOPINTESTFORM
#include "IO/IOPin.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRIOPinTestForm : public UI::GUIForm
		{
		private:
			UI::GUILabel *lblPullType;
			UI::GUIButton *btnPullUp;
			UI::GUIButton *btnPullDown;
			UI::GUILabel *lblPinMode;
			UI::GUIButton *btnPinModeInput;
			UI::GUIButton *btnPinModeOutput;
			UI::GUILabel *lblPinState;
			UI::GUITextBox *txtPinState;
			UI::GUIButton *btnPinHigh;
			UI::GUIButton *btnPinLow;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::IOPin *pin;
			Bool isOutput;

			static void __stdcall OnPullUpClicked(void *userObj);
			static void __stdcall OnPullDownClicked(void *userObj);
			static void __stdcall OnPinModeInputClicked(void *userObj);
			static void __stdcall OnPinModeOutputClicked(void *userObj);
			static void __stdcall OnPinHighClicked(void *userObj);
			static void __stdcall OnPinLowClicked(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
		public:
			AVIRIOPinTestForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, IO::IOPin *pin);
			virtual ~AVIRIOPinTestForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
