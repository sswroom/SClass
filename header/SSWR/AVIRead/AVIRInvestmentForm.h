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
			Optional<Media::StaticImage> assetsImg;

			NN<UI::GUIPanel> pnlDir;
			NN<UI::GUILabel> lblDir;
			NN<UI::GUITextBox> txtDir;
			NN<UI::GUIButton> btnDir;
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpTransaction;
			NN<UI::GUIPanel> pnlTransaction;
			NN<UI::GUIButton> btnTransactionFX;
			NN<UI::GUIButton> btnTransactionDeposit;
			NN<UI::GUIButton> btnTransactionAsset;
			NN<UI::GUIButton> btnTransactionAInterest;
			NN<UI::GUIButton> btnTransactionCInterest;
			NN<UI::GUIListView> lvTransaction;

			NN<UI::GUITabPage> tpCurrency;
			NN<UI::GUITabControl> tcCurrency;
			NN<UI::GUITabPage> tpCurrencySummary;
			NN<UI::GUIListBox> lbCurrency;
			NN<UI::GUIPanel> pnlCurrency;
			NN<UI::GUILabel> lblCurrencyCurr;
			NN<UI::GUITextBox> txtCurrencyCurr;
			NN<UI::GUIButton> btnCurrencyImport;
			NN<UI::GUIPictureBox> pbCurrency;
			NN<UI::GUITabPage> tpCurrencyHist;
			NN<UI::GUIPanel> pnlCurrencyHist;
			NN<UI::GUILabel> lblCurrencyHistDate;
			NN<UI::GUITextBox> txtCurrencyHistDate;
			NN<UI::GUILabel> lblCurrencyHistValue;
			NN<UI::GUITextBox> txtCurrencyHistValue;
			NN<UI::GUIButton> btnCurrencyHistUpdate;
			NN<UI::GUIListView> lvCurrencyHist;

			NN<UI::GUITabPage> tpAssets;
			NN<UI::GUITabControl> tcAssets;
			NN<UI::GUITabPage> tpAssetsSummary;
			NN<UI::GUIPanel> pnlAssets;
			NN<UI::GUIButton> btnAssetsAdd;
			NN<UI::GUIListBox> lbAssets;
			NN<UI::GUIPanel> pnlAssetsDetail;
			NN<UI::GUILabel> lblAssetsShortName;
			NN<UI::GUITextBox> txtAssetsShortName;
			NN<UI::GUILabel> lblAssetsFullName;
			NN<UI::GUITextBox> txtAssetsFullName;
			NN<UI::GUILabel> lblAssetsCurrency;
			NN<UI::GUITextBox> txtAssetsCurrency;
			NN<UI::GUIButton> btnAssetsImport;
			NN<UI::GUIButton> btnAssetsImportDiv;
			NN<UI::GUIPictureBox> pbAssets;
			NN<UI::GUITabPage> tpAssetsHist;
			NN<UI::GUIPanel> pnlAssetsHist;
			NN<UI::GUILabel> lblAssetsHistDate;
			NN<UI::GUITextBox> txtAssetsHistDate;
			NN<UI::GUILabel> lblAssetsHistValue;
			NN<UI::GUITextBox> txtAssetsHistValue;
			NN<UI::GUILabel> lblAssetsHistDiv;
			NN<UI::GUITextBox> txtAssetsHistDiv;
			NN<UI::GUIButton> btnAssetsHistUpdate;
			NN<UI::GUIListView> lvAssetsHist;

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
			static void __stdcall OnCurrencyHistUpdateClicked(AnyType userObj);
			static void __stdcall OnCurrencyHistSelChg(AnyType userObj);
			static void __stdcall OnAssetsAddClicked(AnyType userObj);
			static void __stdcall OnAssetsSelChg(AnyType userObj);
			static void __stdcall OnAssetsSizeChg(AnyType userObj);
			static void __stdcall OnAssetsImportClicked(AnyType userObj);
			static void __stdcall OnAssetsImportDivClicked(AnyType userObj);
			static void __stdcall OnAssetsHistUpdateClicked(AnyType userObj);
			static void __stdcall OnAssetsHistSelChg(AnyType userObj);
			static void __stdcall OnAccountsClicked(AnyType userObj);
			static void __stdcall OnTransactionFXClicked(AnyType userObj);
			static void __stdcall OnTransactionDepositClicked(AnyType userObj);
			static void __stdcall OnTransactionAssetClicked(AnyType userObj);
			static void __stdcall OnTransactionAInterestClicked(AnyType userObj);
			static void __stdcall OnTransactionCInterestClicked(AnyType userObj);
		
			void UpdateCurrencyList(NN<Data::Invest::InvestmentManager> mgr);
			void DisplayCurrency(NN<Data::Invest::Currency> curr);
			void DisplayCurrencyImg(NN<Data::Invest::Currency> curr);
			void DisplayAsset(NN<Data::Invest::Asset> ass);
			void DisplayAssetImg(NN<Data::Invest::Asset> ass);
			void DisplayTransactions(NN<Data::Invest::InvestmentManager> mgr);
		public:
			AVIRInvestmentForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRInvestmentForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
