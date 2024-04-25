#ifndef _SM_SSWR_AVIREAD_AVIREXPORTPARAMFORM
#define _SM_SSWR_AVIREAD_AVIREXPORTPARAMFORM
#include "IO/FileExporter.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRExportParamForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<IO::FileExporter::ParamData> param;
			IO::FileExporter *exporter;
			UI::GUIControl **ctrls;
			UOSInt ctrlsCnt;

		private:
			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
		public:
			AVIRExportParamForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, IO::FileExporter *exporter, Optional<IO::FileExporter::ParamData> param);
			virtual ~AVIRExportParamForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
