#ifndef _SM_SSWR_AVIREAD_AVIRBYDC9RFORM
#define _SM_SSWR_AVIREAD_AVIRBYDC9RFORM
#include "IO/CANListener.h"
#include "IO/Device/BYDC9R.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRBYDC9RForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			IO::Device::BYDC9R c9r;
			IO::CANListener *listener;
			Optional<Net::SSLEngine> ssl;

			NN<UI::GUIGroupBox> grpCANBus;
			NN<UI::GUILabel> lblCANBus;
			NN<UI::GUITextBox> txtCANBus;
			NN<UI::GUIButton> btnCANBus;

			NN<UI::GUILabel> lblSpeedKmHr;
			NN<UI::GUITextBox> txtSpeedKmHr;
			NN<UI::GUILabel> lblDoor1;
			NN<UI::GUITextBox> txtDoor1;
			NN<UI::GUILabel> lblDoor2;
			NN<UI::GUITextBox> txtDoor2;
			NN<UI::GUILabel> lblBatteryLevel;
			NN<UI::GUITextBox> txtBatteryLevel;
			NN<UI::GUILabel> lblMotorRPM;
			NN<UI::GUITextBox> txtMotorRPM;
			NN<UI::GUILabel> lblBatteryStartCharging;
			NN<UI::GUITextBox> txtBatteryStartCharging;
			NN<UI::GUILabel> lblLeftMotorMode;
			NN<UI::GUITextBox> txtLeftMotorMode;
			NN<UI::GUILabel> lblRightotorMode;
			NN<UI::GUITextBox> txtRightMotorMode;
			NN<UI::GUILabel> lblCarbinDoorBack;
			NN<UI::GUITextBox> txtCarbinDoorBack;
			NN<UI::GUILabel> lblCarbinDoorLeft;
			NN<UI::GUITextBox> txtCarbinDoorLeft;
			NN<UI::GUILabel> lblCarbinDoorRight;
			NN<UI::GUITextBox> txtCarbinDoorRight;
			NN<UI::GUILabel> lblPowerMode;
			NN<UI::GUITextBox> txtPowerMode;
			NN<UI::GUILabel> lblBatteryCharging;
			NN<UI::GUITextBox> txtBatteryCharging;
			NN<UI::GUILabel> lblOkLED;
			NN<UI::GUITextBox> txtOkLED;

			IO::Device::BYDC9R::DeviceStatus dispStatus;

			static void __stdcall OnCANBusClicked(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
		public:
			AVIRBYDC9RForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRBYDC9RForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
