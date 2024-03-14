#ifndef _SM_SSWR_AVIREAD_AVIRTFTPCLIENTFORM
#define _SM_SSWR_AVIREAD_AVIRTFTPCLIENTFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRTFTPClientForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			NotNullPtr<UI::GUILabel> lblHost;
			NotNullPtr<UI::GUITextBox> txtHost;
			NotNullPtr<UI::GUILabel> lblPort;
			NotNullPtr<UI::GUITextBox> txtPort;
			NotNullPtr<UI::GUILabel> lblFileName;
			NotNullPtr<UI::GUITextBox> txtFileName;
			NotNullPtr<UI::GUIButton> btnRecv;
			NotNullPtr<UI::GUIButton> btnSend;

		private:
			static void __stdcall OnRecvClick(AnyType userObj);
			static void __stdcall OnSendClick(AnyType userObj);
		public:
			AVIRTFTPClientForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRTFTPClientForm();

			virtual void OnMonitorChanged();
		};
	};
}
#endif
