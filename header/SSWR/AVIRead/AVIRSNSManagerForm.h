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
			NotNullPtr<UI::GUIPanel> pnlChannel;
			NotNullPtr<UI::GUIComboBox> cboChannel;
			NotNullPtr<UI::GUITextBox> txtChannelId;
			NotNullPtr<UI::GUIButton> btnChannelAdd;
			UI::GUIListBox *lbChannels;
			NotNullPtr<UI::GUIHSplitter> hspChannels;
			UI::GUITabControl *tcChannels;

			NotNullPtr<UI::GUITabPage> tpCurrItems;
			UI::GUIListView *lvCurrItems;

			Net::SNS::SNSManager *mgr;
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;

			static void __stdcall OnChannelAddClicked(void *userObj);
			static void __stdcall OnChannelsSelChg(void *userObj);
		public:
			AVIRSNSManagerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSNSManagerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
