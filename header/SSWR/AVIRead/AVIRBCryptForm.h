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
			NotNullPtr<UI::GUITextBox> txtCost;
			NotNullPtr<UI::GUILabel> lblGenPassword;
			NotNullPtr<UI::GUITextBox> txtGenPassword;
			NotNullPtr<UI::GUIButton> btnGenerate;
			NotNullPtr<UI::GUILabel> lblGenHash;
			NotNullPtr<UI::GUITextBox> txtGenHash;

			NotNullPtr<UI::GUIGroupBox> grpCheck;
			NotNullPtr<UI::GUILabel> lblCheckHash;
			NotNullPtr<UI::GUITextBox> txtCheckHash;
			NotNullPtr<UI::GUILabel> lblCheckPassword;
			NotNullPtr<UI::GUITextBox> txtCheckPassword;
			NotNullPtr<UI::GUIButton> btnCheck;
			NotNullPtr<UI::GUILabel> lblCheckResult;
			NotNullPtr<UI::GUITextBox> txtCheckResult;

			static void __stdcall OnGenHashClicked(AnyType userObj);
			static void __stdcall OnCheckClicked(AnyType userObj);
		public:
			AVIRBCryptForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRBCryptForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
