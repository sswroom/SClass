#ifndef _SM_SSWR_AVIREAD_AVIRMQTTPUBLISHFORM
#define _SM_SSWR_AVIREAD_AVIRMQTTPUBLISHFORM
#include "Net/MQTTClient.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
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
		class AVIRMQTTPublishForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;
			UI::GUILabel *lblHost;
			UI::GUITextBox *txtHost;
			UI::GUILabel *lblPort;
			UI::GUITextBox *txtPort;
			UI::GUILabel *lblUsername;
			UI::GUITextBox *txtUsername;
			UI::GUILabel *lblPassword;
			UI::GUITextBox *txtPassword;
			UI::GUILabel *lblTopic;
			UI::GUITextBox *txtTopic;
			UI::GUILabel *lblMessage;
			UI::GUITextBox *txtMessage;
			UI::GUIButton *btnPublish;
			UI::GUILabel *lblStatus;
			UI::GUITextBox *txtStatus;

			static void __stdcall OnPublishClicked(void *userObj);

		public:
			AVIRMQTTPublishForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRMQTTPublishForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
