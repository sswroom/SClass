#ifndef _SM_SSWR_AVIREAD_AVIRJSTEXTFORM
#define _SM_SSWR_AVIREAD_AVIRJSTEXTFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRJSTextForm : public UI::GUIForm
		{
		private:
			UI::GUILabel *lblJSText;
			UI::GUITextBox *txtJSText;
			UI::GUIButton *btnExtract;
			UI::GUILabel *lblOriText;
			UI::GUITextBox *txtOriText;

			SSWR::AVIRead::AVIRCore *core;

			static void __stdcall OnExtractClicked(void *userObj);
		public:
			AVIRJSTextForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRJSTextForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
