#ifndef _SM_SSWR_AVIREAD_AVIRPUSHSERVERFORM
#define _SM_SSWR_AVIREAD_AVIRPUSHSERVERFORM
#include "Net/PushServer.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRPushServerForm : public UI::GUIForm
		{
		private:
			NN<UI::GUILabel> lblPort;
			NN<UI::GUITextBox> txtPort;
			NN<UI::GUILabel> lblAPIKey;
			NN<UI::GUITextBox> txtAPIKey;
			NN<UI::GUIButton> btnStart;

			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			Net::PushServer *svr;
			IO::LogTool log;

			static void __stdcall OnStartClicked(AnyType userObj);
		public:
			AVIRPushServerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRPushServerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
