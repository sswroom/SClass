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
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUIPanel> pnlButton;
			NN<UI::GUIPanel> pnlLabel;
			NN<UI::GUILabel> lblEncType;
			NN<UI::GUIComboBox> cboEncType;
			NN<UI::GUILabel> lblText;
			NN<UI::GUITextBox> txtText;
			NN<UI::GUIButton> btnLoad;

			static void __stdcall OnLoadClicked(AnyType userObj);
		public:
			AVIRCertTextForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRCertTextForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
