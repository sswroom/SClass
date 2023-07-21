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
			SSWR::AVIRead::AVIRCore *core;
			void *param;
			IO::FileExporter *exporter;
			UI::GUIControl **ctrls;
			UOSInt ctrlsCnt;

		private:
			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
		public:
			AVIRExportParamForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core, IO::FileExporter *exporter, void *param);
			virtual ~AVIRExportParamForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
