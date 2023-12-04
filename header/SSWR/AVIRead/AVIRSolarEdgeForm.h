#ifndef _SM_SSWR_AVIREAD_AVIRSOLAREDGEFORM
#define _SM_SSWR_AVIREAD_AVIRSOLAREDGEFORM
#include "Net/SolarEdgeAPI.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPictureBox.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSolarEdgeForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			Net::SolarEdgeAPI *seAPI;
			Data::ArrayList<Net::SolarEdgeAPI::Site*> siteList;
			Data::ArrayList<Net::SolarEdgeAPI::TimedValue> siteEnergyList;
			Data::ArrayList<Net::SolarEdgeAPI::TimedValue> sitePowerList;
			Media::StaticImage *imgSiteEnergy;
			Media::StaticImage *imgSitePower;

			NotNullPtr<UI::GUIPanel> pnlAPIKey;
			UI::GUILabel *lblAPIKey;
			UI::GUITextBox *txtAPIKey;
			UI::GUIButton *btnAPIKey;
			UI::GUITabControl *tcMain;
			
			NotNullPtr<UI::GUITabPage> tpVersion;
			UI::GUILabel *lblCurrVer;
			UI::GUITextBox *txtCurrVer;
			UI::GUILabel *lblSuppVer;
			UI::GUITextBox *txtSuppVer;

			NotNullPtr<UI::GUITabPage> tpSiteList;
			NotNullPtr<UI::GUIPanel> pnlSiteList;
			UI::GUIListView *lvSiteList;
			UI::GUILabel *lblSiteCountry;
			UI::GUITextBox *txtSiteCountry;
			UI::GUILabel *lblSiteCity;
			UI::GUITextBox *txtSiteCity;
			UI::GUILabel *lblSiteAddress;
			UI::GUITextBox *txtSiteAddress;
			UI::GUILabel *lblSiteAddress2;
			UI::GUITextBox *txtSiteAddress2;
			UI::GUILabel *lblSiteZIP;
			UI::GUITextBox *txtSiteZIP;
			UI::GUILabel *lblSiteTimeZone;
			UI::GUITextBox *txtSiteTimeZone;
			UI::GUILabel *lblSiteCountryCode;
			UI::GUITextBox *txtSiteCountryCode;
			UI::GUILabel *lblSiteIsPublic;
			UI::GUITextBox *txtSiteIsPublic;
			UI::GUILabel *lblSitePublicName;
			UI::GUITextBox *txtSitePublicName;
			UI::GUILabel *lblSiteLifetimeEnergy;
			UI::GUITextBox *txtSiteLifetimeEnergy;
			UI::GUILabel *lblSiteLifetimeRevenue;
			UI::GUITextBox *txtSiteLifetimeRevenue;
			UI::GUILabel *lblSiteYearlyEnergy;
			UI::GUITextBox *txtSiteYearlyEnergy;
			UI::GUILabel *lblSiteMonthlyEnergy;
			UI::GUITextBox *txtSiteMonthlyEnergy;
			UI::GUILabel *lblSiteDailyEnergy;
			UI::GUITextBox *txtSiteDailyEnergy;
			UI::GUILabel *lblSiteCurrentPower;
			UI::GUITextBox *txtSiteCurrentPower;

			NotNullPtr<UI::GUITabPage> tpSiteEnergy;
			NotNullPtr<UI::GUIPanel> pnlSiteEnergy;
			UI::GUILabel *lblSiteEnergySite;
			UI::GUIComboBox *cboSiteEnergySite;
			UI::GUILabel *lblSiteEnergyInterval;
			UI::GUIComboBox *cboSiteEnergyInterval;
			UI::GUILabel *lblSiteEnergyRange;
			NotNullPtr<UI::GUIComboBox> cboSiteEnergyYear;
			NotNullPtr<UI::GUIComboBox> cboSiteEnergyMonth;
			NotNullPtr<UI::GUIComboBox> cboSiteEnergyDay;
			UI::GUIButton *btnSiteEnergy;
			UI::GUIPictureBox *pbSiteEnergy;

			NotNullPtr<UI::GUITabPage> tpSitePower;
			NotNullPtr<UI::GUIPanel> pnlSitePower;
			UI::GUILabel *lblSitePowerSite;
			UI::GUIComboBox *cboSitePowerSite;
			UI::GUILabel *lblSitePowerRange;
			NotNullPtr<UI::GUIComboBox> cboSitePowerYear;
			NotNullPtr<UI::GUIComboBox> cboSitePowerMonth;
			NotNullPtr<UI::GUIComboBox> cboSitePowerDay;
			UI::GUIButton *btnSitePower;
			UI::GUIPictureBox *pbSitePower;

			static void __stdcall OnAPIKeyClicked(void *userObj);
			static void __stdcall OnSiteListSelChg(void *userObj);
			static void __stdcall OnSiteEnergyClicked(void *userObj);
			static void __stdcall OnSiteEnergySizeChg(void *userObj);
			static void __stdcall OnSitePowerClicked(void *userObj);
			static void __stdcall OnSitePowerSizeChg(void *userObj);

			static Data::Timestamp GetDefaultStartTime(const Data::Timestamp &startTime, Net::SolarEdgeAPI::TimeUnit timeUnit);
			static Data::Timestamp GetDefaultEndTime(const Data::Timestamp &startTime, Net::SolarEdgeAPI::TimeUnit timeUnit);
			void UpdateSiteEnergyGraph();
			void UpdateSitePowerGraph();
		public:
			AVIRSolarEdgeForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSolarEdgeForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
