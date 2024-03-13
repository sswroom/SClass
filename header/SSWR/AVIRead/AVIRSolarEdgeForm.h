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
			NotNullPtr<UI::GUILabel> lblAPIKey;
			NotNullPtr<UI::GUITextBox> txtAPIKey;
			NotNullPtr<UI::GUIButton> btnAPIKey;
			NotNullPtr<UI::GUITabControl> tcMain;
			
			NotNullPtr<UI::GUITabPage> tpVersion;
			NotNullPtr<UI::GUILabel> lblCurrVer;
			NotNullPtr<UI::GUITextBox> txtCurrVer;
			NotNullPtr<UI::GUILabel> lblSuppVer;
			NotNullPtr<UI::GUITextBox> txtSuppVer;

			NotNullPtr<UI::GUITabPage> tpSiteList;
			NotNullPtr<UI::GUIPanel> pnlSiteList;
			NotNullPtr<UI::GUIListView> lvSiteList;
			NotNullPtr<UI::GUILabel> lblSiteCountry;
			NotNullPtr<UI::GUITextBox> txtSiteCountry;
			NotNullPtr<UI::GUILabel> lblSiteCity;
			NotNullPtr<UI::GUITextBox> txtSiteCity;
			NotNullPtr<UI::GUILabel> lblSiteAddress;
			NotNullPtr<UI::GUITextBox> txtSiteAddress;
			NotNullPtr<UI::GUILabel> lblSiteAddress2;
			NotNullPtr<UI::GUITextBox> txtSiteAddress2;
			NotNullPtr<UI::GUILabel> lblSiteZIP;
			NotNullPtr<UI::GUITextBox> txtSiteZIP;
			NotNullPtr<UI::GUILabel> lblSiteTimeZone;
			NotNullPtr<UI::GUITextBox> txtSiteTimeZone;
			NotNullPtr<UI::GUILabel> lblSiteCountryCode;
			NotNullPtr<UI::GUITextBox> txtSiteCountryCode;
			NotNullPtr<UI::GUILabel> lblSiteIsPublic;
			NotNullPtr<UI::GUITextBox> txtSiteIsPublic;
			NotNullPtr<UI::GUILabel> lblSitePublicName;
			NotNullPtr<UI::GUITextBox> txtSitePublicName;
			NotNullPtr<UI::GUILabel> lblSiteLifetimeEnergy;
			NotNullPtr<UI::GUITextBox> txtSiteLifetimeEnergy;
			NotNullPtr<UI::GUILabel> lblSiteLifetimeRevenue;
			NotNullPtr<UI::GUITextBox> txtSiteLifetimeRevenue;
			NotNullPtr<UI::GUILabel> lblSiteYearlyEnergy;
			NotNullPtr<UI::GUITextBox> txtSiteYearlyEnergy;
			NotNullPtr<UI::GUILabel> lblSiteMonthlyEnergy;
			NotNullPtr<UI::GUITextBox> txtSiteMonthlyEnergy;
			NotNullPtr<UI::GUILabel> lblSiteDailyEnergy;
			NotNullPtr<UI::GUITextBox> txtSiteDailyEnergy;
			NotNullPtr<UI::GUILabel> lblSiteCurrentPower;
			NotNullPtr<UI::GUITextBox> txtSiteCurrentPower;

			NotNullPtr<UI::GUITabPage> tpSiteEnergy;
			NotNullPtr<UI::GUIPanel> pnlSiteEnergy;
			NotNullPtr<UI::GUILabel> lblSiteEnergySite;
			NotNullPtr<UI::GUIComboBox> cboSiteEnergySite;
			NotNullPtr<UI::GUILabel> lblSiteEnergyInterval;
			NotNullPtr<UI::GUIComboBox> cboSiteEnergyInterval;
			NotNullPtr<UI::GUILabel> lblSiteEnergyRange;
			NotNullPtr<UI::GUIComboBox> cboSiteEnergyYear;
			NotNullPtr<UI::GUIComboBox> cboSiteEnergyMonth;
			NotNullPtr<UI::GUIComboBox> cboSiteEnergyDay;
			NotNullPtr<UI::GUIButton> btnSiteEnergy;
			NotNullPtr<UI::GUIPictureBox> pbSiteEnergy;

			NotNullPtr<UI::GUITabPage> tpSitePower;
			NotNullPtr<UI::GUIPanel> pnlSitePower;
			NotNullPtr<UI::GUILabel> lblSitePowerSite;
			NotNullPtr<UI::GUIComboBox> cboSitePowerSite;
			NotNullPtr<UI::GUILabel> lblSitePowerRange;
			NotNullPtr<UI::GUIComboBox> cboSitePowerYear;
			NotNullPtr<UI::GUIComboBox> cboSitePowerMonth;
			NotNullPtr<UI::GUIComboBox> cboSitePowerDay;
			NotNullPtr<UI::GUIButton> btnSitePower;
			NotNullPtr<UI::GUIPictureBox> pbSitePower;

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
			AVIRSolarEdgeForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSolarEdgeForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
