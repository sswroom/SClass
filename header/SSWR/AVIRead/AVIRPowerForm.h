#ifndef _SM_SSWR_AVIREAD_AVIRPOWERFORM
#define _SM_SSWR_AVIREAD_AVIRPOWERFORM
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
		class AVIRPowerForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			NotNullPtr<UI::GUILabel> lblBattery;
			NotNullPtr<UI::GUITextBox> txtBattery;
			NotNullPtr<UI::GUILabel> lblTimeLeft;
			NotNullPtr<UI::GUITextBox> txtTimeLeft;
			NotNullPtr<UI::GUICheckBox> chkNoSleep;
			NotNullPtr<UI::GUICheckBox> chkNoDispOff;
			NotNullPtr<UI::GUIButton> btnSleep;
			NotNullPtr<UI::GUIButton> btnDisplayOff;

			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnSleepClicked(void *userObj);
			static void __stdcall OnDisplayOffClicked(void *userObj);
		public:
			AVIRPowerForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRPowerForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
