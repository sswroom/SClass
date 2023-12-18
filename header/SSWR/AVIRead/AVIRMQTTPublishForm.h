#ifndef _SM_SSWR_AVIREAD_AVIRMQTTPUBLISHFORM
#define _SM_SSWR_AVIREAD_AVIRMQTTPUBLISHFORM
#include "Net/MQTTConn.h"
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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<UI::GUILabel> lblHost;
			UI::GUITextBox *txtHost;
			NotNullPtr<UI::GUILabel> lblPort;
			UI::GUITextBox *txtPort;
			NotNullPtr<UI::GUILabel> lblUsername;
			UI::GUITextBox *txtUsername;
			NotNullPtr<UI::GUILabel> lblPassword;
			UI::GUITextBox *txtPassword;
			NotNullPtr<UI::GUILabel> lblTopic;
			UI::GUITextBox *txtTopic;
			NotNullPtr<UI::GUILabel> lblMessage;
			UI::GUITextBox *txtMessage;
			NotNullPtr<UI::GUIButton> btnPublish;
			NotNullPtr<UI::GUILabel> lblStatus;
			UI::GUITextBox *txtStatus;

			static void __stdcall OnPublishClicked(void *userObj);

		public:
			AVIRMQTTPublishForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRMQTTPublishForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
