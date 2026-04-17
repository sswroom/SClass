#ifndef _SM_SSWR_AVIREAD_AVIRDATARATEFORM
#define _SM_SSWR_AVIREAD_AVIRDATARATEFORM
#include "IO/DataRateCalc.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRDataRateForm : public UI::GUIForm
		{
		private:
			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUIButton> btnSave;
			NN<UI::GUIListView> lvDataRates;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<IO::DataRateCalc> dataRate;

			static void __stdcall OnSaveClicked(AnyType userObj);

		public:
			AVIRDataRateForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<IO::DataRateCalc> dataRate);
			virtual ~AVIRDataRateForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
