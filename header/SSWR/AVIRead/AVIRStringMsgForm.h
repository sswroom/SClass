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
			UI::GUIPanel *pnlButton;
			UI::GUIButton *btnOK;
			UI::GUITextBox *txtMessage;

			SSWR::AVIRead::AVIRCore *core;

			static void __stdcall OnOKClicked(void *userObj);
		public:
			AVIRStringMsgForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, const UTF8Char *title, Text::CString msg);
			virtual ~AVIRStringMsgForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
