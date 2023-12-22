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
			NotNullPtr<UI::GUILabel> lblLang;
			NotNullPtr<UI::GUIComboBox> cboLang;
			NotNullPtr<UI::GUIButton> btnReload;
			NotNullPtr<UI::GUILabel> lblReqTime;
			NotNullPtr<UI::GUITextBox> txtReqTime;
			NotNullPtr<UI::GUILabel> lblUpdateTime;
			NotNullPtr<UI::GUITextBox> txtUpdateTime;
			NotNullPtr<UI::GUILabel> lblSeaTemp;
			NotNullPtr<UI::GUITextBox> txtSeaTemp;
			NotNullPtr<UI::GUILabel> lblSeaTempPlace;
			NotNullPtr<UI::GUITextBox> txtSeaTempPlace;
			NotNullPtr<UI::GUILabel> lblSeaTempTime;
			NotNullPtr<UI::GUITextBox> txtSeaTempTime;
			NotNullPtr<UI::GUILabel> lblGeneralSituation;
			NotNullPtr<UI::GUITextBox> txtGeneralSituation;
			NotNullPtr<UI::GUIListView> lvForecast;

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
