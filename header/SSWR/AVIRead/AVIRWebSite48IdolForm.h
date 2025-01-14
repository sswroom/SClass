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
			NN<UI::GUITabControl> tcMain;
			
			NN<UI::GUITabPage> tpItems;
			NN<UI::GUIPanel> pnlRequest;
			NN<UI::GUILabel> lblPageNo;
			NN<UI::GUITextBox> txtPageNo;
			NN<UI::GUIButton> btnRequestPage;
			NN<UI::GUIListView> lvItems;

			NN<UI::GUITabPage> tpDownloadLink;
			NN<UI::GUILabel> lblVideoId;
			NN<UI::GUITextBox> txtVideoId;
			NN<UI::GUIButton> btnDownloadLink;
			NN<UI::GUILabel> lblDownloadLink;
			NN<UI::GUITextBox> txtDownloadLink;

			NN<UI::GUITabPage> tpVideoName;
			NN<UI::GUILabel> lblNameVideoId;
			NN<UI::GUITextBox> txtNameVideoId;
			NN<UI::GUIButton> btnVideoName;
			NN<UI::GUILabel> lblVideoName;
			NN<UI::GUITextBox> txtVideoName;

			NN<Net::WebSite::WebSite48IdolControl> ctrl;
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;

			static void __stdcall OnRequestPageClicked(AnyType userObj);
			static void __stdcall OnDownloadLinkClicked(AnyType userObj);
			static void __stdcall OnVideoNameClicked(AnyType userObj);
		public:
			AVIRWebSite48IdolForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRWebSite48IdolForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
