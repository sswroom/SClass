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
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;

			NN<UI::GUIPanel> pnlMain;
			NN<UI::GUILabel> lblLang;
			NN<UI::GUIComboBox> cboLang;
			NN<UI::GUIButton> btnReload;
			NN<UI::GUILabel> lblReqTime;
			NN<UI::GUITextBox> txtReqTime;
			NN<UI::GUILabel> lblUpdateTime;
			NN<UI::GUITextBox> txtUpdateTime;
			NN<UI::GUILabel> lblSeaTemp;
			NN<UI::GUITextBox> txtSeaTemp;
			NN<UI::GUILabel> lblSeaTempPlace;
			NN<UI::GUITextBox> txtSeaTempPlace;
			NN<UI::GUILabel> lblSeaTempTime;
			NN<UI::GUITextBox> txtSeaTempTime;
			NN<UI::GUILabel> lblGeneralSituation;
			NN<UI::GUITextBox> txtGeneralSituation;
			NN<UI::GUIListView> lvForecast;

			static void __stdcall OnReloadClicked(AnyType userObj);
			void Reload(Net::HKOWeather::Language lang);
		public:
			AVIRHKOForecastForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRHKOForecastForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
