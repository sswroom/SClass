#ifndef _SM_SSWR_AVIREAD_AVIRSNSMANAGERFORM
#define _SM_SSWR_AVIREAD_AVIRSNSMANAGERFORM
#include "Net/SNS/SNSManager.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
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
		class AVIRSNSManagerForm : public UI::GUIForm
		{
		private:
			UI::GUIPanel *pnlChannel;
			UI::GUIComboBox *cboChannel;
			UI::GUITextBox *txtChannelId;
			UI::GUIButton *btnChannelAdd;
			UI::GUIListBox *lbChannels;
			UI::GUIHSplitter *hspChannels;
			UI::GUITabControl *tcChannels;

			UI::GUITabPage *tpCurrItems;
			UI::GUIListView *lvCurrItems;

			Net::SNS::SNSManager *mgr;
			SSWR::AVIRead::AVIRCore *core;

			static void __stdcall OnChannelAddClicked(void *userObj);
			static void __stdcall OnChannelsSelChg(void *userObj);
		public:
			AVIRSNSManagerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRSNSManagerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
