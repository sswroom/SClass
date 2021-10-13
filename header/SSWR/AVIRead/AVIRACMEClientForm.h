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
			SSWR::AVIRead::AVIRCore *core;
			Net::SocketFactory *sockf;
			Net::ACMEClient *client;

			UI::GUILabel *lblHost;
			UI::GUITextBox *txtHost;
			UI::GUILabel *lblKeyFile;
			UI::GUITextBox *txtKeyFile;
			UI::GUIButton *btnStart;

			UI::GUILabel *lblTermOfService;
			UI::GUITextBox *txtTermOfService;
			UI::GUILabel *lblWebsite;
			UI::GUITextBox *txtWebsite;

			static void __stdcall OnStartClicked(void *userObj);
		public:
			AVIRACMEClientForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRACMEClientForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
