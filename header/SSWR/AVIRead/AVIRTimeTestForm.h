#ifndef _SM_SSWR_AVIREAD_AVIRTIMETESTFORM
#define _SM_SSWR_AVIREAD_AVIRTIMETESTFORM
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"
#include "SSWR/AVIRead/AVIRCore.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRTimeTestForm : public UI::GUIForm
		{
		private:
			UI::GUILabel *lblSleepMS;
			UI::GUITextBox *txtSleepMS;
			UI::GUIButton *btnSleepMS;
			UI::GUILabel *lblSleepUS;
			UI::GUITextBox *txtSleepUS;
			UI::GUIButton *btnSleepUS;
			UI::GUILabel *lblActualTime;
			UI::GUITextBox *txtActualTime;

			SSWR::AVIRead::AVIRCore *core;

			static void __stdcall OnSleepMSClicked(void *userObj);
			static void __stdcall OnSleepUSClicked(void *userObj);
		public:
			AVIRTimeTestForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRTimeTestForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
