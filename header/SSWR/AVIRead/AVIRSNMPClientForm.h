#ifndef _SM_SSWR_AVIREAD_AVIRSNMPCLIENTFORM
#define _SM_SSWR_AVIREAD_AVIRSNMPCLIENTFORM
#include "Net/SNMPClient.h"
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
#include "UI/ListBoxLogger.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSNMPClientForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Net::SNMPClient *cli;

			NN<UI::GUIPanel> pnlRequest;
			NN<UI::GUILabel> lblAgent;
			NN<UI::GUITextBox> txtAgent;
			NN<UI::GUILabel> lblCommunity;
			NN<UI::GUITextBox> txtCommunity;
			NN<UI::GUILabel> lblOID;
			NN<UI::GUITextBox> txtOID;
			NN<UI::GUILabel> lblCommandType;
			NN<UI::GUIComboBox> cboCommandType;
			NN<UI::GUIButton> btnRequest;
			NN<UI::GUIListView> lvResults;

			static void __stdcall OnRequestClicked(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);

		public:
			AVIRSNMPClientForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSNMPClientForm();

			virtual void OnMonitorChanged();

			Bool IsError();
		};
	}
}
#endif
