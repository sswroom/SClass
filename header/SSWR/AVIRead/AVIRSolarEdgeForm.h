#ifndef _SM_SSWR_AVIREAD_AVIRSOLAREDGEFORM
#define _SM_SSWR_AVIREAD_AVIRSOLAREDGEFORM
#include "Net/SolarEdgeAPI.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
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
			SSWR::AVIRead::AVIRCore *core;
			Net::SSLEngine *ssl;
			Net::SolarEdgeAPI *seAPI;
			Data::ArrayList<Net::SolarEdgeAPI::Site*> siteList;

			UI::GUIPanel *pnlAPIKey;
			UI::GUILabel *lblAPIKey;
			UI::GUITextBox *txtAPIKey;
			UI::GUIButton *btnAPIKey;
			UI::GUITabControl *tcMain;
			
			UI::GUITabPage *tpVersion;
			UI::GUILabel *lblCurrVer;
			UI::GUITextBox *txtCurrVer;
			UI::GUILabel *lblSuppVer;
			UI::GUITextBox *txtSuppVer;

			UI::GUITabPage *tpSiteList;
			UI::GUIPanel *pnlSiteList;
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

			static void __stdcall OnAPIKeyClicked(void *userObj);
			static void __stdcall OnSiteListSelChg(void *userObj);
		public:
			AVIRSolarEdgeForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRSolarEdgeForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
