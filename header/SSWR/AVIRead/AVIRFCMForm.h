#ifndef _SM_SSWR_AVIREAD_AVIRFCMFORM
#define _SM_SSWR_AVIREAD_AVIRFCMFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRFCMForm : public UI::GUIForm
		{
		private:
			NN<UI::GUILabel> lblAPIKey;
			NN<UI::GUITextBox> txtAPIKey;
			NN<UI::GUILabel> lblDeviceToken;
			NN<UI::GUITextBox> txtDeviceToken;
			NN<UI::GUILabel> lblMessage;
			NN<UI::GUITextBox> txtMessage;
			NN<UI::GUIButton> btnSend;
			NN<UI::GUILabel> lblStatus;
			NN<UI::GUITextBox> txtStatus;

			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;

			static void __stdcall OnSendClicked(AnyType userObj);
		public:
			AVIRFCMForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRFCMForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
