#ifndef _SM_SSWR_AVIREAD_AVIRINVESTMENTFORM
#define _SM_SSWR_AVIREAD_AVIRINVESTMENTFORM
#include "Data/Invest/InvestmentManager.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPictureBox.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRInvestmentForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Data::Invest::InvestmentManager> mgr;
			NN<Media::DrawEngine> deng;
			Optional<Media::StaticImage> currencyImg;

			NN<UI::GUIPanel> pnlDir;
			NN<UI::GUILabel> lblDir;
			NN<UI::GUITextBox> txtDir;
			NN<UI::GUIButton> btnDir;
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpCurrency;
			NN<UI::GUIListBox> lbCurrency;
			NN<UI::GUIPanel> pnlCurrency;
			NN<UI::GUILabel> lblCurrencyCurr;
			NN<UI::GUITextBox> txtCurrencyCurr;
			NN<UI::GUIButton> btnCurrencyImport;
			NN<UI::GUIPictureBox> pbCurrency;

			NN<UI::GUITabPage> tpAccounts;
			NN<UI::GUIPanel> pnlAccounts;
			NN<UI::GUIButton> btnAccounts;
			NN<UI::GUIListBox> lbAccounts;
			
			NN<UI::GUITabPage> tpSettings;
			NN<UI::GUILabel> lblLocalCurrency;
			NN<UI::GUITextBox> txtLocalCurrency;
			NN<UI::GUILabel> lblRefCurrency;
			NN<UI::GUITextBox> txtRefCurrency;

			static void __stdcall OnDirClicked(AnyType userObj);
			static void __stdcall OnCurrencyImportClicked(AnyType userObj);
			static void __stdcall OnCurrencySelChg(AnyType userObj);
			static void __stdcall OnCurrencySizeChg(AnyType userObj);
			static void __stdcall OnAccountsClicked(AnyType userObj);
		
			void UpdateCurrencyList(NN<Data::Invest::InvestmentManager> mgr);
			void DisplayCurrency(NN<Data::Invest::Currency> curr);
			void DisplayCurrencyImg(NN<Data::Invest::Currency> curr);
		public:
			AVIRInvestmentForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRInvestmentForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
