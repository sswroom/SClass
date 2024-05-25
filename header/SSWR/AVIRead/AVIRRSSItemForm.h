#ifndef _SM_SSWR_AVIREAD_AVIRRSSITEMFORM
#define _SM_SSWR_AVIREAD_AVIRRSSITEMFORM
#include "Media/ImageList.h"
#include "Net/RSS.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPictureBoxSimple.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRRSSItemForm : public UI::GUIForm
		{
		private:
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpInfo;
			NN<UI::GUIListView> lvInfo;

			NN<UI::GUITabPage> tpImage;
			NN<UI::GUIComboBox> cboImage;
			NN<UI::GUIPictureBoxSimple> pbImage;

			NN<UI::GUITabPage> tpText;
			NN<UI::GUITextBox> txtText;

			Optional<Media::ImageList> currImg;
			NN<Net::RSSItem> rssItem;
			NN<SSWR::AVIRead::AVIRCore> core;

			static void __stdcall OnImageSelChg(AnyType userObj);
		public:
			AVIRRSSItemForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Net::RSSItem> rssItem);
			virtual ~AVIRRSSItemForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
