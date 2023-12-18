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
			UI::GUITextBox *txtReqTime;
			NotNullPtr<UI::GUILabel> lblUpdateTime;
			UI::GUITextBox *txtUpdateTime;
			NotNullPtr<UI::GUILabel> lblGeneralSituation;
			UI::GUITextBox *txtGeneralSituation;
			NotNullPtr<UI::GUILabel> lblTCInfo;
			UI::GUITextBox *txtTCInfo;
			NotNullPtr<UI::GUILabel> lblFireDangerWarning;
			UI::GUITextBox *txtFireDangerWarning;
			NotNullPtr<UI::GUILabel> lblForecastPeriod;
			UI::GUITextBox *txtForecastPeriod;
			NotNullPtr<UI::GUILabel> lblForecastDesc;
			UI::GUITextBox *txtForecastDesc;
			NotNullPtr<UI::GUILabel> lblOutlook;
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
