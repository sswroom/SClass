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
			NotNullPtr<UI::GUIPanel> pnlButton;
			UI::GUIButton *btnOK;
			UI::GUITextBox *txtMessage;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			static void __stdcall OnOKClicked(void *userObj);
		public:
			AVIRStringMsgForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Text::CStringNN title, Text::CStringNN msg);
			virtual ~AVIRStringMsgForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
