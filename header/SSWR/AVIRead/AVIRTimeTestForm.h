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
			NN<UI::GUILabel> lblSleepMS;
			NN<UI::GUITextBox> txtSleepMS;
			NN<UI::GUIButton> btnSleepMS;
			NN<UI::GUILabel> lblSleepUS;
			NN<UI::GUITextBox> txtSleepUS;
			NN<UI::GUIButton> btnSleepUS;
			NN<UI::GUILabel> lblActualTime;
			NN<UI::GUITextBox> txtActualTime;

			NN<SSWR::AVIRead::AVIRCore> core;

			static void __stdcall OnSleepMSClicked(AnyType userObj);
			static void __stdcall OnSleepUSClicked(AnyType userObj);
		public:
			AVIRTimeTestForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRTimeTestForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
