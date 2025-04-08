#ifndef _SM_SSWR_AVIREAD_AVIRWEBSITE7GOGOFORM
#define _SM_SSWR_AVIREAD_AVIRWEBSITE7GOGOFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRWebPushForm : public UI::GUIForm
		{
		private:
			NN<UI::GUIPanel> pnlRequest;
			NN<UI::GUILabel> lblChannelId;
			NN<UI::GUITextBox> txtChannelId;
			NN<UI::GUIButton> btnRequestPage;
			NN<UI::GUIListView> lvItems;

			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;

		public:
			AVIRWebPushForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRWebPushForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
