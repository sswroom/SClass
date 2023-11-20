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
			UI::GUITabControl *tcMain;

			NotNullPtr<UI::GUITabPage> tpInfo;
			UI::GUIListView *lvInfo;

			NotNullPtr<UI::GUITabPage> tpImage;
			UI::GUIComboBox *cboImage;
			UI::GUIPictureBoxSimple *pbImage;

			NotNullPtr<UI::GUITabPage> tpText;
			UI::GUITextBox *txtText;

			Media::ImageList *currImg;
			Net::RSSItem *rssItem;
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			static void __stdcall OnImageSelChg(void *userObj);
		public:
			AVIRRSSItemForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Net::RSSItem *rssItem);
			virtual ~AVIRRSSItemForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
