#ifndef _SM_SSWR_AVIREAD_AVIRSMBIOSFORM
#define _SM_SSWR_AVIREAD_AVIRSMBIOSFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSMBIOSForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;

			UI::GUITextBox *txtSMBIOS;

		public:
			AVIRSMBIOSForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRSMBIOSForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
