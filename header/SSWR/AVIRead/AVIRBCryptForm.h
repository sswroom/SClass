#ifndef _SM_SSWR_AVIREAD_AVIRBCRYPTFORM
#define _SM_SSWR_AVIREAD_AVIRBCRYPTFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRBCryptForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			NotNullPtr<UI::GUIGroupBox> grpGenerate;
			NotNullPtr<UI::GUILabel> lblCost;
			UI::GUITextBox *txtCost;
			NotNullPtr<UI::GUILabel> lblGenPassword;
			UI::GUITextBox *txtGenPassword;
			NotNullPtr<UI::GUIButton> btnGenerate;
			NotNullPtr<UI::GUILabel> lblGenHash;
			UI::GUITextBox *txtGenHash;

			NotNullPtr<UI::GUIGroupBox> grpCheck;
			NotNullPtr<UI::GUILabel> lblCheckHash;
			UI::GUITextBox *txtCheckHash;
			NotNullPtr<UI::GUILabel> lblCheckPassword;
			UI::GUITextBox *txtCheckPassword;
			NotNullPtr<UI::GUIButton> btnCheck;
			NotNullPtr<UI::GUILabel> lblCheckResult;
			UI::GUITextBox *txtCheckResult;

			static void __stdcall OnGenHashClicked(void *userObj);
			static void __stdcall OnCheckClicked(void *userObj);
		public:
			AVIRBCryptForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRBCryptForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
