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
			UI::GUILabel *lblAddr;
			UI::GUITextBox *txtAddr;
			UI::GUIButton *btnValidate;
			UI::GUILabel *lblStatus;
			UI::GUITextBox *txtStatus;

			SSWR::AVIRead::AVIRCore *core;
			Net::Email::EmailValidator *validator;

			static void __stdcall OnValidateClicked(void *userObj);
		public:
			AVIREmailAddrValidForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIREmailAddrValidForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
