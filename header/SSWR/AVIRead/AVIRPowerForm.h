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
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUILabel> lblBattery;
			NN<UI::GUITextBox> txtBattery;
			NN<UI::GUILabel> lblTimeLeft;
			NN<UI::GUITextBox> txtTimeLeft;
			NN<UI::GUICheckBox> chkNoSleep;
			NN<UI::GUICheckBox> chkNoDispOff;
			NN<UI::GUIButton> btnSleep;
			NN<UI::GUIButton> btnDisplayOff;

			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnSleepClicked(AnyType userObj);
			static void __stdcall OnDisplayOffClicked(AnyType userObj);
		public:
			AVIRPowerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRPowerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
