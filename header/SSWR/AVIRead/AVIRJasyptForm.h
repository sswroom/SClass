#ifndef _SM_SSWR_AVIREAD_AVIRJASYPTFORM
#define _SM_SSWR_AVIREAD_AVIRJASYPTFORM
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
		class AVIRJasyptForm : public UI::GUIForm
		{
		private:
			UI::GUILabel *lblKeyAlg;
			NotNullPtr<UI::GUIComboBox> cboKeyAlg;
			UI::GUILabel *lblEncAlg;
			NotNullPtr<UI::GUIComboBox> cboEncAlg;
			UI::GUILabel *lblPassword;
			UI::GUITextBox *txtPassword;
			UI::GUILabel *lblMessage;
			UI::GUITextBox *txtMessage;
			NotNullPtr<UI::GUIButton> btnDecrypt;
			NotNullPtr<UI::GUIButton> btnEncrypt;
			UI::GUILabel *lblResult;
			UI::GUITextBox *txtResult;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			static void __stdcall OnDecryptClicked(void *userObj);
			static void __stdcall OnEncryptClicked(void *userObj);
		public:
			AVIRJasyptForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRJasyptForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
