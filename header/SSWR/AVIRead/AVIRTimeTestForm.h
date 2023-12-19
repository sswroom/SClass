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
			NotNullPtr<UI::GUILabel> lblSleepMS;
			NotNullPtr<UI::GUITextBox> txtSleepMS;
			NotNullPtr<UI::GUIButton> btnSleepMS;
			NotNullPtr<UI::GUILabel> lblSleepUS;
			NotNullPtr<UI::GUITextBox> txtSleepUS;
			NotNullPtr<UI::GUIButton> btnSleepUS;
			NotNullPtr<UI::GUILabel> lblActualTime;
			NotNullPtr<UI::GUITextBox> txtActualTime;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			static void __stdcall OnSleepMSClicked(void *userObj);
			static void __stdcall OnSleepUSClicked(void *userObj);
		public:
			AVIRTimeTestForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRTimeTestForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
