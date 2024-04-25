#ifndef _SM_SSWR_AVIREAD_AVIRWHOISCLIENTFORM
#define _SM_SSWR_AVIREAD_AVIRWHOISCLIENTFORM

#include "Net/WhoisClient.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRWHOISClientForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUIPanel> pnlRequest;
			NN<UI::GUILabel> lblIP;
			NN<UI::GUITextBox> txtIP;
			NN<UI::GUILabel> lblRespTime;
			NN<UI::GUITextBox> txtRespTime;
			NN<UI::GUIButton> btnRequest;
			NN<UI::GUIListBox> lbResponse;

			NN<Net::SocketFactory> sockf;

			static void __stdcall OnRequestClicked(AnyType userObj);
		public:
			AVIRWHOISClientForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRWHOISClientForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
