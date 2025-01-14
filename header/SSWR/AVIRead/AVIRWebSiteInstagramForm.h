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
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpUser;
			NN<UI::GUIPanel> pnlRequest;
			NN<UI::GUILabel> lblChannelId;
			NN<UI::GUITextBox> txtChannelId;
			NN<UI::GUIButton> btnRequestUser;
			NN<UI::GUIListView> lvItems;

			NN<UI::GUITabPage> tpPage;
			NN<UI::GUIPanel> pnlPage;
			NN<UI::GUILabel> lblShortCode;
			NN<UI::GUITextBox> txtShortCode;
			NN<UI::GUIButton> btnPage;
			NN<UI::GUIListBox> lbImageURL;

			NN<Net::WebSite::WebSiteInstagramControl> ctrl;
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;

			static void __stdcall OnRequestUserClicked(AnyType userObj);
			static void __stdcall OnPageClicked(AnyType userObj);
		public:
			AVIRWebSiteInstagramForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRWebSiteInstagramForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
