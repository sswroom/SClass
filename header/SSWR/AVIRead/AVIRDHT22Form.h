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
			UI::GUILabel *lblTemp;
			UI::GUITextBox *txtTemp;
			UI::GUILabel *lblRH;
			UI::GUITextBox *txtRH;
			UI::GUILabel *lblHumidity;
			UI::GUITextBox *txtHumidity;
			UI::GUIButton *btnRead;
			UI::GUICheckBox *chkAutoRead;
			UI::GUILabel *lblStatus;
			UI::GUITextBox *txtStatus;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::IOPin *pin;
			IO::Device::DHT22 *dht22;

			static void __stdcall OnReadClicked(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			void ReadData();
		public:
			AVIRDHT22Form(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, IO::IOPin *pin);
			virtual ~AVIRDHT22Form();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
