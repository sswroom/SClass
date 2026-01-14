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
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			Optional<Net::SolarEdgeAPI> seAPI;
			Data::ArrayListNN<Net::SolarEdgeAPI::Site> siteList;
			Data::ArrayListT<Net::SolarEdgeAPI::TimedValue> siteEnergyList;
			Data::ArrayListT<Net::SolarEdgeAPI::TimedValue> sitePowerList;
			Optional<Media::StaticImage> imgSiteEnergy;
			Optional<Media::StaticImage> imgSitePower;

			NN<UI::GUIPanel> pnlAPIKey;
			NN<UI::GUILabel> lblAPIKey;
			NN<UI::GUITextBox> txtAPIKey;
			NN<UI::GUIButton> btnAPIKey;
			NN<UI::GUITabControl> tcMain;
			
			NN<UI::GUITabPage> tpVersion;
			NN<UI::GUILabel> lblCurrVer;
			NN<UI::GUITextBox> txtCurrVer;
			NN<UI::GUILabel> lblSuppVer;
			NN<UI::GUITextBox> txtSuppVer;

			NN<UI::GUITabPage> tpSiteList;
			NN<UI::GUIPanel> pnlSiteList;
			NN<UI::GUIListView> lvSiteList;
			NN<UI::GUILabel> lblSiteCountry;
			NN<UI::GUITextBox> txtSiteCountry;
			NN<UI::GUILabel> lblSiteCity;
			NN<UI::GUITextBox> txtSiteCity;
			NN<UI::GUILabel> lblSiteAddress;
			NN<UI::GUITextBox> txtSiteAddress;
			NN<UI::GUILabel> lblSiteAddress2;
			NN<UI::GUITextBox> txtSiteAddress2;
			NN<UI::GUILabel> lblSiteZIP;
			NN<UI::GUITextBox> txtSiteZIP;
			NN<UI::GUILabel> lblSiteTimeZone;
			NN<UI::GUITextBox> txtSiteTimeZone;
			NN<UI::GUILabel> lblSiteCountryCode;
			NN<UI::GUITextBox> txtSiteCountryCode;
			NN<UI::GUILabel> lblSiteIsPublic;
			NN<UI::GUITextBox> txtSiteIsPublic;
			NN<UI::GUILabel> lblSitePublicName;
			NN<UI::GUITextBox> txtSitePublicName;
			NN<UI::GUILabel> lblSiteLifetimeEnergy;
			NN<UI::GUITextBox> txtSiteLifetimeEnergy;
			NN<UI::GUILabel> lblSiteLifetimeRevenue;
			NN<UI::GUITextBox> txtSiteLifetimeRevenue;
			NN<UI::GUILabel> lblSiteYearlyEnergy;
			NN<UI::GUITextBox> txtSiteYearlyEnergy;
			NN<UI::GUILabel> lblSiteMonthlyEnergy;
			NN<UI::GUITextBox> txtSiteMonthlyEnergy;
			NN<UI::GUILabel> lblSiteDailyEnergy;
			NN<UI::GUITextBox> txtSiteDailyEnergy;
			NN<UI::GUILabel> lblSiteCurrentPower;
			NN<UI::GUITextBox> txtSiteCurrentPower;

			NN<UI::GUITabPage> tpSiteEnergy;
			NN<UI::GUIPanel> pnlSiteEnergy;
			NN<UI::GUILabel> lblSiteEnergySite;
			NN<UI::GUIComboBox> cboSiteEnergySite;
			NN<UI::GUILabel> lblSiteEnergyInterval;
			NN<UI::GUIComboBox> cboSiteEnergyInterval;
			NN<UI::GUILabel> lblSiteEnergyRange;
			NN<UI::GUIComboBox> cboSiteEnergyYear;
			NN<UI::GUIComboBox> cboSiteEnergyMonth;
			NN<UI::GUIComboBox> cboSiteEnergyDay;
			NN<UI::GUIButton> btnSiteEnergy;
			NN<UI::GUIPictureBox> pbSiteEnergy;

			NN<UI::GUITabPage> tpSitePower;
			NN<UI::GUIPanel> pnlSitePower;
			NN<UI::GUILabel> lblSitePowerSite;
			NN<UI::GUIComboBox> cboSitePowerSite;
			NN<UI::GUILabel> lblSitePowerRange;
			NN<UI::GUIComboBox> cboSitePowerYear;
			NN<UI::GUIComboBox> cboSitePowerMonth;
			NN<UI::GUIComboBox> cboSitePowerDay;
			NN<UI::GUIButton> btnSitePower;
			NN<UI::GUIPictureBox> pbSitePower;

			static void __stdcall OnAPIKeyClicked(AnyType userObj);
			static void __stdcall OnSiteListSelChg(AnyType userObj);
			static void __stdcall OnSiteEnergyClicked(AnyType userObj);
			static void __stdcall OnSiteEnergySizeChg(AnyType userObj);
			static void __stdcall OnSitePowerClicked(AnyType userObj);
			static void __stdcall OnSitePowerSizeChg(AnyType userObj);

			static Data::Timestamp GetDefaultStartTime(const Data::Timestamp &startTime, Net::SolarEdgeAPI::TimeUnit timeUnit);
			static Data::Timestamp GetDefaultEndTime(const Data::Timestamp &startTime, Net::SolarEdgeAPI::TimeUnit timeUnit);
			void UpdateSiteEnergyGraph();
			void UpdateSitePowerGraph();
		public:
			AVIRSolarEdgeForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSolarEdgeForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
