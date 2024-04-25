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
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Net::SocketFactory> sockf;
			Net::ACMEClient *client;

			NN<UI::GUILabel> lblHost;
			NN<UI::GUITextBox> txtHost;
			NN<UI::GUILabel> lblKeyFile;
			NN<UI::GUITextBox> txtKeyFile;
			NN<UI::GUIButton> btnStart;

			NN<UI::GUILabel> lblTermOfService;
			NN<UI::GUITextBox> txtTermOfService;
			NN<UI::GUILabel> lblWebsite;
			NN<UI::GUITextBox> txtWebsite;
			NN<UI::GUILabel> lblAccount;
			NN<UI::GUITextBox> txtAccount;

			static void __stdcall OnStartClicked(AnyType userObj);
		public:
			AVIRACMEClientForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRACMEClientForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
