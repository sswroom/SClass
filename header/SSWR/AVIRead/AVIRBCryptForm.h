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
			SSWR::AVIRead::AVIRCore *core;

			UI::GUIGroupBox *grpGenerate;
			UI::GUILabel *lblCost;
			UI::GUITextBox *txtCost;
			UI::GUILabel *lblGenPassword;
			UI::GUITextBox *txtGenPassword;
			UI::GUIButton *btnGenerate;
			UI::GUILabel *lblGenHash;
			UI::GUITextBox *txtGenHash;

			UI::GUIGroupBox *grpCheck;
			UI::GUILabel *lblCheckHash;
			UI::GUITextBox *txtCheckHash;
			UI::GUILabel *lblCheckPassword;
			UI::GUITextBox *txtCheckPassword;
			UI::GUIButton *btnCheck;
			UI::GUILabel *lblCheckResult;
			UI::GUITextBox *txtCheckResult;

			static void __stdcall OnGenHashClicked(void *userObj);
			static void __stdcall OnCheckClicked(void *userObj);
		public:
			AVIRBCryptForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRBCryptForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
