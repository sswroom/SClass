#ifndef _SM_SSWR_AVIREAD_AVIRRSSREADERFORM
#define _SM_SSWR_AVIREAD_AVIRRSSREADERFORM
#include "Data/ArrayListStrUTF8.h"
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
			UI::GUIPanel *pnlURL;
			UI::GUILabel *lblRecent;
			UI::GUIComboBox *cboRecent;
			UI::GUILabel *lblURL;
			UI::GUITextBox *txtURL;
			UI::GUIButton *btnRequest;
			UI::GUITabControl *tcRSS;

			UI::GUITabPage *tpInfo;
			UI::GUIListView *lvInfo;

			UI::GUITabPage *tpItems;
			UI::GUIListView *lvItems;

			Net::RSS *rss;
			Data::ArrayListStrUTF8 *rssList;
			SSWR::AVIRead::AVIRCore *core;

			static void __stdcall OnRequestClicked(void *userObj);
			static void __stdcall OnRecentSelChg(void *userObj);
			static void __stdcall OnItemsDblClick(void *userObj, UOSInt index);

			void RSSListLoad();
			void RSSListStore();
		public:
			AVIRRSSReaderForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRRSSReaderForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
