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
			NotNullPtr<UI::GUILabel> lblKeyAlg;
			NotNullPtr<UI::GUIComboBox> cboKeyAlg;
			NotNullPtr<UI::GUILabel> lblEncAlg;
			NotNullPtr<UI::GUIComboBox> cboEncAlg;
			NotNullPtr<UI::GUILabel> lblPassword;
			NotNullPtr<UI::GUITextBox> txtPassword;
			NotNullPtr<UI::GUILabel> lblMessage;
			NotNullPtr<UI::GUITextBox> txtMessage;
			NotNullPtr<UI::GUIButton> btnDecrypt;
			NotNullPtr<UI::GUIButton> btnEncrypt;
			NotNullPtr<UI::GUILabel> lblResult;
			NotNullPtr<UI::GUITextBox> txtResult;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			static void __stdcall OnDecryptClicked(AnyType userObj);
			static void __stdcall OnEncryptClicked(AnyType userObj);
		public:
			AVIRJasyptForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRJasyptForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
