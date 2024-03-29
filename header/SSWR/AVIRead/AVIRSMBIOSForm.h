#ifndef _SM_SSWR_AVIREAD_AVIRSMBIOSFORM
#define _SM_SSWR_AVIREAD_AVIRSMBIOSFORM
#include "IO/SMBIOS.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSMBIOSForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::SMBIOS *smbios;

			NotNullPtr<UI::GUIPanel> pnlControl;
			NotNullPtr<UI::GUIButton> btnHex;
			NotNullPtr<UI::GUITextBox> txtSMBIOS;

			static void __stdcall OnHexClicked(AnyType userObj);
		public:
			AVIRSMBIOSForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSMBIOSForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
