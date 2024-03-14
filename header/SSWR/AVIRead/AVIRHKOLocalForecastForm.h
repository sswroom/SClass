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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;

			NotNullPtr<UI::GUILabel> lblLang;
			NotNullPtr<UI::GUIComboBox> cboLang;
			NotNullPtr<UI::GUIButton> btnReload;
			NotNullPtr<UI::GUILabel> lblReqTime;
			NotNullPtr<UI::GUITextBox> txtReqTime;
			NotNullPtr<UI::GUILabel> lblUpdateTime;
			NotNullPtr<UI::GUITextBox> txtUpdateTime;
			NotNullPtr<UI::GUILabel> lblGeneralSituation;
			NotNullPtr<UI::GUITextBox> txtGeneralSituation;
			NotNullPtr<UI::GUILabel> lblTCInfo;
			NotNullPtr<UI::GUITextBox> txtTCInfo;
			NotNullPtr<UI::GUILabel> lblFireDangerWarning;
			NotNullPtr<UI::GUITextBox> txtFireDangerWarning;
			NotNullPtr<UI::GUILabel> lblForecastPeriod;
			NotNullPtr<UI::GUITextBox> txtForecastPeriod;
			NotNullPtr<UI::GUILabel> lblForecastDesc;
			NotNullPtr<UI::GUITextBox> txtForecastDesc;
			NotNullPtr<UI::GUILabel> lblOutlook;
			NotNullPtr<UI::GUITextBox> txtOutlook;

			static void __stdcall OnReloadClicked(AnyType userObj);
			void Reload(Net::HKOWeather::Language lang);
		public:
			AVIRHKOLocalForecastForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRHKOLocalForecastForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
