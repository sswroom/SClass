#ifndef _SM_SSWR_AVIREAD_AVIRHKOFORECASTFORM
#define _SM_SSWR_AVIREAD_AVIRHKOFORECASTFORM
#include "Net/HKOWeather.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRHKOForecastForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;

			NotNullPtr<UI::GUIPanel> pnlMain;
			UI::GUILabel *lblLang;
			NotNullPtr<UI::GUIComboBox> cboLang;
			NotNullPtr<UI::GUIButton> btnReload;
			UI::GUILabel *lblReqTime;
			UI::GUITextBox *txtReqTime;
			UI::GUILabel *lblUpdateTime;
			UI::GUITextBox *txtUpdateTime;
			UI::GUILabel *lblSeaTemp;
			UI::GUITextBox *txtSeaTemp;
			UI::GUILabel *lblSeaTempPlace;
			UI::GUITextBox *txtSeaTempPlace;
			UI::GUILabel *lblSeaTempTime;
			UI::GUITextBox *txtSeaTempTime;
			UI::GUILabel *lblGeneralSituation;
			UI::GUITextBox *txtGeneralSituation;
			UI::GUIListView *lvForecast;

			static void __stdcall OnReloadClicked(void *userObj);
			void Reload(Net::HKOWeather::Language lang);
		public:
			AVIRHKOForecastForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRHKOForecastForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
