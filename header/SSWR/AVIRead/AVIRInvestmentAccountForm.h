#ifndef _SM_SSWR_AVIREAD_AVIRINVESTMENTACCOUNTFORM
#define _SM_SSWR_AVIREAD_AVIRINVESTMENTACCOUNTFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRInvestmentAccountForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Text::String> inputName;

			NN<UI::GUILabel> lblName;
			NN<UI::GUITextBox> txtName;
			NN<UI::GUIButton> btnAdd;
			NN<UI::GUIButton> btnCancel;

			static void __stdcall OnAddClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
		public:
			AVIRInvestmentAccountForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRInvestmentAccountForm();

			virtual void OnMonitorChanged();

			Optional<Text::String> GetInputName() const { return this->inputName; }
		};
	}
}
#endif
