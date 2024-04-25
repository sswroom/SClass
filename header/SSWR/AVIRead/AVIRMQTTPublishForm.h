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
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<UI::GUILabel> lblHost;
			NN<UI::GUITextBox> txtHost;
			NN<UI::GUILabel> lblPort;
			NN<UI::GUITextBox> txtPort;
			NN<UI::GUILabel> lblUsername;
			NN<UI::GUITextBox> txtUsername;
			NN<UI::GUILabel> lblPassword;
			NN<UI::GUITextBox> txtPassword;
			NN<UI::GUILabel> lblTopic;
			NN<UI::GUITextBox> txtTopic;
			NN<UI::GUILabel> lblMessage;
			NN<UI::GUITextBox> txtMessage;
			NN<UI::GUIButton> btnPublish;
			NN<UI::GUILabel> lblStatus;
			NN<UI::GUITextBox> txtStatus;

			static void __stdcall OnPublishClicked(AnyType userObj);

		public:
			AVIRMQTTPublishForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRMQTTPublishForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
