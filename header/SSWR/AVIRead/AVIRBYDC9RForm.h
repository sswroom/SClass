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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::Device::BYDC9R c9r;
			IO::CANListener *listener;
			Optional<Net::SSLEngine> ssl;

			NotNullPtr<UI::GUIGroupBox> grpCANBus;
			NotNullPtr<UI::GUILabel> lblCANBus;
			NotNullPtr<UI::GUITextBox> txtCANBus;
			NotNullPtr<UI::GUIButton> btnCANBus;

			NotNullPtr<UI::GUILabel> lblSpeedKmHr;
			NotNullPtr<UI::GUITextBox> txtSpeedKmHr;
			NotNullPtr<UI::GUILabel> lblDoor1;
			NotNullPtr<UI::GUITextBox> txtDoor1;
			NotNullPtr<UI::GUILabel> lblDoor2;
			NotNullPtr<UI::GUITextBox> txtDoor2;
			NotNullPtr<UI::GUILabel> lblBatteryLevel;
			NotNullPtr<UI::GUITextBox> txtBatteryLevel;
			NotNullPtr<UI::GUILabel> lblMotorRPM;
			NotNullPtr<UI::GUITextBox> txtMotorRPM;
			NotNullPtr<UI::GUILabel> lblBatteryStartCharging;
			NotNullPtr<UI::GUITextBox> txtBatteryStartCharging;
			NotNullPtr<UI::GUILabel> lblLeftMotorMode;
			NotNullPtr<UI::GUITextBox> txtLeftMotorMode;
			NotNullPtr<UI::GUILabel> lblRightotorMode;
			NotNullPtr<UI::GUITextBox> txtRightMotorMode;
			NotNullPtr<UI::GUILabel> lblCarbinDoorBack;
			NotNullPtr<UI::GUITextBox> txtCarbinDoorBack;
			NotNullPtr<UI::GUILabel> lblCarbinDoorLeft;
			NotNullPtr<UI::GUITextBox> txtCarbinDoorLeft;
			NotNullPtr<UI::GUILabel> lblCarbinDoorRight;
			NotNullPtr<UI::GUITextBox> txtCarbinDoorRight;
			NotNullPtr<UI::GUILabel> lblPowerMode;
			NotNullPtr<UI::GUITextBox> txtPowerMode;
			NotNullPtr<UI::GUILabel> lblBatteryCharging;
			NotNullPtr<UI::GUITextBox> txtBatteryCharging;
			NotNullPtr<UI::GUILabel> lblOkLED;
			NotNullPtr<UI::GUITextBox> txtOkLED;

			IO::Device::BYDC9R::DeviceStatus dispStatus;

			static void __stdcall OnCANBusClicked(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
		public:
			AVIRBYDC9RForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRBYDC9RForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
