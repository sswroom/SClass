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

			UI::GUILabel *lblBattery;
			UI::GUITextBox *txtBattery;
			UI::GUILabel *lblTimeLeft;
			UI::GUITextBox *txtTimeLeft;
			UI::GUICheckBox *chkNoSleep;
			UI::GUICheckBox *chkNoDispOff;
			UI::GUIButton *btnSleep;
			UI::GUIButton *btnDisplayOff;

			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnSleepClicked(void *userObj);
			static void __stdcall OnDisplayOffClicked(void *userObj);
		public:
			AVIRPowerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRPowerForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
