#ifndef _SM_SSWR_AVIREAD_AVIRWEBSITE7GOGOFORM
#define _SM_SSWR_AVIREAD_AVIRWEBSITE7GOGOFORM
#include "Net/WebSite/WebSite7gogoControl.h"
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
		class AVIRWebSite7gogoForm : public UI::GUIForm
		{
		private:
			NN<UI::GUIPanel> pnlRequest;
			NN<UI::GUILabel> lblChannelId;
			NN<UI::GUITextBox> txtChannelId;
			NN<UI::GUIButton> btnRequestPage;
			NN<UI::GUIListView> lvItems;

			Net::WebSite::WebSite7gogoControl *ctrl;
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;

			static void __stdcall OnRequestPageClicked(AnyType userObj);
		public:
			AVIRWebSite7gogoForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRWebSite7gogoForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
