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
			NN<UI::GUILabel> lblSN;
			NN<UI::GUITextBox> txtSN;
			NN<UI::GUIButton> btnSN;
			NN<UI::GUILabel> lblTemp;
			NN<UI::GUITextBox> txtTemp;
			NN<UI::GUIButton> btnRead;
			NN<UI::GUICheckBox> chkAutoRead;
			NN<UI::GUILabel> lblStatus;
			NN<UI::GUITextBox> txtStatus;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<IO::IOPin> pin;
			NN<IO::OneWireGPIO> oneWire;
			NN<IO::Device::DS18B20> ds18b20;

			static void __stdcall OnSNClicked(AnyType userObj);
			static void __stdcall OnReadClicked(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			void ReadData();
		public:
			AVIRDS18B20Form(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<IO::IOPin> pin);
			virtual ~AVIRDS18B20Form();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
