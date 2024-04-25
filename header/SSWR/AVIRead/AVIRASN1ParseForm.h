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
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUIPanel> pnlOptions;
			NN<UI::GUILabel> lblType;
			NN<UI::GUIComboBox> cboType;
			NN<UI::GUILabel> lblData;
			NN<UI::GUITextBox> txtData;
			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUIButton> btnParse;

			static void __stdcall OnParseClicked(AnyType userObj);
		public:
			AVIRASN1ParseForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRASN1ParseForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
