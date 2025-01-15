#ifndef _SM_SSWR_AVIREAD_AVIRDHT22FORM
#define _SM_SSWR_AVIREAD_AVIRDHT22FORM
#include "IO/IOPin.h"
#include "IO/Device/DHT22.h"
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
		class AVIRDHT22Form : public UI::GUIForm
		{
		private:
			NN<UI::GUILabel> lblTemp;
			NN<UI::GUITextBox> txtTemp;
			NN<UI::GUILabel> lblRH;
			NN<UI::GUITextBox> txtRH;
			NN<UI::GUILabel> lblHumidity;
			NN<UI::GUITextBox> txtHumidity;
			NN<UI::GUIButton> btnRead;
			NN<UI::GUICheckBox> chkAutoRead;
			NN<UI::GUILabel> lblStatus;
			NN<UI::GUITextBox> txtStatus;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<IO::IOPin> pin;
			NN<IO::Device::DHT22> dht22;

			static void __stdcall OnReadClicked(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			void ReadData();
		public:
			AVIRDHT22Form(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<IO::IOPin> pin);
			virtual ~AVIRDHT22Form();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
