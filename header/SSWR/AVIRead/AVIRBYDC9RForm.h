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
			UI::GUILabel *lblCANBus;
			UI::GUITextBox *txtCANBus;
			NotNullPtr<UI::GUIButton> btnCANBus;

			UI::GUILabel *lblSpeedKmHr;
			UI::GUITextBox *txtSpeedKmHr;
			UI::GUILabel *lblDoor1;
			UI::GUITextBox *txtDoor1;
			UI::GUILabel *lblDoor2;
			UI::GUITextBox *txtDoor2;
			UI::GUILabel *lblBatteryLevel;
			UI::GUITextBox *txtBatteryLevel;
			UI::GUILabel *lblMotorRPM;
			UI::GUITextBox *txtMotorRPM;
			UI::GUILabel *lblBatteryStartCharging;
			UI::GUITextBox *txtBatteryStartCharging;
			UI::GUILabel *lblLeftMotorMode;
			UI::GUITextBox *txtLeftMotorMode;
			UI::GUILabel *lblRightotorMode;
			UI::GUITextBox *txtRightMotorMode;
			UI::GUILabel *lblCarbinDoorBack;
			UI::GUITextBox *txtCarbinDoorBack;
			UI::GUILabel *lblCarbinDoorLeft;
			UI::GUITextBox *txtCarbinDoorLeft;
			UI::GUILabel *lblCarbinDoorRight;
			UI::GUITextBox *txtCarbinDoorRight;
			UI::GUILabel *lblPowerMode;
			UI::GUITextBox *txtPowerMode;
			UI::GUILabel *lblBatteryCharging;
			UI::GUITextBox *txtBatteryCharging;
			UI::GUILabel *lblOkLED;
			UI::GUITextBox *txtOkLED;

			IO::Device::BYDC9R::DeviceStatus dispStatus;

			static void __stdcall OnCANBusClicked(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
		public:
			AVIRBYDC9RForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRBYDC9RForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
