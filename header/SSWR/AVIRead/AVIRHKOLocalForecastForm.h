#ifndef _SM_SSWR_AVIREAD_AVIRHKOLOCALFORECASTFORM
#define _SM_SSWR_AVIREAD_AVIRHKOLOCALFORECASTFORM
#include "Net/HKOWeather.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRHKOLocalForecastForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;

			NN<UI::GUILabel> lblLang;
			NN<UI::GUIComboBox> cboLang;
			NN<UI::GUIButton> btnReload;
			NN<UI::GUILabel> lblReqTime;
			NN<UI::GUITextBox> txtReqTime;
			NN<UI::GUILabel> lblUpdateTime;
			NN<UI::GUITextBox> txtUpdateTime;
			NN<UI::GUILabel> lblGeneralSituation;
			NN<UI::GUITextBox> txtGeneralSituation;
			NN<UI::GUILabel> lblTCInfo;
			NN<UI::GUITextBox> txtTCInfo;
			NN<UI::GUILabel> lblFireDangerWarning;
			NN<UI::GUITextBox> txtFireDangerWarning;
			NN<UI::GUILabel> lblForecastPeriod;
			NN<UI::GUITextBox> txtForecastPeriod;
			NN<UI::GUILabel> lblForecastDesc;
			NN<UI::GUITextBox> txtForecastDesc;
			NN<UI::GUILabel> lblOutlook;
			NN<UI::GUITextBox> txtOutlook;

			static void __stdcall OnReloadClicked(AnyType userObj);
			void Reload(Net::HKOWeather::Language lang);
		public:
			AVIRHKOLocalForecastForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRHKOLocalForecastForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
