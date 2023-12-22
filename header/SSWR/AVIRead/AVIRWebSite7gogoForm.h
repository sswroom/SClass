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
			NotNullPtr<UI::GUIPanel> pnlRequest;
			NotNullPtr<UI::GUILabel> lblChannelId;
			NotNullPtr<UI::GUITextBox> txtChannelId;
			NotNullPtr<UI::GUIButton> btnRequestPage;
			NotNullPtr<UI::GUIListView> lvItems;

			Net::WebSite::WebSite7gogoControl *ctrl;
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;

			static void __stdcall OnRequestPageClicked(void *userObj);
		public:
			AVIRWebSite7gogoForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRWebSite7gogoForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
