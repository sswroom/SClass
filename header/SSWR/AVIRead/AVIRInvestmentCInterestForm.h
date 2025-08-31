#ifndef _SM_SSWR_AVIREAD_AVIRINVESTMENTCINTERESTFORM
#define _SM_SSWR_AVIREAD_AVIRINVESTMENTCINTERESTFORM
#include "Data/Invest/InvestmentManager.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRInvestmentCInterestForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Data::Invest::InvestmentManager> mgr;

			NN<UI::GUILabel> lblDate;
			NN<UI::GUITextBox> txtDate;
			NN<UI::GUILabel> lblCurrency;
			NN<UI::GUITextBox> txtCurrency;
			NN<UI::GUILabel> lblAmount;
			NN<UI::GUITextBox> txtAmount;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
		public:
			AVIRInvestmentCInterestForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Data::Invest::InvestmentManager> mgr);
			virtual ~AVIRInvestmentCInterestForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
