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
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;

			NN<UI::GUIPanel> pnlReqTime;
			NN<UI::GUILabel> lblReqTime;
			NN<UI::GUITextBox> txtReqTime;
			NN<UI::GUIListView> lvWarning;

			void Reload();
		public:
			AVIRHKOWarningSummaryForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRHKOWarningSummaryForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
