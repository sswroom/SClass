#ifndef _SM_SSWR_AVIREAD_AVIRASN1PARSEFORM
#define _SM_SSWR_AVIREAD_AVIRASN1PARSEFORM
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
		class AVIRASN1ParseForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;

			UI::GUIPanel *pnlOptions;
			UI::GUILabel *lblType;
			UI::GUIComboBox *cboType;
			UI::GUILabel *lblData;
			UI::GUITextBox *txtData;
			UI::GUIPanel *pnlControl;
			UI::GUIButton *btnParse;

			static void __stdcall OnParseClicked(void *userObj);
		public:
			AVIRASN1ParseForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRASN1ParseForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif