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

			UI::GUILabel *lblLang;
			UI::GUIComboBox *cboLang;
			NotNullPtr<UI::GUIButton> btnReload;
			UI::GUILabel *lblReqTime;
			UI::GUITextBox *txtReqTime;
			UI::GUILabel *lblUpdateTime;
			UI::GUITextBox *txtUpdateTime;
			UI::GUILabel *lblGeneralSituation;
			UI::GUITextBox *txtGeneralSituation;
			UI::GUILabel *lblTCInfo;
			UI::GUITextBox *txtTCInfo;
			UI::GUILabel *lblFireDangerWarning;
			UI::GUITextBox *txtFireDangerWarning;
			UI::GUILabel *lblForecastPeriod;
			UI::GUITextBox *txtForecastPeriod;
			UI::GUILabel *lblForecastDesc;
			UI::GUITextBox *txtForecastDesc;
			UI::GUILabel *lblOutlook;
			UI::GUITextBox *txtOutlook;

			static void __stdcall OnReloadClicked(void *userObj);
			void Reload(Net::HKOWeather::Language lang);
		public:
			AVIRHKOLocalForecastForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRHKOLocalForecastForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
