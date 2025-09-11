#ifndef _SM_SSWR_AVIREAD_AVIRINVESTMENTFORM
#define _SM_SSWR_AVIREAD_AVIRINVESTMENTFORM
#include "Data/ChartPlotter.h"
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
			Optional<Data::ChartPlotter> monthlyChart;
			Optional<Data::ChartPlotter> yearlyChart;
			Optional<Media::StaticImage> monthlyImg;
			Optional<Media::StaticImage> yearlyImg;
			Optional<Media::StaticImage> currencyImg;
			Optional<Media::StaticImage> assetsImg;

			NN<UI::GUIPanel> pnlDir;
			NN<UI::GUILabel> lblDir;
			NN<UI::GUITextBox> txtDir;
			NN<UI::GUIButton> btnDir;
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpYearly;
			NN<UI::GUIPanel> pnlYearly;
			NN<UI::GUILabel> lblYearly;
			NN<UI::GUIComboBox> cboYearlyYear;
			NN<UI::GUIListView> lvYearly;
			NN<UI::GUIVSplitter> vspYearly;
			NN<UI::GUIPictureBox> pbYearly;

			NN<UI::GUITabPage> tpMonthly;
			NN<UI::GUIPanel> pnlMonthly;
			NN<UI::GUILabel> lblMonthly;
			NN<UI::GUIComboBox> cboMonthlyYear;
			NN<UI::GUIComboBox> cboMonthlyMonth;
			NN<UI::GUIListView> lvMonthly;
			NN<UI::GUIVSplitter> vspMonthly;
			NN<UI::GUIPictureBox> pbMonthly;

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
			NN<UI::GUILabel> lblCurrencyTotal;
			NN<UI::GUITextBox> txtCurrencyTotal;
			NN<UI::GUILabel> lblCurrencyValue;
			NN<UI::GUITextBox> txtCurrencyValue;
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
			NN<UI::GUITabPage> tpCurrencyTrade;
			NN<UI::GUIListView> lvCurrencyTrade;
			NN<UI::GUITabPage> tpCurrencyTotal;
			NN<UI::GUIPanel> pnlCurrencyTotal;
			NN<UI::GUILabel> lblCurrencyTotalValue;
			NN<UI::GUITextBox> txtCurrencyTotalValue;
			NN<UI::GUIListView> lvCurrencyTotal;

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
			NN<UI::GUILabel> lblAssetsCurrent;
			NN<UI::GUITextBox> txtAssetsCurrent;
			NN<UI::GUILabel> lblAssetsAmount;
			NN<UI::GUITextBox> txtAssetsAmount;
			NN<UI::GUILabel> lblAssetsValue;
			NN<UI::GUITextBox> txtAssetsValue;
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
			NN<UI::GUITabPage> tpAssetsTrade;
			NN<UI::GUIListView> lvAssetsTrade;

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
			static void __stdcall OnTransactionFXClicked(AnyType userObj);
			static void __stdcall OnTransactionDepositClicked(AnyType userObj);
			static void __stdcall OnTransactionAssetClicked(AnyType userObj);
			static void __stdcall OnTransactionAInterestClicked(AnyType userObj);
			static void __stdcall OnTransactionCInterestClicked(AnyType userObj);
			static void __stdcall OnTransactionDblClk(AnyType userObj, UOSInt index);
			static void __stdcall OnMonthlySelChg(AnyType userObj);
			static void __stdcall OnMonthlySizeChg(AnyType userObj);
			static void __stdcall OnYearlySelChg(AnyType userObj);
			static void __stdcall OnYearlySizeChg(AnyType userObj);
		
			void UpdateCurrencyList(NN<Data::Invest::InvestmentManager> mgr);
			void UpdateMonthly(NN<Data::Invest::InvestmentManager> mgr);
			void UpdateYearly(NN<Data::Invest::InvestmentManager> mgr);
			void DisplayCurrency(NN<Data::Invest::Currency> curr);
			void DisplayCurrencyImg(NN<Data::Invest::Currency> curr);
			void DisplayAsset(NN<Data::Invest::Asset> ass);
			void DisplayAssetImg(NN<Data::Invest::Asset> ass);
			void DisplayTransactions(NN<Data::Invest::InvestmentManager> mgr);
			void DisplayMonthly(NN<Data::Invest::InvestmentManager> mgr, Int32 year, UInt8 month);
			void DisplayYearly(NN<Data::Invest::InvestmentManager> mgr, Int32 year);
			void DisplayMonthlyImg();
			void DisplayYearlyImg();
			static Optional<Data::ChartPlotter> GenerateSummary(NN<Data::Invest::InvestmentManager> mgr, Data::Date startDate, Data::Date endDate, NN<UI::GUIListView> listView);
		public:
			AVIRInvestmentForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRInvestmentForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
