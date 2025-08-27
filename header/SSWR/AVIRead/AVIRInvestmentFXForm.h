#ifndef _SM_SSWR_AVIREAD_AVIRINVESTMENTFXFORM
#define _SM_SSWR_AVIREAD_AVIRINVESTMENTFXFORM
#include "Data/Invest/InvestmentManager.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRInvestmentFXForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Data::Invest::InvestmentManager> mgr;

			NN<UI::GUILabel> lblDate;
			NN<UI::GUITextBox> txtDate;
			NN<UI::GUILabel> lblCurrency1;
			NN<UI::GUITextBox> txtCurrency1;
			NN<UI::GUITextBox> txtValue1;
			NN<UI::GUILabel> lblCurrency2;
			NN<UI::GUITextBox> txtCurrency2;
			NN<UI::GUITextBox> txtValue2;
			NN<UI::GUILabel> lblRefRate;
			NN<UI::GUITextBox> txtRefRateName;
			NN<UI::GUITextBox> txtRefRate;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
			static void __stdcall OnCurrencyChanged(AnyType userObj);
		public:
			AVIRInvestmentFXForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Data::Invest::InvestmentManager> mgr);
			virtual ~AVIRInvestmentFXForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
