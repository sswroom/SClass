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
			NotNullPtr<UI::GUITabControl> tcMain;

			NotNullPtr<UI::GUITabPage> tpInfo;
			NotNullPtr<UI::GUIListView> lvInfo;

			NotNullPtr<UI::GUITabPage> tpImage;
			NotNullPtr<UI::GUIComboBox> cboImage;
			NotNullPtr<UI::GUIPictureBoxSimple> pbImage;

			NotNullPtr<UI::GUITabPage> tpText;
			NotNullPtr<UI::GUITextBox> txtText;

			Media::ImageList *currImg;
			Net::RSSItem *rssItem;
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			static void __stdcall OnImageSelChg(AnyType userObj);
		public:
			AVIRRSSItemForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Net::RSSItem *rssItem);
			virtual ~AVIRRSSItemForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
