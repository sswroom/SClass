#ifndef _SM_SSWR_AVIREAD_AVIRSELIOPINFORM
#define _SM_SSWR_AVIREAD_AVIRSELIOPINFORM
#include "IO/IOPin.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSelIOPinForm : public UI::GUIForm
		{
		public:
			IO::IOPin *ioPin;
			SSWR::AVIRead::AVIRCore::IOPinType ioPinType;
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::VirtualIOPinMgr *vioPinMgr;
			IO::GPIOControl *gpioPin;

			NotNullPtr<UI::GUIPanel> pnlPinType;
			NotNullPtr<UI::GUILabel> lblPinType;
			NotNullPtr<UI::GUIComboBox> cboPinType;
			NotNullPtr<UI::GUITabControl> tcConfig;

			NotNullPtr<UI::GUITabPage> tpGPIO;
			NotNullPtr<UI::GUILabel> lblGPIO;
			NotNullPtr<UI::GUIComboBox> cboGPIO;

			NotNullPtr<UI::GUITabPage> tpVirtualPin;
			NotNullPtr<UI::GUILabel> lblVirtualPin;
			NotNullPtr<UI::GUIComboBox> cboVirtualPin;

			NotNullPtr<UI::GUIPanel> pnlButtons;
			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnCancel;

			static void __stdcall OnOKClick(AnyType userObj);
			static void __stdcall OnCancelClick(AnyType userObj);
			static void __stdcall OnPinTypeChg(AnyType userObj);
		public:
			AVIRSelIOPinForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSelIOPinForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
