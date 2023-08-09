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
			UI::GUIPanel *pnlRequest;
			UI::GUILabel *lblChannelId;
			UI::GUITextBox *txtChannelId;
			UI::GUIButton *btnRequestPage;
			UI::GUIListView *lvItems;

			Net::WebSite::WebSiteTwitterControl *ctrl;
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Net::SSLEngine *ssl;

			static void __stdcall OnRequestPageClicked(void *userObj);
		public:
			AVIRWebSiteTwitterForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRWebSiteTwitterForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
