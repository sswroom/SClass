#ifndef _SM_SSWR_AVIREAD_AVIRINVESTMENTASSETFORM
#define _SM_SSWR_AVIREAD_AVIRINVESTMENTASSETFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRInvestmentAssetForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Text::String> shortName;
			Optional<Text::String> fullName;
			UInt32 currency;

			NN<UI::GUILabel> lblShortName;
			NN<UI::GUITextBox> txtShortName;
			NN<UI::GUILabel> lblFullName;
			NN<UI::GUITextBox> txtFullName;
			NN<UI::GUILabel> lblCurrency;
			NN<UI::GUITextBox> txtCurrency;
			NN<UI::GUIButton> btnAdd;
			NN<UI::GUIButton> btnCancel;

			static void __stdcall OnAddClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
		public:
			AVIRInvestmentAssetForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRInvestmentAssetForm();

			virtual void OnMonitorChanged();

			NN<Text::String> GetShortName() const { return Text::String::OrEmpty(this->shortName); }
			NN<Text::String> GetFullName() const { return Text::String::OrEmpty(this->fullName); }
			UInt32 GetCurrency() const { return this->currency; }
		};
	}
}
#endif
