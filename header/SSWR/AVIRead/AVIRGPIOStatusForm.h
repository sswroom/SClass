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
				UInt16 pinMode;
			} PinState;
			
		private:
			UI::GUIListView *lvStatus;

			SSWR::AVIRead::AVIRCore *core;
			IO::GPIOControl *ctrl;
			PinState *states;

			static void __stdcall OnTimerTick(void *userObj);
		public:
			AVIRGPIOStatusForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRGPIOStatusForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
