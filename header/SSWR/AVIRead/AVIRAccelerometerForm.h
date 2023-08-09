#ifndef _SM_SSWR_AVIREAD_AVIRACCELEROMETERFORM
#define _SM_SSWR_AVIREAD_AVIRACCELEROMETERFORM
#include "IO/MotionDetectorAccelerometer.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRAccelerometerForm : public UI::GUIForm
		{
		private:
			UI::GUILabel *lblX;
			UI::GUITextBox *txtX;
			UI::GUILabel *lblY;
			UI::GUITextBox *txtY;
			UI::GUILabel *lblZ;
			UI::GUITextBox *txtZ;
			UI::GUIPanel *pnlMoving;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::MotionDetectorAccelerometer *motion;
			Bool currMoving;

			static void __stdcall OnTimerTick(void *userObj);
		public:
			AVIRAccelerometerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, IO::SensorAccelerometer *acc);
			virtual ~AVIRAccelerometerForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
