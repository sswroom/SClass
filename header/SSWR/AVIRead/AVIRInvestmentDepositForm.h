#ifndef _SM_SSWR_AVIREAD_AVIRINVESTMENTDEPOSITFORM
#define _SM_SSWR_AVIREAD_AVIRINVESTMENTDEPOSITFORM
#include "Data/Invest/InvestmentManager.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRInvestmentDepositForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Data::Invest::InvestmentManager> mgr;

			NN<UI::GUILabel> lblStartDate;
			NN<UI::GUITextBox> txtStartDate;
			NN<UI::GUILabel> lblEndDate;
			NN<UI::GUITextBox> txtEndDate;
			NN<UI::GUILabel> lblCurrency;
			NN<UI::GUITextBox> txtCurrency;
			NN<UI::GUILabel> lblStartAmount;
			NN<UI::GUITextBox> txtStartAmount;
			NN<UI::GUILabel> lblEndAmount;
			NN<UI::GUITextBox> txtEndAmount;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
		public:
			AVIRInvestmentDepositForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Data::Invest::InvestmentManager> mgr);
			virtual ~AVIRInvestmentDepositForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
