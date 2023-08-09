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

			UI::GUILabel *lblHost;
			UI::GUITextBox *txtHost;
			UI::GUILabel *lblPort;
			UI::GUITextBox *txtPort;
			UI::GUILabel *lblFileName;
			UI::GUITextBox *txtFileName;
			UI::GUIButton *btnRecv;
			UI::GUIButton *btnSend;

		private:
			static void __stdcall OnRecvClick(void *userObj);
			static void __stdcall OnSendClick(void *userObj);
		public:
			AVIRTFTPClientForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRTFTPClientForm();

			virtual void OnMonitorChanged();
		};
	};
}
#endif
