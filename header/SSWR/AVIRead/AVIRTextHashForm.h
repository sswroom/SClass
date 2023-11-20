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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Text::TextBinEnc::TextBinEncList encList;

			UI::GUILabel *lblText;
			UI::GUITextBox *txtText;
			NotNullPtr<UI::GUIPanel> pnlControl;
			UI::GUILabel *lblEncrypt;
			UI::GUIComboBox *cboEncrypt;
			UI::GUILabel *lblHashType;
			UI::GUIComboBox *cboHashType;
			UI::GUIButton *btnGenerate;
			UI::GUILabel *lblHashValue;
			UI::GUITextBox *txtHashValue;

			static void __stdcall OnGenerateClicked(void *userObj);
		public:
			AVIRTextHashForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRTextHashForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
