#ifndef _SM_SSWR_AVIREAD_AVIREMAILADDRVALIDFORM
#define _SM_SSWR_AVIREAD_AVIREMAILADDRVALIDFORM
#include "Net/Email/EmailValidator.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIREmailAddrValidForm : public UI::GUIForm
		{
		private:
			NotNullPtr<UI::GUILabel> lblAddr;
			NotNullPtr<UI::GUITextBox> txtAddr;
			NotNullPtr<UI::GUIButton> btnValidate;
			NotNullPtr<UI::GUILabel> lblStatus;
			NotNullPtr<UI::GUITextBox> txtStatus;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Net::Email::EmailValidator *validator;

			static void __stdcall OnValidateClicked(AnyType userObj);
		public:
			AVIREmailAddrValidForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIREmailAddrValidForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
