#ifndef _SM_SSWR_AVIREAD_AVIRWEBSITEINSTAGRAMFORM
#define _SM_SSWR_AVIREAD_AVIRWEBSITEINSTAGRAMFORM
#include "Net/WebSite/WebSiteInstagramControl.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRWebSiteInstagramForm : public UI::GUIForm
		{
		private:
			UI::GUITabControl *tcMain;

			UI::GUITabPage *tpUser;
			UI::GUIPanel *pnlRequest;
			UI::GUILabel *lblChannelId;
			UI::GUITextBox *txtChannelId;
			UI::GUIButton *btnRequestUser;
			UI::GUIListView *lvItems;

			UI::GUITabPage *tpPage;
			UI::GUIPanel *pnlPage;
			UI::GUILabel *lblShortCode;
			UI::GUITextBox *txtShortCode;
			UI::GUIButton *btnPage;
			UI::GUIListBox *lbImageURL;

			Net::WebSite::WebSiteInstagramControl *ctrl;
			SSWR::AVIRead::AVIRCore *core;
			Net::SSLEngine *ssl;

			static void __stdcall OnRequestUserClicked(void *userObj);
			static void __stdcall OnPageClicked(void *userObj);
		public:
			AVIRWebSiteInstagramForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRWebSiteInstagramForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
