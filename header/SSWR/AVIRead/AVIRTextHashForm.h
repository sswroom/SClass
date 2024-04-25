#ifndef _SM_SSWR_AVIREAD_AVIRTEXTHASHFORM
#define _SM_SSWR_AVIREAD_AVIRTEXTHASHFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "Text/TextBinEnc/TextBinEncList.h"
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
		class AVIRTextHashForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Text::TextBinEnc::TextBinEncList encList;

			NN<UI::GUILabel> lblText;
			NN<UI::GUITextBox> txtText;
			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUILabel> lblEncrypt;
			NN<UI::GUIComboBox> cboEncrypt;
			NN<UI::GUILabel> lblHashType;
			NN<UI::GUIComboBox> cboHashType;
			NN<UI::GUIButton> btnGenerate;
			NN<UI::GUILabel> lblHashValue;
			NN<UI::GUITextBox> txtHashValue;

			static void __stdcall OnGenerateClicked(AnyType userObj);
		public:
			AVIRTextHashForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRTextHashForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
