#ifndef _SM_SSWR_AVIREAD_AVIRCERTTEXTFORM
#define _SM_SSWR_AVIREAD_AVIRCERTTEXTFORM
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
		class AVIRCertTextForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;

			UI::GUIPanel *pnlButton;
			UI::GUIPanel *pnlLabel;
			UI::GUILabel *lblEncType;
			UI::GUIComboBox *cboEncType;
			UI::GUILabel *lblText;
			UI::GUITextBox *txtText;
			UI::GUIButton *btnLoad;

			static void __stdcall OnLoadClicked(void *userObj);
		public:
			AVIRCertTextForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRCertTextForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
