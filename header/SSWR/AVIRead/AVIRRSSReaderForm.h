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
			NN<UI::GUIPanel> pnlURL;
			NN<UI::GUILabel> lblRecent;
			NN<UI::GUIComboBox> cboRecent;
			NN<UI::GUILabel> lblURL;
			NN<UI::GUITextBox> txtURL;
			NN<UI::GUIButton> btnRequest;
			NN<UI::GUITabControl> tcRSS;

			NN<UI::GUITabPage> tpInfo;
			NN<UI::GUIListView> lvInfo;

			NN<UI::GUITabPage> tpItems;
			NN<UI::GUIListView> lvItems;

			Optional<Net::RSS> rss;
			Data::ArrayListStringNN rssList;
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;

			static void __stdcall OnRequestClicked(AnyType userObj);
			static void __stdcall OnRecentSelChg(AnyType userObj);
			static void __stdcall OnItemsDblClick(AnyType userObj, UIntOS index);

			void RSSListLoad();
			void RSSListStore();
		public:
			AVIRRSSReaderForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRRSSReaderForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
