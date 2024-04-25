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
			NN<UI::GUILabel> lblX;
			NN<UI::GUITextBox> txtX;
			NN<UI::GUILabel> lblY;
			NN<UI::GUITextBox> txtY;
			NN<UI::GUILabel> lblZ;
			NN<UI::GUITextBox> txtZ;
			NN<UI::GUIPanel> pnlMoving;

			NN<SSWR::AVIRead::AVIRCore> core;
			IO::MotionDetectorAccelerometer motion;
			Bool currMoving;

			static void __stdcall OnTimerTick(AnyType userObj);
		public:
			AVIRAccelerometerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<IO::SensorAccelerometer> acc);
			virtual ~AVIRAccelerometerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
