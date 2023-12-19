#ifndef _SM_SSWR_AVIREAD_AVIRACMECLIENTFORM
#define _SM_SSWR_AVIREAD_AVIRACMECLIENTFORM
#include "Net/ACMEClient.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRACMEClientForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<Net::SocketFactory> sockf;
			Net::ACMEClient *client;

			NotNullPtr<UI::GUILabel> lblHost;
			NotNullPtr<UI::GUITextBox> txtHost;
			NotNullPtr<UI::GUILabel> lblKeyFile;
			NotNullPtr<UI::GUITextBox> txtKeyFile;
			NotNullPtr<UI::GUIButton> btnStart;

			NotNullPtr<UI::GUILabel> lblTermOfService;
			NotNullPtr<UI::GUITextBox> txtTermOfService;
			NotNullPtr<UI::GUILabel> lblWebsite;
			NotNullPtr<UI::GUITextBox> txtWebsite;
			NotNullPtr<UI::GUILabel> lblAccount;
			NotNullPtr<UI::GUITextBox> txtAccount;

			static void __stdcall OnStartClicked(void *userObj);
		public:
			AVIRACMEClientForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRACMEClientForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
