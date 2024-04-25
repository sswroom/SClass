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
			NN<UI::GUILabel> lblAddr;
			NN<UI::GUITextBox> txtAddr;
			NN<UI::GUIButton> btnValidate;
			NN<UI::GUILabel> lblStatus;
			NN<UI::GUITextBox> txtStatus;

			NN<SSWR::AVIRead::AVIRCore> core;
			Net::Email::EmailValidator *validator;

			static void __stdcall OnValidateClicked(AnyType userObj);
		public:
			AVIREmailAddrValidForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIREmailAddrValidForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
