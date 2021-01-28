#ifndef _SM_SSWR_AVIREAD_AVIRPASSWORDHASHFORM
#define _SM_SSWR_AVIREAD_AVIRPASSWORDHASHFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRPasswordHashForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;

			UI::GUILabel *lblPassword;
			UI::GUITextBox *txtPassword;
			UI::GUILabel *lblHashType;
			UI::GUIComboBox *cboHashType;
			UI::GUIButton *btnGenerate;
			UI::GUILabel *lblHashValue;
			UI::GUITextBox *txtHashValue;

			static void __stdcall OnGenerateClicked(void *userObj);
		public:
			AVIRPasswordHashForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRPasswordHashForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
