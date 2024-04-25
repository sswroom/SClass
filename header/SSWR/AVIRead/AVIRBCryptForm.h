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
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUIGroupBox> grpGenerate;
			NN<UI::GUILabel> lblCost;
			NN<UI::GUITextBox> txtCost;
			NN<UI::GUILabel> lblGenPassword;
			NN<UI::GUITextBox> txtGenPassword;
			NN<UI::GUIButton> btnGenerate;
			NN<UI::GUILabel> lblGenHash;
			NN<UI::GUITextBox> txtGenHash;

			NN<UI::GUIGroupBox> grpCheck;
			NN<UI::GUILabel> lblCheckHash;
			NN<UI::GUITextBox> txtCheckHash;
			NN<UI::GUILabel> lblCheckPassword;
			NN<UI::GUITextBox> txtCheckPassword;
			NN<UI::GUIButton> btnCheck;
			NN<UI::GUILabel> lblCheckResult;
			NN<UI::GUITextBox> txtCheckResult;

			static void __stdcall OnGenHashClicked(AnyType userObj);
			static void __stdcall OnCheckClicked(AnyType userObj);
		public:
			AVIRBCryptForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRBCryptForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
