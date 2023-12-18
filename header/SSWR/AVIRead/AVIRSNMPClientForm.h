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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Net::SNMPClient *cli;

			NotNullPtr<UI::GUIPanel> pnlRequest;
			UI::GUILabel *lblAgent;
			UI::GUITextBox *txtAgent;
			UI::GUILabel *lblCommunity;
			UI::GUITextBox *txtCommunity;
			UI::GUILabel *lblOID;
			UI::GUITextBox *txtOID;
			UI::GUILabel *lblCommandType;
			NotNullPtr<UI::GUIComboBox> cboCommandType;
			NotNullPtr<UI::GUIButton> btnRequest;
			UI::GUIListView *lvResults;

			static void __stdcall OnRequestClicked(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);

		public:
			AVIRSNMPClientForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSNMPClientForm();

			virtual void OnMonitorChanged();

			Bool IsError();
		};
	};
};
#endif
