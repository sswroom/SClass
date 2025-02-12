#ifndef _SM_SSWR_AVIREAD_AVIRSIDERUNNERLOGFORM
#define _SM_SSWR_AVIREAD_AVIRSIDERUNNERLOGFORM
#include "IO/SeleniumIDE.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSIDERunnerLogForm : public UI::GUIForm
		{
		public:
			struct StepStat
			{
				UOSInt count;
				Double totalTime;
				Double minTime;
				Double maxTime;
			};
		private:
			NN<UI::GUIPanel> pnlFile;
			NN<UI::GUIPanel> pnlCtrl;
			NN<UI::GUIListView> lvSteps;
			NN<UI::GUILabel> lblFile;
			NN<UI::GUITextBox> txtFile;
			NN<UI::GUIButton> btnSave;

			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<IO::SeleniumIDE> side;
			Data::ArrayListNN<StepStat> steps;

			static void __stdcall OnFiles(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			static void __stdcall OnExportClicked(AnyType userObj);
		public:
			AVIRSIDERunnerLogForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSIDERunnerLogForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
