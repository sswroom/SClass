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
			NotNullPtr<UI::GUILabel> lblTemp;
			NotNullPtr<UI::GUITextBox> txtTemp;
			NotNullPtr<UI::GUILabel> lblRH;
			NotNullPtr<UI::GUITextBox> txtRH;
			NotNullPtr<UI::GUILabel> lblHumidity;
			NotNullPtr<UI::GUITextBox> txtHumidity;
			NotNullPtr<UI::GUIButton> btnRead;
			NotNullPtr<UI::GUICheckBox> chkAutoRead;
			NotNullPtr<UI::GUILabel> lblStatus;
			NotNullPtr<UI::GUITextBox> txtStatus;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<IO::IOPin> pin;
			IO::Device::DHT22 *dht22;

			static void __stdcall OnReadClicked(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			void ReadData();
		public:
			AVIRDHT22Form(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<IO::IOPin> pin);
			virtual ~AVIRDHT22Form();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
