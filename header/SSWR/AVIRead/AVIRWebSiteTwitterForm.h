#ifndef _SM_SSWR_AVIREAD_AVIRWEBSITETWITTERFORM
#define _SM_SSWR_AVIREAD_AVIRWEBSITETWITTERFORM
#include "Net/WebSite/WebSiteTwitterControl.h"
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
		class AVIRWebSiteTwitterForm : public UI::GUIForm
		{
		private:
			NN<UI::GUIPanel> pnlRequest;
			NN<UI::GUILabel> lblChannelId;
			NN<UI::GUITextBox> txtChannelId;
			NN<UI::GUIButton> btnRequestPage;
			NN<UI::GUIListView> lvItems;

			NN<Net::WebSite::WebSiteTwitterControl> ctrl;
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;

			static void __stdcall OnRequestPageClicked(AnyType userObj);
		public:
			AVIRWebSiteTwitterForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRWebSiteTwitterForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
