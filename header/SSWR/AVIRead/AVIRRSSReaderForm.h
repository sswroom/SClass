#ifndef _SM_SSWR_AVIREAD_AVIRRSSREADERFORM
#define _SM_SSWR_AVIREAD_AVIRRSSREADERFORM
#include "Data/ArrayListStringNN.h"
#include "Net/RSS.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
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
		class AVIRRSSReaderForm : public UI::GUIForm
		{
		private:
			NotNullPtr<UI::GUIPanel> pnlURL;
			NotNullPtr<UI::GUILabel> lblRecent;
			NotNullPtr<UI::GUIComboBox> cboRecent;
			NotNullPtr<UI::GUILabel> lblURL;
			NotNullPtr<UI::GUITextBox> txtURL;
			NotNullPtr<UI::GUIButton> btnRequest;
			NotNullPtr<UI::GUITabControl> tcRSS;

			NotNullPtr<UI::GUITabPage> tpInfo;
			NotNullPtr<UI::GUIListView> lvInfo;

			NotNullPtr<UI::GUITabPage> tpItems;
			NotNullPtr<UI::GUIListView> lvItems;

			Net::RSS *rss;
			Data::ArrayListStringNN rssList;
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;

			static void __stdcall OnRequestClicked(AnyType userObj);
			static void __stdcall OnRecentSelChg(AnyType userObj);
			static void __stdcall OnItemsDblClick(AnyType userObj, UOSInt index);

			void RSSListLoad();
			void RSSListStore();
		public:
			AVIRRSSReaderForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRRSSReaderForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
