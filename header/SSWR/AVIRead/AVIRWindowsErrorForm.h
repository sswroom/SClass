#ifndef _SM_SSWR_AVIREAD_AVIRWINDOWSERRORFORM
#define _SM_SSWR_AVIREAD_AVIRWINDOWSERRORFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRWindowsErrorForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;

			UI::GUILabel *lblErrorCode;
			UI::GUITextBox *txtErrorCode;

			UI::GUILabel *lblErrorName;
			UI::GUITextBox *txtErrorName;

			static void __stdcall OnErrorCodeChanged(void *userObj);

		public:
			AVIRWindowsErrorForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRWindowsErrorForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif