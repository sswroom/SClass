#ifndef _SM_SSWR_AVIREAD_AVIRWEBSITE48IDOLFORM
#define _SM_SSWR_AVIREAD_AVIRWEBSITE48IDOLFORM
#include "Net/WebSite/WebSite48IdolControl.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRWebSite48IdolForm : public UI::GUIForm
		{
		private:
			NotNullPtr<UI::GUITabControl> tcMain;
			
			NotNullPtr<UI::GUITabPage> tpItems;
			NotNullPtr<UI::GUIPanel> pnlRequest;
			NotNullPtr<UI::GUILabel> lblPageNo;
			NotNullPtr<UI::GUITextBox> txtPageNo;
			NotNullPtr<UI::GUIButton> btnRequestPage;
			NotNullPtr<UI::GUIListView> lvItems;

			NotNullPtr<UI::GUITabPage> tpDownloadLink;
			NotNullPtr<UI::GUILabel> lblVideoId;
			NotNullPtr<UI::GUITextBox> txtVideoId;
			NotNullPtr<UI::GUIButton> btnDownloadLink;
			NotNullPtr<UI::GUILabel> lblDownloadLink;
			NotNullPtr<UI::GUITextBox> txtDownloadLink;

			NotNullPtr<UI::GUITabPage> tpVideoName;
			NotNullPtr<UI::GUILabel> lblNameVideoId;
			NotNullPtr<UI::GUITextBox> txtNameVideoId;
			NotNullPtr<UI::GUIButton> btnVideoName;
			NotNullPtr<UI::GUILabel> lblVideoName;
			NotNullPtr<UI::GUITextBox> txtVideoName;

			Net::WebSite::WebSite48IdolControl *ctrl;
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;

			static void __stdcall OnRequestPageClicked(void *userObj);
			static void __stdcall OnDownloadLinkClicked(void *userObj);
			static void __stdcall OnVideoNameClicked(void *userObj);
		public:
			AVIRWebSite48IdolForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRWebSite48IdolForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
