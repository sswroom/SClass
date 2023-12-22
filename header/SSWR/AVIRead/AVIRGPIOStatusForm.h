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
				UOSInt pinMode;
			} PinState;
			
		private:
			NotNullPtr<UI::GUIListView> lvStatus;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::GPIOControl *ctrl;
			PinState *states;

			static void __stdcall OnTimerTick(void *userObj);
		public:
			AVIRGPIOStatusForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRGPIOStatusForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
