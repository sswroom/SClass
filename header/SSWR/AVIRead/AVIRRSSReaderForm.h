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
			UI::GUILabel *lblRecent;
			UI::GUIComboBox *cboRecent;
			UI::GUILabel *lblURL;
			UI::GUITextBox *txtURL;
			UI::GUIButton *btnRequest;
			UI::GUITabControl *tcRSS;

			NotNullPtr<UI::GUITabPage> tpInfo;
			UI::GUIListView *lvInfo;

			NotNullPtr<UI::GUITabPage> tpItems;
			UI::GUIListView *lvItems;

			Net::RSS *rss;
			Data::ArrayListStringNN rssList;
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Net::SSLEngine *ssl;

			static void __stdcall OnRequestClicked(void *userObj);
			static void __stdcall OnRecentSelChg(void *userObj);
			static void __stdcall OnItemsDblClick(void *userObj, UOSInt index);

			void RSSListLoad();
			void RSSListStore();
		public:
			AVIRRSSReaderForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRRSSReaderForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
