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
			NN<UI::GUILabel> lblPullType;
			NN<UI::GUIButton> btnPullUp;
			NN<UI::GUIButton> btnPullDown;
			NN<UI::GUILabel> lblPinMode;
			NN<UI::GUIButton> btnPinModeInput;
			NN<UI::GUIButton> btnPinModeOutput;
			NN<UI::GUILabel> lblPinState;
			NN<UI::GUITextBox> txtPinState;
			NN<UI::GUIButton> btnPinHigh;
			NN<UI::GUIButton> btnPinLow;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<IO::IOPin> pin;
			Bool isOutput;

			static void __stdcall OnPullUpClicked(AnyType userObj);
			static void __stdcall OnPullDownClicked(AnyType userObj);
			static void __stdcall OnPinModeInputClicked(AnyType userObj);
			static void __stdcall OnPinModeOutputClicked(AnyType userObj);
			static void __stdcall OnPinHighClicked(AnyType userObj);
			static void __stdcall OnPinLowClicked(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
		public:
			AVIRIOPinTestForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<IO::IOPin> pin);
			virtual ~AVIRIOPinTestForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
