#ifndef _SM_SSWR_AVIREAD_AVIRSMBIOSFORM
#define _SM_SSWR_AVIREAD_AVIRSMBIOSFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"
#include "Win32/SMBIOS.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSMBIOSForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;
			Win32::SMBIOS *smbios;

			UI::GUIPanel *pnlControl;
			UI::GUIButton *btnHex;
			UI::GUITextBox *txtSMBIOS;

			static void __stdcall OnHexClicked(void *userObj);
		public:
			AVIRSMBIOSForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRSMBIOSForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
