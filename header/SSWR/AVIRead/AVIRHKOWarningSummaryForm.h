#ifndef _SM_SSWR_AVIREAD_AVIRHKOWARNINGSUMMARYFORM
#define _SM_SSWR_AVIREAD_AVIRHKOWARNINGSUMMARYFORM
#include "Net/HKOWeather.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRHKOWarningSummaryForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;

			NotNullPtr<UI::GUIPanel> pnlReqTime;
			NotNullPtr<UI::GUILabel> lblReqTime;
			NotNullPtr<UI::GUITextBox> txtReqTime;
			NotNullPtr<UI::GUIListView> lvWarning;

			void Reload();
		public:
			AVIRHKOWarningSummaryForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRHKOWarningSummaryForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
