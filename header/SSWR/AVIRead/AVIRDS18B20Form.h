#ifndef _SM_SSWR_AVIREAD_AVIRDS18B20FORM
#define _SM_SSWR_AVIREAD_AVIRDS18B20FORM
#include "IO/IOPin.h"
#include "IO/Device/DS18B20.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRDS18B20Form : public UI::GUIForm
		{
		private:
			NotNullPtr<UI::GUILabel> lblSN;
			NotNullPtr<UI::GUITextBox> txtSN;
			NotNullPtr<UI::GUIButton> btnSN;
			NotNullPtr<UI::GUILabel> lblTemp;
			NotNullPtr<UI::GUITextBox> txtTemp;
			NotNullPtr<UI::GUIButton> btnRead;
			NotNullPtr<UI::GUICheckBox> chkAutoRead;
			NotNullPtr<UI::GUILabel> lblStatus;
			NotNullPtr<UI::GUITextBox> txtStatus;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<IO::IOPin> pin;
			IO::OneWireGPIO *oneWire;
			IO::Device::DS18B20 *ds18b20;

			static void __stdcall OnSNClicked(AnyType userObj);
			static void __stdcall OnReadClicked(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			void ReadData();
		public:
			AVIRDS18B20Form(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<IO::IOPin> pin);
			virtual ~AVIRDS18B20Form();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
