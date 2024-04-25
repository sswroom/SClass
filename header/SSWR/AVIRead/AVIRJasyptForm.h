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
			NN<UI::GUILabel> lblKeyAlg;
			NN<UI::GUIComboBox> cboKeyAlg;
			NN<UI::GUILabel> lblEncAlg;
			NN<UI::GUIComboBox> cboEncAlg;
			NN<UI::GUILabel> lblPassword;
			NN<UI::GUITextBox> txtPassword;
			NN<UI::GUILabel> lblMessage;
			NN<UI::GUITextBox> txtMessage;
			NN<UI::GUIButton> btnDecrypt;
			NN<UI::GUIButton> btnEncrypt;
			NN<UI::GUILabel> lblResult;
			NN<UI::GUITextBox> txtResult;

			NN<SSWR::AVIRead::AVIRCore> core;

			static void __stdcall OnDecryptClicked(AnyType userObj);
			static void __stdcall OnEncryptClicked(AnyType userObj);
		public:
			AVIRJasyptForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRJasyptForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
