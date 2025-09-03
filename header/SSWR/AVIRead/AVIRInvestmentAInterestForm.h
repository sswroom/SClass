#ifndef _SM_SSWR_AVIREAD_AVIRINVESTMENTAINTERESTFORM
#define _SM_SSWR_AVIREAD_AVIRINVESTMENTAINTERESTFORM
#include "Data/Invest/InvestmentManager.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRInvestmentAInterestForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Data::Invest::InvestmentManager> mgr;
			Optional<Data::Invest::TradeEntry> ent;

			NN<UI::GUILabel> lblStartDate;
			NN<UI::GUITextBox> txtStartDate;
			NN<UI::GUILabel> lblEndDate;
			NN<UI::GUITextBox> txtEndDate;
			NN<UI::GUILabel> lblAsset;
			NN<UI::GUIComboBox> cboAsset;
			NN<UI::GUILabel> lblAssetAmount;
			NN<UI::GUITextBox> txtAssetAmount;
			NN<UI::GUILabel> lblCurrencyValue;
			NN<UI::GUITextBox> txtCurrencyValue;
			NN<UI::GUILabel> lblAssetDiv;
			NN<UI::GUITextBox> txtAssetDiv;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
			static void __stdcall OnAssetSelChg(AnyType userObj);
			static void __stdcall OnStartDateChg(AnyType userObj);
			static void __stdcall OnCurrencyValueChg(AnyType userObj);
		public:
			AVIRInvestmentAInterestForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Data::Invest::InvestmentManager> mgr);
			virtual ~AVIRInvestmentAInterestForm();

			virtual void OnMonitorChanged();
			void SetEntry(NN<Data::Invest::TradeEntry> ent);
		};
	}
}
#endif
