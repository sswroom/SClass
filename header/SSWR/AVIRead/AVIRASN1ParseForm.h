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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			NotNullPtr<UI::GUIPanel> pnlOptions;
			NotNullPtr<UI::GUILabel> lblType;
			NotNullPtr<UI::GUIComboBox> cboType;
			NotNullPtr<UI::GUILabel> lblData;
			NotNullPtr<UI::GUITextBox> txtData;
			NotNullPtr<UI::GUIPanel> pnlControl;
			NotNullPtr<UI::GUIButton> btnParse;

			static void __stdcall OnParseClicked(void *userObj);
		public:
			AVIRASN1ParseForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRASN1ParseForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
