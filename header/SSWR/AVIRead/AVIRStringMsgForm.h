#ifndef _SM_SSWR_AVIREAD_AVIRSTRINGMSGFORM
#define _SM_SSWR_AVIREAD_AVIRSTRINGMSGFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRStringMsgForm : public UI::GUIForm
		{
		private:
			NN<UI::GUIPanel> pnlButton;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUITextBox> txtMessage;

			NN<SSWR::AVIRead::AVIRCore> core;

			static void __stdcall OnOKClicked(AnyType userObj);
		public:
			AVIRStringMsgForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Text::CStringNN title, Text::CStringNN msg);
			virtual ~AVIRStringMsgForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
