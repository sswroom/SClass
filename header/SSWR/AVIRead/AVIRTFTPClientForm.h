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
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUILabel> lblHost;
			NN<UI::GUITextBox> txtHost;
			NN<UI::GUILabel> lblPort;
			NN<UI::GUITextBox> txtPort;
			NN<UI::GUILabel> lblFileName;
			NN<UI::GUITextBox> txtFileName;
			NN<UI::GUIButton> btnRecv;
			NN<UI::GUIButton> btnSend;

		private:
			static void __stdcall OnRecvClick(AnyType userObj);
			static void __stdcall OnSendClick(AnyType userObj);
		public:
			AVIRTFTPClientForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRTFTPClientForm();

			virtual void OnMonitorChanged();
		};
	};
}
#endif
