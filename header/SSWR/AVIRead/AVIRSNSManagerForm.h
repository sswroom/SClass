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
			NN<UI::GUIPanel> pnlChannel;
			NN<UI::GUIComboBox> cboChannel;
			NN<UI::GUITextBox> txtChannelId;
			NN<UI::GUIButton> btnChannelAdd;
			NN<UI::GUIListBox> lbChannels;
			NN<UI::GUIHSplitter> hspChannels;
			NN<UI::GUITabControl> tcChannels;

			NN<UI::GUITabPage> tpCurrItems;
			NN<UI::GUIListView> lvCurrItems;

			Net::SNS::SNSManager *mgr;
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;

			static void __stdcall OnChannelAddClicked(AnyType userObj);
			static void __stdcall OnChannelsSelChg(AnyType userObj);
		public:
			AVIRSNSManagerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSNSManagerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
