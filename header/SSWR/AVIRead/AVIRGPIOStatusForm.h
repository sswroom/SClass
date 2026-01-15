#ifndef _SM_SSWR_AVIREAD_AVIRGPIOSTATUSFORM
#define _SM_SSWR_AVIREAD_AVIRGPIOSTATUSFORM
#include "IO/GPIOControl.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUIListView.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGPIOStatusForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				Bool isHigh;
				UIntOS pinMode;
			} PinState;
			
		private:
			NN<UI::GUIListView> lvStatus;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<IO::GPIOControl> ctrl;
			UnsafeArrayOpt<PinState> states;

			static void __stdcall OnTimerTick(AnyType userObj);
		public:
			AVIRGPIOStatusForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRGPIOStatusForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
